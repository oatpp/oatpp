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

#ifndef oatpp_async_Mutex_hpp
#define oatpp_async_Mutex_hpp

#include "./CoroutineWaitList.hpp"

namespace oatpp { namespace async {

/**
 * Lock (mutex) for coroutines/threads synchronization. <br>
 * - When called from a thread - must be used with `std::lock_guard`.
 * - When called from coroutine - must be used with &l:LockGuard;.
 */
class Lock : private CoroutineWaitList::Listener {
private:
  std::atomic<v_int32> m_counter;
  std::mutex m_mutex;
  CoroutineWaitList m_list;
private:
  void onNewItem(CoroutineWaitList& list) override;
public:

  /**
   * Constructor.
   */
  Lock();

  /**
   * Wait until lock is unlocked, and repeat.
   * @return - &id:oatpp::async::Action;.
   */
  Action waitAsync();

  /**
   * Lock on current thread. !Should NOT be called from within the Coroutine!
   */
  void lock();

  /**
   * Unlock
   */
  void unlock();

  /**
   * Try to lock.
   * @return - `true` if the lock was acquired, `false` otherwise.
   */
  bool try_lock();

};

/**
 * Asynchronous lock guard. <br>
 * Should be used as a lock guard in coroutines.
 */
class LockGuard {
public:
  /**
   * Convenince typedef for &id:oatpp::async::CoroutineStarter;.
   */
  typedef oatpp::async::CoroutineStarter CoroutineStarter;
private:
  bool m_ownsLock;
  Lock* m_lock;
public:
  LockGuard(const LockGuard&) = delete;
  LockGuard& operator = (const LockGuard&) = delete;
public:

  /**
   * Default constructor.
   */
  LockGuard();

  /**
   * Constructor with lock.
   */
  LockGuard(Lock* lock);

  /**
   * Non-virtual destructor. <br>
   * Will unlock the Lock if owns lock.
   */
  ~LockGuard();

  /**
   * Set lock object.
   * @param lock - lock object.
   */
  void setLockObject(Lock* lock);

  /**
   * Lock the lock.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  CoroutineStarter lockAsync();

  /**
   * Lock and guard the lock. <br>
   * Same as `setLockObject(lock) + lockAsync();`.
   * @param lock - lock to lock and guard.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  CoroutineStarter lockAsync(Lock* lock);

  /**
   * Lock the lock. (Async-inline usage. Should be called from a separate method of coroutine).
   * @param nextAction - action to take after lock is locked.
   * @return - &id:oatpp::async::Action;.
   */
  Action lockAsyncInline(oatpp::async::Action&& nextAction);

  /**
   * Check if owns lock.
   * @return
   */
  bool owns_lock() const;

  /**
   * Unlock guarded lock.
   */
  void unlock();

};

/**
 * Synchronize coroutine execution by lock.
 * @param lock - &l:Lock; for synchronization.
 * @param starter - Coroutine to execute in synchronized manner. &id:oatpp::async::CoroutineStarter;.
 * @return - starter of synchronization coroutine (wrapper coroutine). &id:oatpp::async::CoroutineStarter;.
 */
CoroutineStarter synchronize(oatpp::async::Lock *lock, CoroutineStarter&& starter);

}}

#endif // oatpp_async_Mutex_hpp
