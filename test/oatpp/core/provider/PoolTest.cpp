/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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

#include "PoolTest.hpp"

#include "oatpp/core/provider/Pool.hpp"
#include "oatpp/core/async/Executor.hpp"

#include <thread>

namespace oatpp { namespace test { namespace core { namespace provider {

namespace {

class Resource {
public:

  virtual ~Resource() = default;

  virtual v_int64 myId() = 0;

};

class MyResource : public Resource {
private:
  v_int64 m_id;
public:

  MyResource(v_int64 number)
    : m_id(number)
  {}

  v_int64 myId() override {
    return m_id;
  }

};

class Provider : public oatpp::provider::Provider<Resource> {
private:

  class ResourceInvalidator : public oatpp::provider::Invalidator<Resource> {
  public:

    void invalidate(const std::shared_ptr<Resource>& resource) override {
      (void) resource;
    }

  };

private:
  std::shared_ptr<ResourceInvalidator> m_invalidator = std::make_shared<ResourceInvalidator>();
  std::atomic<v_int64> m_id;
public:

  oatpp::provider::ResourceHandle<Resource> get() override {
    return oatpp::provider::ResourceHandle<Resource>(
      std::make_shared<MyResource>(++m_id),
      m_invalidator
    );
  }

  async::CoroutineStarterForResult<const oatpp::provider::ResourceHandle<Resource> &> getAsync() override {

    class GetCoroutine : public oatpp::async::CoroutineWithResult<GetCoroutine, const oatpp::provider::ResourceHandle<Resource>&> {
    private:
      Provider* m_provider;
    public:

      GetCoroutine(Provider* provider)
        : m_provider(provider)
      {}

      Action act() override {
        return _return(oatpp::provider::ResourceHandle<Resource>(
          std::make_shared<MyResource>(++ m_provider->m_id),
          m_provider->m_invalidator
        ));
      }

    };

    return GetCoroutine::startForResult(this);
  }

  void stop() override {
    OATPP_LOGD("Provider", "stop()");
  }

  v_int64 getIdCounter() {
    return m_id;
  }

};


struct AcquisitionProxy : public oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy> {

  AcquisitionProxy(const oatpp::provider::ResourceHandle<Resource>& resource,
                   const std::shared_ptr<PoolInstance>& pool)
    : oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy>(resource, pool)
  {}

  v_int64 myId() override {
    return _handle.object->myId();
  }

};

typedef oatpp::provider::Pool<oatpp::provider::Provider<Resource>, Resource, AcquisitionProxy> Pool;


class ClientCoroutine : public oatpp::async::Coroutine<ClientCoroutine> {
private:
  std::shared_ptr<Pool> m_pool;
  oatpp::provider::ResourceHandle<Resource> m_resource;
  bool m_invalidate;
public:

  ClientCoroutine(const std::shared_ptr<Pool>& pool, bool invalidate)
    : m_pool(pool)
    , m_invalidate(invalidate)
  {}

  Action act() override {
    return m_pool->getAsync().callbackTo(&ClientCoroutine::onGet);
  }

  Action onGet(const oatpp::provider::ResourceHandle<Resource>& resource) {
    m_resource = resource;
    return waitFor(std::chrono::milliseconds(100)).next(yieldTo(&ClientCoroutine::onUse));
  }

  Action onUse() {
    if(m_invalidate) {
      m_resource.invalidator->invalidate(m_resource.object);
    }
    return finish();
  }

};

void clientMethod(std::shared_ptr<Pool> pool, bool invalidate) {
  auto resource = pool->get();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  if(invalidate) {
    resource.invalidator->invalidate(resource.object);
  }
}

}

void PoolTest::onRun() {

  oatpp::async::Executor executor(10, 1, 1);

  auto provider = std::make_shared<Provider>();
  auto pool = Pool::createShared(provider, 10, std::chrono::seconds(2));


  std::list<std::thread> threads;

  OATPP_LOGD(TAG, "Run 1");
  for(v_int32 i = 0; i < 100; i ++ ) {
    threads.push_back(std::thread(clientMethod, pool, false));
    executor.execute<ClientCoroutine>(pool, false);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds (200));

  OATPP_LOGD(TAG, "1) pool->getCounter() == %d", pool->getCounter());
  OATPP_ASSERT(pool->getCounter() == 10);
  OATPP_LOGD(TAG, "Waiting...");
  std::this_thread::sleep_for(std::chrono::seconds(10));
  OATPP_LOGD(TAG, "Pool counter=%d", pool->getCounter());
  OATPP_ASSERT(pool->getCounter() == 0);

  OATPP_LOGD(TAG, "Run 2");
  for(v_int32 i = 0; i < 100; i ++ ) {
    threads.push_back(std::thread(clientMethod, pool, false));
    executor.execute<ClientCoroutine>(pool, false);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds (200));

  OATPP_LOGD(TAG, "2) pool->getCounter() == %d", pool->getCounter());
  OATPP_ASSERT(pool->getCounter() == 10);
  OATPP_LOGD(TAG, "Waiting...");
  std::this_thread::sleep_for(std::chrono::seconds(10));
  OATPP_LOGD(TAG, "Pool counter=%d", pool->getCounter());
  OATPP_ASSERT(pool->getCounter() == 0);

  for(std::thread& thread : threads) {
    thread.join();
  }

  executor.waitTasksFinished();

  OATPP_LOGD(TAG, "counter=%d", provider->getIdCounter());
  OATPP_ASSERT(provider->getIdCounter() == 20);

  pool->stop();

  executor.stop();
  executor.join();

  /* wait pool cleanup task exit */
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

}

}}}}
