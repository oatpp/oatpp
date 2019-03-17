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

namespace oatpp { namespace async {

/**
 * Asynchronous Processor.<br>
 * Responsible for processing and managing multiple Coroutines.
 */
class Processor {
private:
  
  bool checkWaitingQueue();
  bool considerContinueImmediately();
  
private:
  oatpp::collection::FastQueue<AbstractCoroutine> m_activeQueue;
  oatpp::collection::FastQueue<AbstractCoroutine> m_waitingQueue;
private:
  v_int64 m_inactivityTick = 0;
public:

  /**
   * Add Coroutine to processor.
   * @param coroutine - pointer to Coroutine.
   */
  void addCoroutine(AbstractCoroutine* coroutine);

  /**
   * Add Coroutine to processor in "waiting queue"
   * @param coroutine
   */
  void addWaitingCoroutine(AbstractCoroutine* coroutine);

  /**
   * Iterate Coroutines.
   * @param numIterations - number of iterations.
   * @return - `true` if there are active Coroutines.
   */
  bool iterate(v_int32 numIterations);

  /**
   * Check if there is no more Coroutines in processor.
   * @return - `true` if all coroutines in all queues are finished.
   */
  bool isEmpty() {
    return m_activeQueue.first == nullptr && m_waitingQueue.first == nullptr;
  }
  
};
  
}}

#endif /* oatpp_async_Processor_hpp */
