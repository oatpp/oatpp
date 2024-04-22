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

#include "Processor.hpp"
#include "./CoroutineWaitList.hpp"
#include "oatpp/async/worker/Worker.hpp"

namespace oatpp { namespace async {

void Processor::addWorker(const std::shared_ptr<worker::Worker>& worker) {

  switch(worker->getType()) {

    case worker::Worker::Type::IO:
      m_ioWorkers.push_back(worker);
      m_ioPopQueues.push_back(utils::FastQueue<CoroutineHandle>());
    break;

    case worker::Worker::Type::TIMER:
      m_timerWorkers.push_back(worker);
      m_timerPopQueues.push_back(utils::FastQueue<CoroutineHandle>());
    break;

    case worker::Worker::Type::PROCESSOR:
    case worker::Worker::Type::TYPES_COUNT:
    default:
      break;

  }

}

void Processor::popIOTask(CoroutineHandle* coroutine) {
  if(m_ioPopQueues.size() > 0) {
    auto &queue = m_ioPopQueues[(++m_ioBalancer) % m_ioPopQueues.size()];
    queue.pushBack(coroutine);
    //m_ioWorkers[(++m_ioBalancer) % m_ioWorkers.size()]->pushOneTask(coroutine);
  } else {
    throw std::runtime_error("[oatpp::async::Processor::popIOTasks()]: Error. Processor has no I/O workers.");
  }
}

void Processor::popTimerTask(CoroutineHandle* coroutine) {
  if(m_timerPopQueues.size() > 0) {
    auto &queue = m_timerPopQueues[(++m_timerBalancer) % m_timerPopQueues.size()];
    queue.pushBack(coroutine);
    //m_timerWorkers[(++m_timerBalancer) % m_timerWorkers.size()]->pushOneTask(coroutine);
  } else {
    throw std::runtime_error("[oatpp::async::Processor::popTimerTask()]: Error. Processor has no Timer workers.");
  }
}

void Processor::addCoroutine(CoroutineHandle* coroutine) {

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
        coroutine->_SCH_A = Action::clone(action);
        putCoroutineToSleep(coroutine);
        action.m_data.waitListData.waitList->add(coroutine);
        break;

      default:
        m_queue.pushBack(coroutine);

    }

  } else {
    throw std::runtime_error("[oatpp::async::processor::addTask()]: Error. Attempt to schedule coroutine to wrong processor.");
  }

}

void Processor::pushOneTask(CoroutineHandle* coroutine) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    m_pushList.pushBack(coroutine);
  }
  m_taskCondition.notify_one();
}

void Processor::pushTasks(utils::FastQueue<CoroutineHandle>& tasks) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    utils::FastQueue<CoroutineHandle>::moveAll(tasks, m_pushList);
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

  for(size_t i = 0; i < m_ioWorkers.size(); i++) {
    auto& worker = m_ioWorkers[i];
    auto& popQueue = m_ioPopQueues[i];
    worker->pushTasks(popQueue);
  }

  for(size_t i = 0; i < m_timerWorkers.size(); i++) {
    auto& worker = m_timerWorkers[i];
    auto& popQueue = m_timerPopQueues[i];
    worker->pushTasks(popQueue);
  }

}

void Processor::consumeAllTasks() {
  for(auto& submission : m_taskList) {
    m_queue.pushBack(submission->createCoroutine(this));
  }
  m_taskList.clear();
}

void Processor::pushQueues() {

  utils::FastQueue<CoroutineHandle> tmpList;

  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    consumeAllTasks();
    utils::FastQueue<CoroutineHandle>::moveAll(m_pushList, tmpList);
  }

  while(tmpList.first != nullptr) {
    addCoroutine(tmpList.popFront());
  }

}

void Processor::putCoroutineToSleep(CoroutineHandle* ch) {
  if(ch->_SCH_A.m_data.waitListData.timePointMicroseconds == 0) {
    std::lock_guard<std::mutex> lock(m_sleepMutex);
    m_sleepNoTimeSet.insert(ch);
  } else {
    std::lock_guard<std::mutex> lock(m_sleepMutex);
    m_sleepTimeSet.insert(ch);
    m_sleepCV.notify_one();
  }
}

void Processor::wakeCoroutine(CoroutineHandle* ch) {
  if(ch->_SCH_A.m_data.waitListData.timePointMicroseconds == 0) {
    std::lock_guard<std::mutex> lock(m_sleepMutex);
    m_sleepNoTimeSet.erase(ch);
  } else {
    std::lock_guard<std::mutex> lock(m_sleepMutex);
    m_sleepTimeSet.erase(ch);
  }
  ch->_SCH_A = Action::createActionByType(Action::TYPE_NONE);
  pushOneTask(ch);
}

void Processor::checkCoroutinesSleep() {
  while (m_running) {
    {
      std::unique_lock<std::mutex> lock{m_sleepMutex};
      while (m_running && m_sleepTimeSet.empty()) {
        m_sleepCV.wait(lock);
      }

      auto now = oatpp::Environment::getMicroTickCount();
      for(auto it = m_sleepTimeSet.begin(); it != m_sleepTimeSet.end();) {
        auto ch = *it;
        if(ch->_SCH_A.m_data.waitListData.timePointMicroseconds < now) {
          it = m_sleepTimeSet.erase(it);
          ch->_SCH_A.m_data.waitListData.waitList->forgetCoroutine(ch);
          ch->_SCH_A = Action::createActionByType(Action::TYPE_NONE);
          pushOneTask(ch);
        } else {
          it ++;
        }
      }

    }

    if(m_running) std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }
}

bool Processor::iterate(v_int32 numIterations) {

  pushQueues();

  for(v_int32 i = 0; i < numIterations; i++) {

    auto CP = m_queue.first;
    if (CP == nullptr) {
      break;
    }
    if (CP->finished()) {
      m_queue.popFrontNoData();
      -- m_tasksCounter;
    } else {

      const Action &action = CP->iterateAndTakeAction();

      switch (action.m_type) {

        case Action::TYPE_IO_WAIT:
          CP->_SCH_A = Action::clone(action);
          m_queue.popFront();
          popIOTask(CP);
          break;

        case Action::TYPE_WAIT_REPEAT:
          CP->_SCH_A = Action::clone(action);
          m_queue.popFront();
          popTimerTask(CP);
          break;

        case Action::TYPE_WAIT_LIST:
          CP->_SCH_A = Action::clone(action);
          m_queue.popFront();
          putCoroutineToSleep(CP);
          action.m_data.waitListData.waitList->add(CP);
          break;

        default:
          m_queue.round();
      }

    }

  }

  popTasks();

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
  return m_queue.first != nullptr || m_pushList.first != nullptr || !m_taskList.empty();
  
}

void Processor::stop() {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_taskLock);
    m_running = false;
  }
  m_taskCondition.notify_one();
  m_sleepCV.notify_one();

  m_sleepSetTask.join();

}

v_int32 Processor::getTasksCount() {
  return m_tasksCounter.load();
}

}}
