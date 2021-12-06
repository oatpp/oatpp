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

#ifndef oatpp_async_Future_hpp
#define oatpp_async_Future_hpp

#include "Coroutine.hpp"
#include "CoroutineWaitList.hpp"

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async {

template<typename T>
class Promise; // FWD

template<typename T>
class Future {
  friend Promise<T>;
public:
  typedef T ValueType;
private:

  class Handle : private CoroutineWaitList::Listener {
  private:

    void onNewItem(CoroutineWaitList& list) override {
      std::unique_lock<std::mutex> lock(m_mutex);
      if(m_ready) {
        lock.unlock();
        list.notifyAll();
      }
    }

  private:
    bool m_ready;
    ValueType m_value;
    CoroutineWaitList m_list;
    std::exception_ptr m_ePtr;
    std::mutex m_mutex;
    std::condition_variable m_condition;
  public:

    Handle()
      : m_ready(false)
    {
      m_list.setListener(this);
    }

    void setValue(const ValueType& value) {
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_ready) {
          m_value = value;
          m_ready = true;
        } else {
          throw std::runtime_error("[oatpp::async::Future::Handle::setValue()]: Error. Promise already fulfilled.");
        }
      }
      m_list.notifyAll();
      m_condition.notify_all();
    }

    void setException(std::exception_ptr ptr) {
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_ready) {
          m_ePtr = ptr;
          m_ready = true;
        } else {
          throw std::runtime_error("[oatpp::async::Future::Handle::setException()]: Error. Promise already fulfilled.");
        }
      }
      m_list.notifyAll();
      m_condition.notify_all();
    }

    void wait() {
      std::unique_lock<std::mutex> lock(m_mutex);
      while(!m_ready) {
        m_condition.wait(lock);
      }
    }

    static CoroutineStarter waitAsync(const std::shared_ptr<Handle>& handle) {

      class WaitCoroutine : public Coroutine<WaitCoroutine> {
      private:
        std::shared_ptr<Handle> m_handle;
      public:

        WaitCoroutine(const std::shared_ptr<Handle>& handle)
          : m_handle(handle)
        {}

        Action act() override {
          std::lock_guard<std::mutex> lock(m_handle->m_mutex);
          if(!m_handle->m_ready) {
            return Action::createWaitListAction(&m_handle->m_list);
          }
          return this->finish();
        }

      };

      return WaitCoroutine::start(handle);

    }

    ValueType get() {
      wait();
      if(m_ePtr) {
        std::rethrow_exception(m_ePtr);
      }
      return m_value;
    }

    static CoroutineStarterForResult<const ValueType&> getAsync(const std::shared_ptr<Handle>& handle) {

      class GetCoroutine : public CoroutineWithResult<GetCoroutine, const ValueType&> {
      private:
        std::shared_ptr<Handle> m_handle;
      public:

        GetCoroutine(const std::shared_ptr<Handle>& handle)
          : m_handle(handle)
        {}

        Action act() override {

          std::unique_lock<std::mutex> lock(m_handle->m_mutex);

          if(!m_handle->m_ready) {
            return Action::createWaitListAction(&m_handle->m_list);
          }

          if(m_handle->m_ePtr) {
            std::rethrow_exception(m_handle->m_ePtr);
          }

          lock.unlock();

          return this->_return(m_handle->m_value);
        }

      };

      return GetCoroutine::startForResult(handle);

    }

  };

private:
  std::shared_ptr<Handle> m_handle;
private:

  Future(const std::shared_ptr<Handle>& handle)
    : m_handle(handle)
  {}

public:

  Future() {}

  void wait() const {
    m_handle->wait();
  }

  CoroutineStarter waitAsync() const {
    return Handle::waitAsync(m_handle);
  }

  ValueType get() const {
    return m_handle->get();
  }

  CoroutineStarterForResult<const ValueType&> getAsync() const {
    return m_handle->getAsync(m_handle);
  }

};

template<typename T>
class Promise {
public:
  typedef T ValueType;
private:
  std::shared_ptr<typename Future<ValueType>::Handle> m_handle;
public:

  Promise()
    : m_handle(std::make_shared<typename Future<ValueType>::Handle>())
  {}

  Future<ValueType> getFuture() const {
    return Future<ValueType>(m_handle);
  }

  void setValue(const ValueType& value) {
    m_handle->setValue(value);
  }

  void setException(std::exception_ptr ptr) {
    m_handle->setException(ptr);
  }

};

}}

#endif //oatpp_async_Future_hpp
