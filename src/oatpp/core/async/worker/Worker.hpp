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

#ifndef oatpp_async_worker_Worker_hpp
#define oatpp_async_worker_Worker_hpp

#include "oatpp/core/async/Coroutine.hpp"
#include <thread>

namespace oatpp { namespace async { namespace worker {

/**
 * Worker base class.
 * Workers are used by &id:oatpp::async::Executor; to reschedule worker-specific tasks from &id:oatpp::async::Processor;.
 */
class Worker {
public:

  /**
   * Worker type
   */
  enum Type : v_int32 {

    /**
     * Worker type - general processor.
     */
    PROCESSOR = 0,

    /**
     * Worker type - timer processor.
     */
    TIMER = 1,

    /**
     * Worker type - I/O processor.
     */
    IO = 2,

    /**
     * Number of types in this enum.
     */
    TYPES_COUNT = 3

  };

private:
  Type m_type;
protected:
  static void setCoroutineScheduledAction(CoroutineHandle* coroutine, Action&& action);
  static Action& getCoroutineScheduledAction(CoroutineHandle* coroutine);
  static Processor* getCoroutineProcessor(CoroutineHandle* coroutine);
  static void dismissAction(Action& action);
  static CoroutineHandle* nextCoroutine(CoroutineHandle* coroutine);
public:

  /**
   * Constructor.
   * @param type - worker type - one of &l:Worker::Type; values.
   */
  Worker(Type type);

  /**
   * Default virtual destructor.
   */
  virtual ~Worker() = default;

  /**
   * Push list of tasks to worker.
   * @param tasks - &id:oatpp::async::utils::FastQueue; of &id:oatpp::async::CoroutineHandle;.
   */
  virtual void pushTasks(utils::FastQueue<CoroutineHandle>& tasks) = 0;

  /**
   * Push one task to worker.
   * @param task - &id:oatpp::async::CoroutineHandle;.
   */
  virtual void pushOneTask(CoroutineHandle* task) = 0;

  /**
   * Break run loop.
   */
  virtual void stop() = 0;

  /**
   * Join all worker-threads.
   */
  virtual void join() = 0;

  /**
   * Detach all worker-threads.
   */
  virtual void detach() = 0;

  /**
   * Get worker type.
   * @return - one of &l:Worker::Type; values.
   */
  Type getType();

};

}}}

#endif //oatpp_async_worker_Worker_hpp
