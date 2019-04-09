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
#include "./Worker.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

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
  
  class SubmissionProcessor/* : public Worker */{
  private:
    oatpp::async::Processor m_processor;
  private:
    bool m_isRunning;
  public:
    SubmissionProcessor();
  public:
    
    void run();
    void stop() ;

    template<typename CoroutineType, typename ... Args>
    void execute(Args... params) {
      m_processor.execute<CoroutineType, Args...>(params...);
    }
    
  };

public:
  /**
   * Default number of threads to run coroutines.
   */
  static const v_int32 THREAD_NUM_DEFAULT;
private:
  v_int32 m_threadsCount;
  //std::shared_ptr<oatpp::concurrency::Thread>* m_threads;
  std::thread* m_threads;
  SubmissionProcessor* m_processors;
  std::atomic<v_word32> m_balancer;
public:

  /**
   * Constructor.
   * @param threadsCount - Number of threads to run coroutines.
   */
  Executor(v_int32 threadsCount = THREAD_NUM_DEFAULT);

  /**
   * Non-virtual Destructor.
   */
  ~Executor();

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
    auto& processor = m_processors[m_balancer % m_threadsCount];
    processor.execute<CoroutineType, Args...>(params...);
    m_balancer ++;
  }
  
};
  
}}

#endif /* oatpp_async_Executor_hpp */
