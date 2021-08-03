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

#include "oatpp/core/provider/Pool.hpp"
#include "oatpp/core/async/Executor.hpp"

namespace oatpp { namespace test { namespace core { namespace provider {

namespace {

struct Resource {
};

class Provider : public oatpp::provider::Provider<Resource> {
public:

  std::shared_ptr<Resource> get() override {
    return std::make_shared<Resource>();
  }

  async::CoroutineStarterForResult<const std::shared_ptr<Resource> &> getAsync() override {

    class GetCoroutine : public oatpp::async::CoroutineWithResult<GetCoroutine, const std::shared_ptr<Resource>&> {
    private:
      Provider* m_provider;
    public:

      GetCoroutine(Provider* provider)
        : m_provider(provider)
      {}

      Action act() override {
        return _return(std::make_shared<Resource>());
      }

    };

    return GetCoroutine::startForResult(this);
  }

  void invalidate(const std::shared_ptr<Resource>& resource) override {
    (void) resource;
  }

  void stop() override {
    OATPP_LOGD("Provider", "stop()");
  }

};

struct AcquisitionProxy : public oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy> {

  AcquisitionProxy(const std::shared_ptr<Resource>& resource, const std::shared_ptr<PoolInstance>& pool)
    : oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy>(resource, pool)
  {}

};

struct Pool : public oatpp::provider::PoolTemplate<Resource, AcquisitionProxy> {

  Pool(const std::shared_ptr<Provider>& provider, v_int64 maxResources, v_int64 maxResourceTTL, const std::chrono::duration<v_int64, std::micro>& timeout)
    : oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>(provider, maxResources, maxResourceTTL, timeout)
  {}

  static std::shared_ptr<Resource> get(const std::shared_ptr<PoolTemplate>& _this) {
    return oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>::get(_this);
  }

  static async::CoroutineStarterForResult<const std::shared_ptr<Resource>&> getAsync(const std::shared_ptr<PoolTemplate>& _this) {
    return oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>::getAsync(_this);
  }

  static std::shared_ptr<PoolTemplate> createShared(const std::shared_ptr<Provider>& provider,
                                                    v_int64 maxResources,
                                                    const std::chrono::duration<v_int64, std::micro>& maxResourceTTL,
                                                    const std::chrono::duration<v_int64, std::micro>& timeout) {
    auto ptr = std::make_shared<Pool>(provider, maxResources, maxResourceTTL.count(), timeout);
    startCleanupTask(ptr);
    return ptr;
  }

};

class ClientCoroutine : public oatpp::async::Coroutine<ClientCoroutine> {
private:
  std::shared_ptr<oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>> m_pool;
  std::promise<std::shared_ptr<Resource>>* m_promise;
public:

  ClientCoroutine(const std::shared_ptr<oatpp::provider::PoolTemplate<Resource, AcquisitionProxy>>& pool, std::promise<std::shared_ptr<Resource>>* promise)
    : m_pool(pool)
    , m_promise(promise)
  {}

  Action act() override {
    return Pool::getAsync(m_pool).callbackTo(&ClientCoroutine::onGet);
  }

  Action onGet(const std::shared_ptr<Resource>& resource) {
    m_promise->set_value(resource);
    return finish();
  }

};

}

void PoolTemplateTest::onRun() {
  const auto provider = std::make_shared<Provider>();
  const v_int64 maxResources = 1;

  {
    OATPP_LOGD(TAG, "Synchronously with timeout");
    auto poolTemplate = Pool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds(500));
    
    std::shared_ptr<Resource> resource = Pool::get(poolTemplate);
    OATPP_ASSERT(resource != nullptr);
    OATPP_ASSERT(Pool::get(poolTemplate) == nullptr);
    
    poolTemplate->stop();
    
    OATPP_ASSERT(Pool::get(poolTemplate) == nullptr);
  }
  {
    OATPP_LOGD(TAG, "Synchronously without timeout");
    auto poolTemplate = Pool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds::zero());

    std::shared_ptr<Resource> resource = Pool::get(poolTemplate);
    OATPP_ASSERT(resource != nullptr);
    std::future<std::shared_ptr<Resource>> futureResource = std::async(std::launch::async, [&poolTemplate]() {
      return Pool::get(poolTemplate);
    });
    OATPP_ASSERT(futureResource.wait_for(std::chrono::seconds(1)) == std::future_status::timeout);

    poolTemplate->stop();

    OATPP_ASSERT(Pool::get(poolTemplate) == nullptr);
  }
  {
    OATPP_LOGD(TAG, "Asynchronously with timeout");
    oatpp::async::Executor executor(1, 1, 1);
    auto poolTemplate = Pool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds(500));

    std::shared_ptr<Resource> resource;
    {
      std::promise<std::shared_ptr<Resource>> promise;
      auto future = promise.get_future();
      executor.execute<ClientCoroutine>(poolTemplate, &promise);
      resource = future.get();
      OATPP_ASSERT(resource != nullptr);
    }
    {
      std::promise<std::shared_ptr<Resource>> promise;
      auto future = promise.get_future();
      executor.execute<ClientCoroutine>(poolTemplate, &promise);
      OATPP_ASSERT(future.get() == nullptr);
    }

    poolTemplate->stop();
    executor.stop();
    executor.join();
  }
  {
    OATPP_LOGD(TAG, "Asynchronously without timeout");
    oatpp::async::Executor executor(1, 1, 1);
    auto poolTemplate = Pool::createShared(provider, maxResources, std::chrono::seconds(10), std::chrono::milliseconds::zero());

    std::shared_ptr<Resource> resource = Pool::get(poolTemplate);
    OATPP_ASSERT(resource != nullptr);

    std::promise<std::shared_ptr<Resource>> promise;
    auto future = promise.get_future();
    executor.execute<ClientCoroutine>(poolTemplate, &promise);
    OATPP_ASSERT(future.wait_for(std::chrono::seconds(1)) == std::future_status::timeout);

    poolTemplate->stop();
    executor.stop();
    executor.join();
  }
}

}}}}
