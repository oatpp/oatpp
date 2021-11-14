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

#ifndef oatpp_async_Executor_hpp
#define oatpp_async_Executor_hpp

#include "./Processor.hpp"
#include "oatpp/core/async/worker/Worker.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

#include <tuple>
#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async {

/**
 * Asynchronous Executor.<br>
 * Executes coroutines in multiple &id:oatpp::async::Processor;
 * allocating one thread per processor.
 */
class Executor {
private:
  
  class SubmissionProcessor : public worker::Worker {
  private:
    oatpp::async::Processor m_processor;
  private:
    std::atomic<bool> m_isRunning;
  private:
    std::thread m_thread;
  public:
    SubmissionProcessor();
  public:

    template<typename CoroutineType, typename ... Args>
    void execute(Args... params) {
      m_processor.execute<CoroutineType, Args...>(params...);
    }

    oatpp::async::Processor& getProcessor();

    void pushTasks(utils::FastQueue<CoroutineHandle>& tasks) override;

    void pushOneTask(CoroutineHandle* task) override;

    void run();

    void stop() override;

    void join() override;

    void detach() override;
    
  };

public:
  /**
   * Special value to indicate that Executor should choose it's own the value of specified parameter.
   */
  static constexpr const v_int32 VALUE_SUGGESTED = -1000;
public:

  /**
   * IO Worker type naive.
   */
  static constexpr const v_int32 IO_WORKER_TYPE_NAIVE = 0;

  /**
   * IO Worker type event.
   */
  static constexpr const v_int32 IO_WORKER_TYPE_EVENT = 1;
private:
  std::atomic<v_uint32> m_balancer;
private:
  std::vector<std::shared_ptr<SubmissionProcessor>> m_processorWorkers;
  std::vector<std::shared_ptr<worker::Worker>> m_allWorkers;
private:
  static v_int32 chooseProcessorWorkersCount(v_int32 processorWorkersCount);
  static v_int32 chooseIOWorkersCount(v_int32 processorWorkersCount, v_int32 ioWorkersCount);
  static v_int32 chooseTimerWorkersCount(v_int32 timerWorkersCount);
  static v_int32 chooseIOWorkerType(v_int32 ioWorkerType);
  void linkWorkers(const std::vector<std::shared_ptr<worker::Worker>>& workers);
public:

  /**
   * Constructor.
   * @param processorWorkersCount - number of data processing workers.
   * @param ioWorkersCount - number of I/O processing workers.
   * @param timerWorkersCount - number of timer processing workers.
   * @param IOWorkerType
   */
  Executor(v_int32 processorWorkersCount = VALUE_SUGGESTED,
           v_int32 ioWorkersCount = VALUE_SUGGESTED,
           v_int32 timerWorkersCount = VALUE_SUGGESTED,
           v_int32 ioWorkerType = VALUE_SUGGESTED);

  /**
   * Non-virtual Destructor.
   */
  ~Executor() = default;

  /**
   * Join all worker-threads.
   */
  void join();

  /**
   * Detach all worker-threads.
   */
  void detach();

  /**
   * Stop Executor. <br>
   * After all worker-threads are stopped. Join should unblock.
   */
  void stop();

  /**
   * Execute Coroutine.
   * @tparam CoroutineType - type of coroutine to execute.
   * @tparam Args - types of arguments to be passed to Coroutine constructor.
   * @param params - actual arguments to be passed to Coroutine constructor.
   */
  template<typename CoroutineType, typename ... Args>
  void execute(Args... params) {
    auto& processor = m_processorWorkers[(++ m_balancer) % m_processorWorkers.size()];
    processor->execute<CoroutineType, Args...>(params...);
  }

  /**
   * Get number of all not finished tasks.
   * @return - number of all not finished tasks.
   */
  v_int32 getTasksCount();

  /**
   * Wait until all tasks are finished.
   * @param timeout
   */
  void waitTasksFinished(const std::chrono::duration<v_int64, std::micro>& timeout = std::chrono::minutes(1));
  
};
  
}}

#endif /* oatpp_async_Executor_hpp */
