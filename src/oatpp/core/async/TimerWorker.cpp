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

#include "TimerWorker.hpp"

#include "Processor.hpp"

#include <chrono>

namespace oatpp { namespace async {

void TimerWorker::pushTasks(oatpp::collection::FastQueue<AbstractCoroutine>& tasks) {
  {
    std::lock_guard<std::mutex> guard(m_backlogMutex);
    oatpp::collection::FastQueue<AbstractCoroutine>::moveAll(tasks, m_backlog);
  }
  m_backlogCondition.notify_one();
}

void TimerWorker::consumeBacklog(bool blockToConsume) {

  if(blockToConsume) {

    std::unique_lock<std::mutex> lock(m_backlogMutex);
    while (m_backlog.first == nullptr && m_running) {
      m_backlogCondition.wait(lock);
    }
    oatpp::collection::FastQueue<AbstractCoroutine>::moveAll(m_backlog, m_queue);
  } else {

    std::unique_lock<std::mutex> lock(m_backlogMutex, std::try_to_lock);
    if (lock.owns_lock()) {
      oatpp::collection::FastQueue<AbstractCoroutine>::moveAll(m_backlog, m_queue);
    }

  }

}

void TimerWorker::work() {

  v_int32 consumeIteration = 0;
  v_int32 roundIteration = 0;

  while(m_running) {

    auto CP = m_queue.first;
    if(CP != nullptr) {

      Action action = CP->iterate();
      if (action.getType() == Action::TYPE_WAIT_RETRY) {
        ++ roundIteration;
        if(roundIteration == 10) {
          roundIteration = 0;
          m_queue.round();
        }
      } else {
        roundIteration = 0;
        m_queue.popFront();
        setCoroutineScheduledAction(CP, std::move(action));
        getCoroutineProcessor(CP)->pushOneTaskFromTimer(CP);
      }

      ++ consumeIteration;
      if(consumeIteration == 100) {
        consumeIteration = 0;
        consumeBacklog(false);
      }

    } else {
      consumeBacklog(true);
    }

  }

}

}}