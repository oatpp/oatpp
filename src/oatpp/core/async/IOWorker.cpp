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

#include "IOWorker.hpp"

#include <chrono>

namespace oatpp { namespace async {

void IOWorker::addTask(const Task& task) {
  {
    std::lock_guard<std::mutex> guard(m_backlogMutex);
    m_backlog.pushBack(task);
  }
  m_backlogCondition.notify_one();
}

void IOWorker::queueBacklog() {

  auto curr = m_backlog.getFirstNode();
  while(curr != nullptr) {
    m_queue.pushBack(new TaskSlot({curr->getData()}));
    curr = curr->getNext();
  }

  m_backlog.clear();

}

void IOWorker::consumeBacklog(bool blockToConsume) {

  if(blockToConsume) {

    std::unique_lock<std::mutex> lock(m_backlogMutex);
    while (m_backlog.count() == 0) {
      m_backlogCondition.wait(lock);
    }

  } else {

    std::unique_lock<std::mutex> lock(m_backlogMutex, std::try_to_lock);
    if (lock.owns_lock()) {
      queueBacklog();
    }

  }

}

void IOWorker::work() {

  v_int32 sleepIteration = 0;
  v_int32 consumeIteration = 0;

  while(m_running) {

    auto curr = m_queue.first;
    if(curr != nullptr) {

      const Action &action = curr->task.coroutine->iterate();
      if (action.getType() == Action::TYPE_WAIT_FOR_IO) {
        m_queue.round();
      } else {
        curr->task.sender->addTask(curr->task);
        m_queue.popFrontNoData();
      }

      ++ sleepIteration;
      if(sleepIteration == 1000) {
        sleepIteration = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      ++ consumeIteration;
      if(consumeIteration == 100) {
        consumeBacklog(false);
      }

    } else {
      sleepIteration = 0;
      consumeBacklog(true);
    }

  }

}

}}