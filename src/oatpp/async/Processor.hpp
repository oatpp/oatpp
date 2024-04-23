/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
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

#ifndef oatpp_async_Processor_hpp
#define oatpp_async_Processor_hpp

#include "./Coroutine.hpp"
#include "./CoroutineWaitList.hpp"
#include "oatpp/async/utils/FastQueue.hpp"
#include "oatpp/concurrency/SpinLock.hpp"

#include <thread>
#include <condition_variable>
#include <list>
#include <mutex>
#include <set>
#include <vector>

namespace oatpp { namespace async {

/**
 * Asynchronous Processor.<br>
 * Responsible for processing and managing multiple Coroutines.
 * Do not use bare processor to run coroutines. Use &id:oatpp::async::Executor; instead;.
 */
class Processor {
    friend class CoroutineWaitList;
private:

  class TaskSubmission {
  public:
    virtual ~TaskSubmission() = default;
    virtual CoroutineHandle* createCoroutine(Processor* processor) = 0;
  };

  /*
   * Sequence generating templates
   * used to convert tuple to parameters pack
   * Example: expand SequenceGenerator<3>:
   * // 2, 2, {} // 1, 1, {2} // 0, 0, {1, 2} // 0, {0, 1, 2}
   * where {...} is int...S
   */
  template<int ...> struct IndexSequence {};
  template<int N, int ...S> struct SequenceGenerator : SequenceGenerator <N - 1, N - 1, S...> {};
  template<int ...S>
  struct SequenceGenerator<0, S...> {
    typedef IndexSequence<S...> type;
  };

  template<typename CoroutineType, typename ... Args>
  class SubmissionTemplate : public TaskSubmission {
  private:
    std::tuple<Args...> m_params;
  public:

    SubmissionTemplate(Args... params)
      : m_params(std::make_tuple(params...))
    {}

    virtual CoroutineHandle* createCoroutine(Processor* processor) override {
      return creator(processor, typename SequenceGenerator<sizeof...(Args)>::type());
    }

    template<int ...S>
    CoroutineHandle* creator(Processor* processor, IndexSequence<S...>) {
      return new CoroutineHandle(processor, new CoroutineType(std::get<S>(m_params) ...));
    }

  };

private:

  std::vector<std::shared_ptr<worker::Worker>> m_ioWorkers;
  std::vector<std::shared_ptr<worker::Worker>> m_timerWorkers;

  std::vector<utils::FastQueue<CoroutineHandle>> m_ioPopQueues;
  std::vector<utils::FastQueue<CoroutineHandle>> m_timerPopQueues;

  v_uint32 m_ioBalancer = 0;
  v_uint32 m_timerBalancer = 0;

private:

  std::unordered_set<CoroutineHandle*> m_sleepNoTimeSet;
  std::unordered_set<CoroutineHandle*> m_sleepTimeSet;
  std::mutex m_sleepMutex;
  std::condition_variable m_sleepCV;

  std::thread m_sleepSetTask{&Processor::checkCoroutinesSleep, this};

private:

  oatpp::concurrency::SpinLock m_taskLock;
  std::condition_variable_any m_taskCondition;
  std::list<std::shared_ptr<TaskSubmission>> m_taskList;
  utils::FastQueue<CoroutineHandle> m_pushList;

private:

  utils::FastQueue<CoroutineHandle> m_queue;

private:
  std::atomic_bool m_running{true};
  std::atomic<v_int32> m_tasksCounter{0};
private:

  void popIOTask(CoroutineHandle* coroutine);
  void popTimerTask(CoroutineHandle* coroutine);

  void consumeAllTasks();
  void addCoroutine(CoroutineHandle* coroutine);
  void popTasks();
  void pushQueues();

  void putCoroutineToSleep(CoroutineHandle* ch);
  void wakeCoroutine(CoroutineHandle* ch);
  void checkCoroutinesSleep();

public:

  Processor() = default;

  /**
   * Add dedicated co-worker to processor.
   * @param worker - &id:oatpp::async::worker::Worker;.
   */
  void addWorker(const std::shared_ptr<worker::Worker>& worker);

  /**
   * Push one Coroutine back to processor.
   * @param coroutine - &id:oatpp::async::CoroutineHandle; previously popped-out(rescheduled to coworker) from this processor.
   */
  void pushOneTask(CoroutineHandle* coroutine);

  /**
   * Push list of Coroutines back to processor.
   * @param tasks - &id:oatpp::async::utils::FastQueue; of &id:oatpp::async::CoroutineHandle; previously popped-out(rescheduled to coworker) from this processor.
   */
  void pushTasks(utils::FastQueue<CoroutineHandle>& tasks);

  /**
   * Execute Coroutine.
   * @tparam CoroutineType - type of coroutine to execute.
   * @tparam Args - types of arguments to be passed to Coroutine constructor.
   * @param params - actual arguments to be passed to Coroutine constructor.
   */
  template<typename CoroutineType, typename ... Args>
  void execute(Args... params) {
    auto submission = std::make_shared<SubmissionTemplate<CoroutineType, Args...>>(params...);
    ++ m_tasksCounter;
    {
      std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
      m_taskList.push_back(submission);
    }
    m_taskCondition.notify_one();
  }

  /**
   * Sleep and wait for tasks.
   */
  void waitForTasks();

  /**
   * Iterate Coroutines.
   * @param numIterations - number of iterations.
   * @return - `true` if there are active Coroutines.
   */
  bool iterate(v_int32 numIterations);

  /**
   * Stop waiting for new tasks.
   */
  void stop();

  /**
   * Get number of all not-finished tasks including tasks rescheduled for processor's co-workers.
   * @return - number of not-finished tasks.
   */
  v_int32 getTasksCount();

  
};
  
}}

#endif /* oatpp_async_Processor_hpp */
