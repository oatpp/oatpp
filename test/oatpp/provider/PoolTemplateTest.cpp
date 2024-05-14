/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "PoolTemplateTest.hpp"
#include <future>

#include "oatpp/provider/Pool.hpp"
#include "oatpp/async/Executor.hpp"

namespace oatpp { namespace provider {

namespace {

struct Resource {
};

class TestProvider : public oatpp::provider::Provider<Resource> {
private:

  class ResourceInvalidator : public oatpp::provider::Invalidator<Resource> {
  public:

    void invalidate(const std::shared_ptr<Resource>& resource) override {
      (void) resource;
    }

  };

private:
  std::shared_ptr<ResourceInvalidator> m_invalidator;
public:

  oatpp::provider::ResourceHandle<Resource> get() override {
    return oatpp::provider::ResourceHandle<Resource>(std::make_shared<Resource>(), m_invalidator);
  }

  async::CoroutineStarterForResult<const oatpp::provider::ResourceHandle<Resource> &> getAsync() override {

    class GetCoroutine : public oatpp::async::CoroutineWithResult<GetCoroutine, const oatpp::provider::ResourceHandle<Resource>&> {
    private:
      TestProvider* m_provider;
    public:

      GetCoroutine(TestProvider* provider)
        : m_provider(provider)
      {}

      Action act() override {
        return _return(oatpp::provider::ResourceHandle<Resource>(std::make_shared<Resource>(), m_provider->m_invalidator));
      }

    };

    return GetCoroutine::startForResult(this);
  }

  void stop() override {
    OATPP_LOGd("TestProvider", "stop()")
  }

};

struct AcquisitionProxy : public oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy> {

  AcquisitionProxy(const oatpp::provider::ResourceHandle<Resource>& resource, const std::shared_ptr<PoolInstance>& pool)
    : oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy>(resource, pool)
  {}

};

struct TestPool : public oatpp::provider::PoolTemplate<Resource, AcquisitionProxy> {

  TestPool(const std::shared_ptr<TestProvider>& provider, v_int64 maxResources, v_int64 maxResourceTTL, const std::chrono::duration<v_int64, std::micro>& timeout)
    : oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>(provider, maxResources, maxResourceTTL, timeout)
  {}

  static oatpp::provider::ResourceHandle<Resource> get(const std::shared_ptr<PoolTemplate>& _this) {
    return oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>::get(_this);
  }

  static async::CoroutineStarterForResult<const oatpp::provider::ResourceHandle<Resource>&> getAsync(const std::shared_ptr<PoolTemplate>& _this) {
    return oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>::getAsync(_this);
  }

  static std::shared_ptr<PoolTemplate> createShared(const std::shared_ptr<TestProvider>& provider,
                                                    v_int64 maxResources,
                                                    const std::chrono::duration<v_int64, std::micro>& maxResourceTTL,
                                                    const std::chrono::duration<v_int64, std::micro>& timeout) {
    auto ptr = std::make_shared<TestPool>(provider, maxResources, maxResourceTTL.count(), timeout);
    startCleanupTask(ptr);
    return ptr;
  }

};

class ClientCoroutine : public oatpp::async::Coroutine<ClientCoroutine> {
private:
  std::shared_ptr<oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>> m_pool;
  std::promise<oatpp::provider::ResourceHandle<Resource>>* m_promise;
public:

  ClientCoroutine(const std::shared_ptr<oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>>& pool, std::promise<oatpp::provider::ResourceHandle<Resource>>* promise)
    : m_pool(pool)
    , m_promise(promise)
  {}

  Action act() override {
    return TestPool::getAsync(m_pool).callbackTo(&ClientCoroutine::onGet);
  }

  Action onGet(const oatpp::provider::ResourceHandle<Resource>& resource) {
    m_promise->set_value(resource);
    return finish();
  }

};

}

void PoolTemplateTest::onRun() {
  const auto provider = std::make_shared<TestProvider>();
  const v_int64 maxResources = 1;

  {
    OATPP_LOGd(TAG, "Synchronously with timeout")
    auto poolTemplate = TestPool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds(500));

    oatpp::provider::ResourceHandle<Resource> resource = TestPool::get(poolTemplate);
    OATPP_ASSERT(resource != nullptr)
    OATPP_ASSERT(TestPool::get(poolTemplate) == nullptr)
    
    poolTemplate->stop();
    
    OATPP_ASSERT(TestPool::get(poolTemplate) == nullptr)
  }
  {
    OATPP_LOGd(TAG, "Synchronously without timeout")
    auto poolTemplate = TestPool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds::zero());

    oatpp::provider::ResourceHandle<Resource> resource = TestPool::get(poolTemplate);
    OATPP_ASSERT(resource != nullptr)
    std::future<oatpp::provider::ResourceHandle<Resource>> futureResource = std::async(std::launch::async, [&poolTemplate]() {
      return TestPool::get(poolTemplate);
    });
    OATPP_ASSERT(futureResource.wait_for(std::chrono::seconds(1)) == std::future_status::timeout)

    poolTemplate->stop();

    OATPP_ASSERT(TestPool::get(poolTemplate) == nullptr)
  }
  {
    OATPP_LOGd(TAG, "Asynchronously with timeout")
    oatpp::async::Executor executor(1, 1, 1);
    auto poolTemplate = TestPool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds(500));

    oatpp::provider::ResourceHandle<Resource> resourceHandle;
    {
      std::promise<oatpp::provider::ResourceHandle<Resource>> promise;
      auto future = promise.get_future();
      executor.execute<ClientCoroutine>(poolTemplate, &promise);
      resourceHandle = future.get();
      OATPP_ASSERT(resourceHandle != nullptr)
      OATPP_ASSERT(resourceHandle.object != nullptr)
      OATPP_ASSERT(resourceHandle.invalidator != nullptr)
    }
    {
      std::promise<oatpp::provider::ResourceHandle<Resource>> promise;
      auto future = promise.get_future();
      executor.execute<ClientCoroutine>(poolTemplate, &promise);
      resourceHandle = future.get();
      OATPP_ASSERT(resourceHandle == nullptr)
      OATPP_ASSERT(resourceHandle.object == nullptr)
      OATPP_ASSERT(resourceHandle.invalidator == nullptr)
    }

    poolTemplate->stop();
    executor.stop();
    executor.join();
  }
  {
    OATPP_LOGd(TAG, "Asynchronously without timeout")
    oatpp::async::Executor executor(1, 1, 1);
    auto poolTemplate = TestPool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds::zero());

    oatpp::provider::ResourceHandle<Resource> resource = TestPool::get(poolTemplate);
    OATPP_ASSERT(resource != nullptr)

    std::promise<oatpp::provider::ResourceHandle<Resource>> promise;
    auto future = promise.get_future();
    executor.execute<ClientCoroutine>(poolTemplate, &promise);
    OATPP_ASSERT(future.wait_for(std::chrono::seconds(1)) == std::future_status::timeout)

    poolTemplate->stop();
    executor.stop();
    executor.join();
  }
}

}}
