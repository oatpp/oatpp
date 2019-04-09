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

namespace oatpp { namespace async {

void Processor::addCoroutine(AbstractCoroutine* coroutine) {

  if(coroutine->_PP == this) {

    const Action& action = coroutine->_SCH_A;

    switch(action.m_type) {

      case Action::TYPE_WAIT_FOR_IO:
        coroutine->_SCH_A = Action::clone(action);
        m_queue.popFront();
        m_sch_pop_io_tmp.pushBack(coroutine);
        break;

      case Action::TYPE_WAIT_RETRY:
        coroutine->_SCH_A = Action::clone(action);
        m_queue.popFront();
        m_sch_pop_timer_tmp.pushBack(coroutine);
        break;

    }

    action.m_type = Action::TYPE_NONE;

    m_queue.pushBack(coroutine);

  } else {
    throw std::runtime_error("[oatpp::async::processor::addCoroutine()]: Error. Attempt to schedule coroutine to wrong processor.");
  }

}

void Processor::pushTaskFromIO(AbstractCoroutine* coroutine) {
  std::lock_guard<std::mutex> lock(m_sch_push_io_mutex);
  m_sch_push_io.pushBack(coroutine);
  m_waitCondition.notify_one();
}

void Processor::pushTaskFromTimer(AbstractCoroutine* coroutine) {
  std::lock_guard<std::mutex> lock(m_sch_push_timer_mutex);
  m_sch_push_timer.pushBack(coroutine);
  m_waitCondition.notify_one();
}

void Processor::popIOTasks(oatpp::collection::FastQueue<AbstractCoroutine>& queue) {
  if(m_sch_pop_io.first != nullptr) {
    std::lock_guard<std::mutex> lock(m_sch_pop_io_mutex);
    collection::FastQueue<AbstractCoroutine>::moveAll(m_sch_pop_io, queue);
  }
}

void Processor::popTimerTasks(oatpp::collection::FastQueue<AbstractCoroutine>& queue) {
  if(m_sch_pop_timer.first != nullptr) {
    std::lock_guard<std::mutex> lock(m_sch_pop_timer_mutex);
    collection::FastQueue<AbstractCoroutine>::moveAll(m_sch_pop_timer, queue);
  }
}

void Processor::waitForTasks() {

  std::unique_lock<std::mutex> lock(m_waitMutex);
  while (m_sch_push_io.first == nullptr && m_sch_push_timer.first == nullptr && m_taskList.empty() && m_running) {
    m_waitCondition.wait(lock);
  }

}

void Processor::popTmpQueues() {

  {
    std::lock_guard<std::mutex> lock(m_sch_pop_io_mutex);
    collection::FastQueue<AbstractCoroutine>::moveAll(m_sch_pop_io_tmp, m_sch_pop_io);
  }

  {
    std::lock_guard<std::mutex> lock(m_sch_pop_timer_mutex);
    collection::FastQueue<AbstractCoroutine>::moveAll(m_sch_pop_timer_tmp, m_sch_pop_timer);
  }

}

void Processor::pushAllFromQueue(oatpp::collection::FastQueue<AbstractCoroutine>& pushQueue) {
  auto curr = pushQueue.first;
  while(curr != nullptr) {
    addCoroutine(curr);
    curr = curr->_ref;
  }
  pushQueue.first = nullptr;
  pushQueue.last = nullptr;
  pushQueue.count = 0;
}

void Processor::consumeAllTasks() {
  for(auto& submission : m_taskList) {
    m_queue.pushBack(submission->createCoroutine());
  }
  m_taskList.clear();
}

void Processor::pushQueues() {

  static constexpr v_int32 MAX_BATCH_SIZE = 1000;

  if(!m_taskList.empty()) {
    if (m_taskList.size() < MAX_BATCH_SIZE && m_queue.first != nullptr) {
      std::unique_lock<std::mutex> lock(m_taskMutex, std::try_to_lock);
      if (lock.owns_lock()) {
        consumeAllTasks();
      }
    } else {
      std::lock_guard<std::mutex> lock(m_taskMutex);
      consumeAllTasks();
    }
  }

  if(m_sch_push_io.first != nullptr) {
    if (m_sch_push_io.count < MAX_BATCH_SIZE && m_queue.first != nullptr) {
      std::unique_lock<std::mutex> lock(m_sch_push_io_mutex, std::try_to_lock);
      if (lock.owns_lock()) {
        pushAllFromQueue(m_sch_push_io);
      }
    } else {
      std::lock_guard<std::mutex> lock(m_sch_push_io_mutex);
      pushAllFromQueue(m_sch_push_io);
    }
  }

  if(m_sch_push_timer.first != nullptr) {
    if (m_sch_push_timer.count < MAX_BATCH_SIZE && m_queue.first != nullptr) {
      std::unique_lock<std::mutex> lock(m_sch_push_timer_mutex, std::try_to_lock);
      if (lock.owns_lock()) {
        pushAllFromQueue(m_sch_push_timer);
      }
    } else {
      std::lock_guard<std::mutex> lock(m_sch_push_timer_mutex);
      pushAllFromQueue(m_sch_push_timer);
    }
  }

}

bool Processor::iterate(v_int32 numIterations) {

  pushQueues();

  for(v_int32 i = 0; i < numIterations; i++) {
    
    auto CP = m_queue.first;
    if(CP == nullptr) {
      break;
    }
    if(CP->finished()) {
      m_queue.popFrontNoData();
    } else {

      const Action& action = CP->iterate();

      switch(action.m_type) {

        case Action::TYPE_WAIT_FOR_IO:
          CP->_SCH_A = Action::clone(action);
          m_queue.popFront();
          m_sch_pop_io_tmp.pushBack(CP);
          OATPP_LOGD("Processor", "push to IO");
          break;

        case Action::TYPE_WAIT_RETRY:
          CP->_SCH_A = Action::clone(action);
          m_queue.popFront();
          m_sch_pop_timer_tmp.pushBack(CP);
          OATPP_LOGD("Processor", "push to Timer");
          break;

        default:
          m_queue.round();
      }

      action.m_type = Action::TYPE_NONE;

    }
  }

  popTmpQueues();
  
  return m_queue.first != nullptr ||
         m_sch_push_io.first != nullptr ||
         m_sch_push_timer.first != nullptr ||
         !m_taskList.empty();
  
}

void Processor::stop() {
  m_waitCondition.notify_one();
}

}}
