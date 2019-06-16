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

#include "Processor.hpp"
#include "./CoroutineWaitList.hpp"
#include "oatpp/core/async/worker/Worker.hpp"

namespace oatpp { namespace async {

void Processor::addWorker(const std::shared_ptr<worker::Worker>& worker) {

  switch(worker->getType()) {

    case worker::Worker::Type::IO:
      m_ioWorkers.push_back(worker);
      m_ioPopQueues.push_back(collection::FastQueue<AbstractCoroutine>());
    break;

    case worker::Worker::Type::TIMER:
      m_timerWorkers.push_back(worker);
      m_timerPopQueues.push_back(collection::FastQueue<AbstractCoroutine>());
    break;

    default:
      break;

  }

}

void Processor::popIOTask(AbstractCoroutine* coroutine) {
  if(m_ioPopQueues.size() > 0) {
    auto &queue = m_ioPopQueues[(++m_ioBalancer) % m_ioPopQueues.size()];
    queue.pushBack(coroutine);
    //m_ioWorkers[(++m_ioBalancer) % m_ioWorkers.size()]->pushOneTask(coroutine);
  } else {
    throw std::runtime_error("[oatpp::async::Processor::popIOTasks()]: Error. Processor has no I/O workers.");
  }
}

void Processor::popTimerTask(AbstractCoroutine* coroutine) {
  if(m_timerPopQueues.size() > 0) {
    auto &queue = m_timerPopQueues[(++m_timerBalancer) % m_timerPopQueues.size()];
    queue.pushBack(coroutine);
    //m_timerWorkers[(++m_timerBalancer) % m_timerWorkers.size()]->pushOneTask(coroutine);
  } else {
    throw std::runtime_error("[oatpp::async::Processor::popTimerTask()]: Error. Processor has no Timer workers.");
  }
}

void Processor::addCoroutine(AbstractCoroutine* coroutine) {

  if(coroutine->_PP == this) {

    const Action& action = coroutine->takeAction(std::move(coroutine->_SCH_A));

    switch(action.m_type) {

      case Action::TYPE_IO_REPEAT:
        coroutine->_SCH_A = Action::clone(action);
        popIOTask(coroutine);
        break;

      case Action::TYPE_IO_WAIT:
        coroutine->_SCH_A = Action::clone(action);
        popIOTask(coroutine);
        break;

      case Action::TYPE_WAIT_REPEAT:
        coroutine->_SCH_A = Action::clone(action);
        popTimerTask(coroutine);
        break;

      case Action::TYPE_WAIT_LIST:
        coroutine->_SCH_A = Action::createActionByType(Action::TYPE_NONE);
        action.m_data.waitList->pushBack(coroutine);
        break;

      default:
        m_queue.pushBack(coroutine);

    }

    action.m_type = Action::TYPE_NONE;

  } else {
    throw std::runtime_error("[oatpp::async::processor::addCoroutine()]: Error. Attempt to schedule coroutine to wrong processor.");
  }

}

void Processor::pushOneTask(AbstractCoroutine* coroutine) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    m_pushList.pushBack(coroutine);
  }
  m_taskCondition.notify_one();
}

void Processor::pushTasks(oatpp::collection::FastQueue<AbstractCoroutine>& tasks) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    collection::FastQueue<AbstractCoroutine>::moveAll(tasks, m_pushList);
  }
  m_taskCondition.notify_one();
}

void Processor::waitForTasks() {

  std::unique_lock<oatpp::concurrency::SpinLock> lock(m_taskLock);
  while (m_pushList.first == nullptr && m_taskList.empty() && m_running) {
    m_taskCondition.wait(lock);
  }

}

void Processor::popTasks() {

  for(v_int32 i = 0; i < m_ioWorkers.size(); i++) {
    auto& worker = m_ioWorkers[i];
    auto& popQueue = m_ioPopQueues[i];
    worker->pushTasks(popQueue);
  }

  for(v_int32 i = 0; i < m_timerWorkers.size(); i++) {
    auto& worker = m_timerWorkers[i];
    auto& popQueue = m_timerPopQueues[i];
    worker->pushTasks(popQueue);
  }

}

void Processor::consumeAllTasks() {
  for(auto& submission : m_taskList) {
    auto coroutine = submission->createCoroutine();
    coroutine->_PP = this;
    m_queue.pushBack(coroutine);
  }
  m_taskList.clear();
}

void Processor::pushQueues() {

  static constexpr v_int32 MAX_BATCH_SIZE = 1000;

  if(!m_taskList.empty()) {
    if (m_taskList.size() < MAX_BATCH_SIZE && m_queue.first != nullptr) {
      std::unique_lock<oatpp::concurrency::SpinLock> lock(m_taskLock, std::try_to_lock);
      if (lock.owns_lock()) {
        consumeAllTasks();
      }
    } else {
      std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
      consumeAllTasks();
    }
  }

  if(m_pushList.first != nullptr) {
    if (m_pushList.count < MAX_BATCH_SIZE && m_queue.first != nullptr) {
      std::unique_lock<oatpp::concurrency::SpinLock> lock(m_taskLock, std::try_to_lock);
      if (lock.owns_lock()) {
        while(m_pushList.first != nullptr) {
          addCoroutine(m_pushList.popFront());
        }
      }
    } else {
      std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
      while(m_pushList.first != nullptr) {
        addCoroutine(m_pushList.popFront());
      }
    }
  }

}

bool Processor::iterate(v_int32 numIterations) {

  pushQueues();

  for(v_int32 i = 0; i < numIterations; i++) {

    for(v_int32 j = 0; j < 10; j ++) {

      auto CP = m_queue.first;
      if (CP == nullptr) {
        goto end_loop;
      }
      if (CP->finished()) {
        m_queue.popFrontNoData();
        -- m_tasksCounter;
      } else {

        const Action &action = CP->takeAction(CP->iterate());

        switch (action.m_type) {

          case Action::TYPE_IO_WAIT:
            CP->_SCH_A = Action::clone(action);
            m_queue.popFront();
            popIOTask(CP);
            break;

//          case Action::TYPE_IO_REPEAT:  // DO NOT RESCHEDULE COROUTINE WITH ACTIVE I/O
//            CP->_SCH_A = Action::clone(action);
//            m_queue.popFront();
//            popIOTask(CP);
//            break;

          case Action::TYPE_WAIT_REPEAT:
            CP->_SCH_A = Action::clone(action);
            m_queue.popFront();
            popTimerTask(CP);
            break;

          case Action::TYPE_WAIT_LIST:
            CP->_SCH_A = Action::createActionByType(Action::TYPE_NONE);
            m_queue.popFront();
            action.m_data.waitList->pushBack(CP);
            break;

//        default:
//          m_queue.round();
        }

        action.m_type = Action::TYPE_NONE;

      }

    }

    m_queue.round();

  }

  end_loop:

  popTasks();
  
  return m_queue.first != nullptr || m_pushList.first != nullptr || !m_taskList.empty();
  
}

void Processor::stop() {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    m_running = false;
  }
  m_taskCondition.notify_one();
}

v_int32 Processor::getTasksCount() {
  return m_tasksCounter.load();
}

}}
