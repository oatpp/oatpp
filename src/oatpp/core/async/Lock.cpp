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

#include "Lock.hpp"

namespace oatpp { namespace async {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lock

Lock::Lock()
  : m_counter(0)
{
  m_list.setListener(this);
}

void Lock::onNewItem(CoroutineWaitList& list) {
  auto counter = m_counter.load();
  if(counter == 0) {
    list.notifyFirst();
  } else if(counter < 0) {
    throw std::runtime_error("[oatpp::async::Lock::onNewItem()]: Error. Invalid state.");
  }
}

Action Lock::waitAsync() {
  auto counter = m_counter.load();
  if(counter > 0) {
    return Action::createWaitListAction(&m_list);
  } else if(counter == 0) {
    return Action::createActionByType(Action::TYPE_REPEAT);
  }
  throw std::runtime_error("[oatpp::async::Lock::waitAsync()]: Error. Invalid state.");
}

void Lock::lock() {
  m_mutex.lock();
  ++ m_counter;
}

void Lock::unlock() {
  m_mutex.unlock();
  -- m_counter;
  if(m_counter < 0) {
    throw std::runtime_error("[oatpp::async::Lock::unlock()]: Error. Invalid state.");
  }
  m_list.notifyFirst();
}

bool Lock::try_lock() {
  bool result = m_mutex.try_lock();
  if(result) {
    ++ m_counter;
  }
  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LockGuard

LockGuard::LockGuard()
  : m_ownsLock(false)
  , m_lock(nullptr)
{}

LockGuard::LockGuard(Lock* lock)
  : m_ownsLock(false)
  , m_lock(lock)
{}

LockGuard::~LockGuard() {
  if(m_ownsLock) {
    m_lock->unlock();
  }
}

void LockGuard::setLockObject(Lock* lock) {
  if(m_lock == nullptr) {
    m_lock = lock;
  } else if(m_lock != lock) {
    throw std::runtime_error("[oatpp::async::LockGuard::setLockObject()]: Error. Invalid state. LockGuard is NOT reusable.");
  }
}

CoroutineStarter LockGuard::lockAsync() {

  class LockCoroutine : public Coroutine<LockCoroutine> {
  private:
    LockGuard* m_guard;
  public:

    LockCoroutine(LockGuard* guard)
      : m_guard(guard)
    {}

    Action act() override {
      return m_guard->lockAsyncInline(finish());
    }

  };

  return LockCoroutine::start(this);

}

CoroutineStarter LockGuard::lockAsync(Lock* lock) {
  setLockObject(lock);
  return lockAsync();
}


Action LockGuard::lockAsyncInline(oatpp::async::Action&& nextAction) {

  if(!m_ownsLock) {

    m_ownsLock = m_lock->try_lock();

    if(m_ownsLock) {
      return std::forward<Action>(nextAction);
    } else {
      return m_lock->waitAsync();
    }

  } else {
    throw std::runtime_error("[oatpp::async::LockGuard::lockAsyncInline()]: Error. Invalid state. Double lock attempt.");
  }

}


void LockGuard::unlock() {

  if(m_lock) {

    if(m_ownsLock) {

      m_lock->unlock();
      m_ownsLock = false;

    } else {
      throw std::runtime_error("[oatpp::async::LockGuard::unlock()]: Error. Invalid state. LockGuard is NOT owning the lock.");
    }

  } else {
    throw std::runtime_error("[oatpp::async::LockGuard::unlock()]: Error. Invalid state. Lock object is nullptr.");
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Misc

CoroutineStarter synchronize(oatpp::async::Lock *lock, CoroutineStarter&& starter) {

  class Synchronized : public oatpp::async::Coroutine<Synchronized> {
  private:
    oatpp::async::LockGuard m_lockGuard;
    CoroutineStarter m_starter;
  public:

    Synchronized(oatpp::async::Lock *lock, CoroutineStarter&& starter)
      : m_lockGuard(lock)
      , m_starter(std::forward<CoroutineStarter>(starter))
    {}

    Action act() override {
      return m_lockGuard.lockAsync().next(std::move(m_starter)).next(finish());
    }

  };

  return new Synchronized(lock, std::forward<CoroutineStarter>(starter));

}

}}
