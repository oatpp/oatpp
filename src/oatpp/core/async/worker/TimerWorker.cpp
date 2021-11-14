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

#include "oatpp/core/async/Processor.hpp"

#include <chrono>

namespace oatpp { namespace async { namespace worker {

TimerWorker::TimerWorker(const std::chrono::duration<v_int64, std::micro>& granularity)
  : Worker(Type::TIMER)
  , m_running(true)
  , m_granularity(granularity)
{
  m_thread = std::thread(&TimerWorker::run, this);
}

void TimerWorker::pushTasks(utils::FastQueue<CoroutineHandle>& tasks) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    utils::FastQueue<CoroutineHandle>::moveAll(tasks, m_backlog);
  }
  m_backlogCondition.notify_one();
}

void TimerWorker::consumeBacklog() {

  std::unique_lock<oatpp::concurrency::SpinLock> lock(m_backlogLock);
  while (m_backlog.first == nullptr && m_queue.first == nullptr && m_running) {
    m_backlogCondition.wait(lock);
  }
  utils::FastQueue<CoroutineHandle>::moveAll(m_backlog, m_queue);

}

void TimerWorker::pushOneTask(CoroutineHandle* task) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.pushBack(task);
  }
  m_backlogCondition.notify_one();
}

void TimerWorker::run() {

  while(m_running) {

    consumeBacklog();
    auto curr = m_queue.first;
    CoroutineHandle* prev = nullptr;

    auto startTime = std::chrono::system_clock::now();
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(startTime.time_since_epoch());
    v_int64 tick = ms.count();

    while(curr != nullptr) {

      auto next = nextCoroutine(curr);

      const Action& schA = getCoroutineScheduledAction(curr);

      if(schA.getTimePointMicroseconds() < tick) {

        Action action = curr->iterate();

        switch(action.getType()) {

          case Action::TYPE_WAIT_REPEAT:
            setCoroutineScheduledAction(curr, std::move(action));
            break;

          case Action::TYPE_IO_WAIT:
            setCoroutineScheduledAction(curr, oatpp::async::Action::createWaitRepeatAction(0));
            break;

          default:
            m_queue.cutEntry(curr, prev);
            setCoroutineScheduledAction(curr, std::move(action));
            getCoroutineProcessor(curr)->pushOneTask(curr);
            curr = prev;
            break;

        }

      }

      prev = curr;
      curr = next;
    }

    auto elapsed = std::chrono::system_clock::now() - startTime;
    if(elapsed < m_granularity) {
      std::this_thread::sleep_for(m_granularity - elapsed);
    }

  }

}

void TimerWorker::stop() {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);
    m_running = false;
  }
  m_backlogCondition.notify_one();
}

void TimerWorker::join() {
  m_thread.join();
}

void TimerWorker::detach() {
  m_thread.detach();
}

}}}