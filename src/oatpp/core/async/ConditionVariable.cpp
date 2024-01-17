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

#include "ConditionVariable.hpp"

namespace oatpp { namespace async {

ConditionVariable::ConditionVariable()
  : m_notified(false)
{
  m_list.setListener(this);
}

void ConditionVariable::onNewItem(CoroutineWaitList& list) {
  if(m_notified) {
    m_list.notifyFirst();
  }
}

CoroutineStarter ConditionVariable::wait(LockGuard& lockGuard, std::function<bool()> condition) {

  class WaitCoroutine : public Coroutine<WaitCoroutine> {
  private:
    ConditionVariable* m_cv;
    oatpp::async::LockGuard* m_lockGuard;
    std::function<bool()> m_condition;
  public:

    WaitCoroutine(ConditionVariable* cv, LockGuard& lockGuard, std::function<bool()> condition)
      : m_cv(cv)
      , m_lockGuard(&lockGuard)
      , m_condition(condition)
    {}

    Action act() override {
      return m_lockGuard->lockAsyncInline(yieldTo(&WaitCoroutine::onLocked));
    }

    Action onLocked() {
      if(m_lockGuard->owns_lock()) {
        if (m_condition()) {
          return finish();
        }
        m_cv->m_notified = false;
        m_lockGuard->unlock();
      } else {
        return yieldTo(&WaitCoroutine::act);
      }
      return Action::createWaitListAction(&m_cv->m_list);
    }

  };

  return WaitCoroutine::start(this, lockGuard, condition);

}

CoroutineStarter ConditionVariable::waitUntil(LockGuard& lockGuard, std::function<bool()> condition, const std::chrono::system_clock::time_point& timeoutTime) {

  class WaitCoroutine : public Coroutine<WaitCoroutine> {
  private:
    ConditionVariable* m_cv;
    oatpp::async::LockGuard* m_lockGuard;
    std::function<bool()> m_condition;
    std::chrono::system_clock::time_point m_timeoutTime;
  public:

    WaitCoroutine(ConditionVariable* cv, LockGuard& lockGuard, std::function<bool()> condition, const std::chrono::system_clock::time_point& timeoutTime)
      : m_cv(cv)
      , m_lockGuard(&lockGuard)
      , m_condition(condition)
      , m_timeoutTime(timeoutTime)
    {}

    Action act() override {
      return m_lockGuard->lockAsyncInline(yieldTo(&WaitCoroutine::onLocked));
    }

    Action onLocked() {

      if(m_lockGuard->owns_lock()) {

        if (m_condition()) {
          return finish();
        }
        m_cv->m_notified = false;
        m_lockGuard->unlock();

      } else {
        if(std::chrono::system_clock::now() > m_timeoutTime) {
          return finish();
        }
        return yieldTo(&WaitCoroutine::act);
      }

      if(std::chrono::system_clock::now() > m_timeoutTime) {
        return finish();
      }


      return Action::createWaitListAction(&m_cv->m_list, m_timeoutTime);
    }

  };

  return WaitCoroutine::start(this, lockGuard, condition, timeoutTime);

}

CoroutineStarter ConditionVariable::waitFor(LockGuard& lockGuard, std::function<bool()> condition, const std::chrono::duration<v_int64, std::micro>& timeout) {
  return waitUntil(lockGuard, condition, std::chrono::system_clock::now() + timeout);
}

void ConditionVariable::notifyFirst() {
  m_notified = true;
  m_list.notifyFirst();
}

void ConditionVariable::notifyAll() {
  m_notified = true;
  m_list.notifyAll();
}

}}
