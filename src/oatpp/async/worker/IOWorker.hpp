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

#ifndef oatpp_async_worker_IOWorker_hpp
#define oatpp_async_worker_IOWorker_hpp

#include "./Worker.hpp"
#include "oatpp/concurrency/SpinLock.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async { namespace worker {

/**
* Naive implementation of IOWorker.
* Polls all I/O handles in a loop. Reschedules long-waiting handles to Timer.
*/
class IOWorker : public Worker {
private:
  bool m_running;
  utils::FastQueue<CoroutineHandle> m_backlog;
  utils::FastQueue<CoroutineHandle> m_queue;
  oatpp::concurrency::SpinLock m_backlogLock;
  std::condition_variable_any m_backlogCondition;
private:
  std::thread m_thread;
private:
  void consumeBacklog(bool blockToConsume);
public:

  /**
  * Constructor.
  */
  IOWorker();

  /**
  * Push list of tasks to worker.
  * @param tasks - &id:oatpp::async::utils::FastQueue; of &id:oatpp::async::CoroutineHandle;.
  */
  void pushTasks(utils::FastQueue<CoroutineHandle>& tasks) override;

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

};

}}}

#endif //oatpp_async_worker_IOWorker_hpp
