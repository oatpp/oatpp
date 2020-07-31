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

#ifndef oatpp_provider_Pool_hpp
#define oatpp_provider_Pool_hpp

#include "Provider.hpp"
#include "oatpp/core/async/CoroutineWaitList.hpp"

#include <thread>
#include <condition_variable>

namespace oatpp { namespace provider {

template<class TResource, class AcquisitionProxyImpl>
class PoolTemplate; // FWD

/**
 * Pool acquisition proxy template.
 * @tparam TResource - abstract resource interface type, Ex.: `IOStream`.
 * @tparam AcquisitionProxyImpl - implementation of proxy.
 */
template<class TResource, class AcquisitionProxyImpl>
class AcquisitionProxy : public TResource {
  friend PoolTemplate<TResource, AcquisitionProxyImpl>;
public:
  /**
   * Convenience typedef for Pool.
   */
  typedef PoolTemplate<TResource, AcquisitionProxyImpl> PoolInstance;
private:

  void __pool__invalidate() {
    m_valid = false;
  }

  std::shared_ptr<TResource> __pool__getUnderlyingResource() {
    return _obj;
  }

protected:
  std::shared_ptr<TResource> _obj;
private:
  std::shared_ptr<PoolInstance> m_pool;
  bool m_valid;
public:

  /**
   * Constructor.
   * @param resource - base resource.
   * @param pool - &l:AcquisitionProxy::PoolInstance;.
   */
  AcquisitionProxy(const std::shared_ptr<TResource>& resource, const std::shared_ptr<PoolInstance>& pool)
    : _obj(resource)
    , m_pool(pool)
    , m_valid(true)
  {}

  /**
   * Virtual destructor.
   */
  virtual ~AcquisitionProxy() {
    m_pool->release(std::move(_obj), m_valid);
  }

};

template<class TResource, class AcquisitionProxyImpl>
class PoolTemplate : public oatpp::base::Countable, public async::CoroutineWaitList::Listener {
  friend AcquisitionProxy<TResource, AcquisitionProxyImpl>;
private:

  struct PoolRecord {
    std::shared_ptr<TResource> resource;
    v_int64 timestamp;
  };

private:

  void onNewItem(async::CoroutineWaitList& list) override {

    std::unique_lock<std::mutex> guard(m_lock);

    if(!m_running) {
      guard.unlock();
      list.notifyAll();
      return;
    }

    if(m_counter < m_maxResources || m_bench.size() > 0) {
      guard.unlock();
      list.notifyFirst();
    }

  }

  void release(std::shared_ptr<TResource>&& resource, bool canReuse) {

    {

      std::lock_guard<std::mutex> guard(m_lock);

      if(!m_running) {
        -- m_counter;
        return;
      }

      if(canReuse) {
        m_bench.push_back({resource, oatpp::base::Environment::getMicroTickCount()});
      } else {
        -- m_counter;
      }

    }

    m_condition.notify_one();
    m_waitList.notifyFirst();

  }

private:

  static void cleanupTask(std::shared_ptr<PoolTemplate> pool) {

    while(pool->m_running) { // timer-based cleanup loop

      {
        std::lock_guard<std::mutex> guard(pool->m_lock);
        auto ticks = oatpp::base::Environment::getMicroTickCount();

        auto i = pool->m_bench.begin();
        while (i != pool->m_bench.end()) {

          auto elapsed = ticks - i->timestamp;
          if(elapsed > pool->m_maxResourceTTL) {
            pool->m_provider->invalidate(i->resource);
            i = pool->m_bench.erase(i);
            pool->m_counter --;
          } else {
            i ++;
          }

        }

      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

    { /* invalidate all pooled resources */

      std::lock_guard<std::mutex> guard(pool->m_lock);
      auto i = pool->m_bench.begin();
      while (i != pool->m_bench.end()) {
        pool->m_provider->invalidate(i->resource);
        i = pool->m_bench.erase(i);
        pool->m_counter --;
      }

      pool->m_finished = true;

    }

    pool->m_condition.notify_all();

  }

private:
  std::shared_ptr<Provider<TResource>> m_provider;
  v_int64 m_counter;
  v_int64 m_maxResources;
  v_int64 m_maxResourceTTL;
  std::atomic<bool> m_running;
  bool m_finished;
private:
  std::list<PoolRecord> m_bench;
  async::CoroutineWaitList m_waitList;
  std::condition_variable m_condition;
  std::mutex m_lock;
protected:

  PoolTemplate(const std::shared_ptr<Provider<TResource>>& provider, v_int64 maxResources, v_int64 maxResourceTTL)
    : m_provider(provider)
    , m_counter(0)
    , m_maxResources(maxResources)
    , m_maxResourceTTL(maxResourceTTL)
    , m_running(true)
    , m_finished(false)
  {}

  void startCleanupTask(const std::shared_ptr<PoolTemplate>& _this) {
    std::thread poolCleanupTask(cleanupTask, _this);
    poolCleanupTask.detach();
  }

public:

  static std::shared_ptr<PoolTemplate> createShared(const std::shared_ptr<Provider<TResource>>& provider,
                                                    v_int64 maxResources,
                                                    const std::chrono::duration<v_int64, std::micro>& maxResourceTTL)
  {
    /* "new" is called directly to keep constructor private */
    auto ptr = std::shared_ptr<PoolTemplate>(new PoolTemplate(provider, maxResources, maxResourceTTL.count()));
    ptr->startCleanupTask();
    return ptr;
  }

  virtual ~PoolTemplate() {
    stop();
  }

  std::shared_ptr<TResource> get(const std::shared_ptr<PoolTemplate>& _this) {

    {

      std::unique_lock<std::mutex> guard(m_lock);

      while (m_running && m_bench.size() == 0 && m_counter >= m_maxResources ) {
        m_condition.wait(guard);
      }

      if(!m_running) {
        return nullptr;
      }

      if (m_bench.size() > 0) {
        auto record = m_bench.front();
        m_bench.pop_front();
        return std::make_shared<AcquisitionProxyImpl>(record.resource, _this);
      } else {
        ++ m_counter;
      }

    }

    try {
      return std::make_shared<AcquisitionProxyImpl>(m_provider->get(), _this);
    } catch (...) {
      std::lock_guard<std::mutex> guard(m_lock);
      -- m_counter;
      return nullptr;
    }

  }

  async::CoroutineStarterForResult<const std::shared_ptr<TResource>&> getAsync(const std::shared_ptr<PoolTemplate>& _this) {

    class GetCoroutine : public oatpp::async::CoroutineWithResult<GetCoroutine, const std::shared_ptr<TResource>&> {
    private:
      std::shared_ptr<PoolTemplate> m_pool;
      std::shared_ptr<Provider<TResource>> m_provider;
    public:

      GetCoroutine(const std::shared_ptr<PoolTemplate>& pool, const std::shared_ptr<Provider<TResource>>& provider)
        : m_pool(pool)
        , m_provider(provider)
      {}

      async::Action act() override {

        {
          /* Careful!!! Using non-async lock */
          std::unique_lock<std::mutex> guard(m_pool->m_lock);

          if (m_pool->m_running && m_pool->m_bench.size() == 0 && m_pool->m_counter >= m_pool->m_maxResources) {
            guard.unlock();
            return async::Action::createWaitListAction(&m_pool->m_waitList);
          }

          if(!m_pool->m_running) {
            guard.unlock();
            return this->_return(nullptr);
          }

          if (m_pool->m_bench.size() > 0) {
            auto record = m_pool->m_bench.front();
            m_pool->m_bench.pop_front();
            guard.unlock();
            return this->_return(std::make_shared<AcquisitionProxyImpl>(record.resource, m_pool));
          } else {
            ++ m_pool->m_counter;
          }

        }

        return m_provider->getAsync().callbackTo(&GetCoroutine::onGet);

      }

      async::Action onGet(const std::shared_ptr<TResource>& resource) {
        return this->_return(std::make_shared<AcquisitionProxyImpl>(resource, m_pool));
      }

      async::Action handleError(oatpp::async::Error* error) override {
        {
          /* Careful!!! Using non-async lock */
          std::lock_guard<std::mutex> guard(m_pool->m_lock);
          -- m_pool->m_counter;
        }
        return error;
      }

    };

    return GetCoroutine::startForResult(_this, m_provider);

  }

  void invalidate(const std::shared_ptr<TResource>& resource) {
    auto proxy = std::static_pointer_cast<AcquisitionProxyImpl>(resource);
    proxy->__pool__invalidate();
    m_provider->invalidate(proxy->__pool__getUnderlyingResource());
  }

  void stop() {

    {
      std::lock_guard<std::mutex> guard(m_lock);

      if (!m_running) {
        return;
      }

      m_running = false;
      m_counter -= m_bench.size();
      m_bench.clear();
    }

    m_condition.notify_all();
    m_waitList.notifyAll();

    {
      std::unique_lock<std::mutex> guard(m_lock);
      while (!m_finished) {
        m_condition.wait(guard);
      }

    }

    m_provider->stop();

  }

  v_int64 getCounter() {
    std::lock_guard<std::mutex> guard(m_lock);
    return m_counter;
  }

};

/**
 * Pool template class.
 * @tparam TProvider - base class for pool to inherit, ex.: ServerConnectionProvider.
 * @tparam TResource - abstract resource interface type, Ex.: `IOStream`. Must be the same as a return-type of Provider.
 * @tparam AcquisitionProxyImpl - implementation of &l:AcquisitionProxy;.
 */
template<class TProvider, class TResource, class AcquisitionProxyImpl>
class Pool :
  public TProvider,
  public std::enable_shared_from_this<Pool<TProvider, TResource, AcquisitionProxyImpl>>,
  public PoolTemplate<TResource, AcquisitionProxyImpl> {
private:
  typedef PoolTemplate<TResource, AcquisitionProxyImpl> TPool;
protected:

  /**
   * Protected Constructor.
   * @param provider
   * @param maxResources
   * @param maxResourceTTL
   */
  Pool(const std::shared_ptr<TProvider>& provider, v_int64 maxResources, v_int64 maxResourceTTL)
    : PoolTemplate<TResource, AcquisitionProxyImpl>(provider, maxResources, maxResourceTTL)
  {
    TProvider::m_properties = provider->getProperties();
  }

public:

  /**
   * Create shared Pool.
   * @param provider - resource provider.
   * @param maxResources - max resource count in the pool.
   * @param maxResourceTTL - max time-to-live for unused resource in the pool.
   * @return - `std::shared_ptr` of `Pool`.
   */
  static std::shared_ptr<Pool> createShared(const std::shared_ptr<TProvider>& provider,
                                            v_int64 maxResources,
                                            const std::chrono::duration<v_int64, std::micro>& maxResourceTTL)
  {
    /* "new" is called directly to keep constructor private */
    auto ptr = std::shared_ptr<Pool>(new Pool(provider, maxResources, maxResourceTTL.count()));
    ptr->startCleanupTask(ptr);
    return ptr;
  }

  /**
   * Get resource.
   * @return
   */
  std::shared_ptr<TResource> get() override {
    return TPool::get(this->shared_from_this());
  }

  /**
   * Get resource asynchronously.
   * @return
   */
  async::CoroutineStarterForResult<const std::shared_ptr<TResource>&> getAsync() override {
    return TPool::getAsync(this->shared_from_this());
  }

  /**
   * Invalidate resource.
   * @param resource
   */
  void invalidate(const std::shared_ptr<TResource>& resource) override {
    TPool::invalidate(resource);
  }

  /**
   * Stop pool. <br>
   * *Note: call to stop() may block.*
   */
  void stop() override {
    TPool::stop();
  }

  /**
   * Get pool resource count. Both acquired and available.
   * @return
   */
  v_int64 getCounter() {
    return TPool::getCounter();
  }

};

}}

#endif // oatpp_provider_Pool_hpp
