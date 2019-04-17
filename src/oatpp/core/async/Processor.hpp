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

#ifndef oatpp_async_Processor_hpp
#define oatpp_async_Processor_hpp

#include "./Coroutine.hpp"
#include "oatpp/core/collection/FastQueue.hpp"

#include <mutex>
#include <list>
#include <vector>
#include <condition_variable>

namespace oatpp { namespace async {

/**
 * Asynchronous Processor.<br>
 * Responsible for processing and managing multiple Coroutines.
 */
class Processor {
private:

  class TaskSubmission {
  public:
    virtual ~TaskSubmission() {};
    virtual AbstractCoroutine* createCoroutine() = 0;
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

    virtual AbstractCoroutine* createCoroutine() {
      return creator(typename SequenceGenerator<sizeof...(Args)>::type());
    }

    template<int ...S>
    AbstractCoroutine* creator(IndexSequence<S...>) {
      return new CoroutineType(std::get<S>(m_params) ...);
    }

  };

private:

  oatpp::collection::FastQueue<AbstractCoroutine> m_sch_push_io;
  oatpp::collection::FastQueue<AbstractCoroutine> m_sch_push_timer;

  oatpp::concurrency::SpinLock::Atom m_sch_push_io_atom;
  oatpp::concurrency::SpinLock::Atom m_sch_push_timer_atom;

private:

  std::vector<std::shared_ptr<worker::Worker>> m_ioWorkers;
  std::vector<std::shared_ptr<worker::Worker>> m_timerWorkers;

  std::vector<oatpp::collection::FastQueue<AbstractCoroutine>> m_ioPopQueues;
  std::vector<oatpp::collection::FastQueue<AbstractCoroutine>> m_timerPopQueues;

  v_word32 m_ioBalancer = 0;
  v_word32 m_timerBalancer = 0;

private:

  std::mutex m_taskMutex;
  std::list<std::shared_ptr<TaskSubmission>> m_taskList;

private:

  oatpp::collection::FastQueue<AbstractCoroutine> m_queue;

private:

  std::mutex m_waitMutex;
  std::condition_variable m_waitCondition;

private:

  bool m_running = true;

private:

  void popIOTask(AbstractCoroutine* coroutine);
  void popTimerTask(AbstractCoroutine* coroutine);

  void consumeAllTasks();
  void addCoroutine(AbstractCoroutine* coroutine);
  void popTasks();
  void pushAllFromQueue(oatpp::collection::FastQueue<AbstractCoroutine>& pushQueue);
  void pushQueues();

public:

  Processor()
    : m_sch_push_io_atom(false)
    , m_sch_push_timer_atom(false)
  {}

  void addWorker(const std::shared_ptr<worker::Worker>& worker);

  /**
   * Return coroutine scheduled for I/O back to owner processor.
   * @param coroutine
   */
  void pushOneTaskFromIO(AbstractCoroutine* coroutine);

  /**
   * Return coroutine scheduled for Timer back to owner processor.
   * @param coroutine
   */
  void pushOneTaskFromTimer(AbstractCoroutine* coroutine);

  void pushTasksFromTimer(oatpp::collection::FastQueue<AbstractCoroutine>& tasks);

  /**
   * Execute Coroutine.
   * @tparam CoroutineType - type of coroutine to execute.
   * @tparam Args - types of arguments to be passed to Coroutine constructor.
   * @param params - actual arguments to be passed to Coroutine constructor.
   */
  template<typename CoroutineType, typename ... Args>
  void execute(Args... params) {
    auto submission = std::make_shared<SubmissionTemplate<CoroutineType, Args...>>(params...);
    std::lock_guard<std::mutex> lock(m_taskMutex);
    m_taskList.push_back(submission);
    m_waitCondition.notify_one();
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

  
};
  
}}

#endif /* oatpp_async_Processor_hpp */
