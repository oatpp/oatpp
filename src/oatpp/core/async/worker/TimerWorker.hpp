/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#ifndef oatpp_async_worker_TimerWorker_hpp
#define oatpp_async_worker_TimerWorker_hpp

#include "./Worker.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async { namespace worker {

/**
 * Timer worker.
 * Used to wait for timer-scheduled coroutines.
 */
class TimerWorker : public Worker {
private:
  std::atomic_bool m_running;
  oatpp::collection::FastQueue<CoroutineHandle> m_backlog;
  oatpp::collection::FastQueue<CoroutineHandle> m_queue;
  oatpp::concurrency::SpinLock m_backlogLock;
  std::condition_variable_any m_backlogCondition;
private:
  std::chrono::duration<v_int64, std::micro> m_granularity;
 private:
  bool m_detached;
  std::thread m_thread;
  oatpp::concurrency::SpinLock m_threadLock;
private:
  void consumeBacklog();
public:

  /**
   * Constructor.
   * @param granularity - minimum possible time to wait.
   */
  TimerWorker(const std::chrono::duration<v_int64, std::micro>& granularity = std::chrono::milliseconds(100));

  /**
   * Push list of tasks to worker.
   * @param tasks - &id:oatpp::collection::FastQueue; of &id:oatpp::async::CoroutineHandle;.
   */
  void pushTasks(oatpp::collection::FastQueue<CoroutineHandle>& tasks) override;

  /**
   * Push one task to worker.
   * @param task - &id:CoroutineHandle;.
   */
  void pushOneTask(CoroutineHandle* task) override;

  /**
   * Run worker.
   */
  void run();

  /**
   * Break run loop.
   */
  void stop() override;

  /**
   * Join all worker-threads.
   */
  void join() override;

  /**
   * Detach all worker-threads.
   */
  void detach() override;

  /**
   * Abort a Coroutine by its id
   * @param coroutineId - Coroutine to abort
   * @return - `true` if a coroutine to abort was found, `false` if not.
   */
  bool abortCoroutine(v_uint64 coroutineId) override;

};

}}}

#endif //oatpp_async_worker_TimerWorker_hpp
