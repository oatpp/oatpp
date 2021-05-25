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
#include <set>

namespace oatpp { namespace async { namespace worker {

TimerWorker::TimerWorker(const std::chrono::duration<v_int64, std::micro>& granularity)
  : Worker(Type::TIMER)
  , m_running(true)
  , m_granularity(granularity)
{
  m_thread = std::thread(&TimerWorker::run, this);
}

TimerWorker::~TimerWorker() {
  std::lock_guard<oatpp::concurrency::SpinLock> guard{m_backlogLock};
  for (CoroutineHandle* coroutine : m_backlog) {
    delete coroutine;
  }
  for (CoroutineHandle* coroutine : m_queue) {
    delete coroutine;
  }
}

void TimerWorker::pushTasks(std::vector<CoroutineHandle*>& tasks) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.reserve(m_backlog.size() + tasks.size());
    std::move(std::begin(tasks), std::end(tasks), std::back_inserter(m_backlog));
    tasks.clear();
  }
  m_backlogCondition.notify_one();
}

void TimerWorker::consumeBacklog() {

  std::unique_lock<oatpp::concurrency::SpinLock> lock(m_backlogLock);
  while (m_backlog.empty() && m_queue.empty() && m_running) {
    m_backlogCondition.wait(lock);
  }
  m_queue.reserve(m_queue.size() + m_backlog.size());
  std::move(std::begin(m_backlog), std::end(m_backlog), std::back_inserter(m_queue));
  m_backlog.clear();

}

void TimerWorker::pushOneTask(CoroutineHandle* task) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.push_back(task);
  }
  m_backlogCondition.notify_one();
}

void TimerWorker::run() {

  while(m_running) {

    consumeBacklog();

    auto startTime = std::chrono::system_clock::now();
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(startTime.time_since_epoch());
    v_int64 tick = ms.count();

    std::set<CoroutineHandle*> elementsToErase;
    for (CoroutineHandle* curr : m_queue) {
       
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
            elementsToErase.insert(curr);
            setCoroutineScheduledAction(curr, std::move(action));
            getCoroutineProcessor(curr)->pushOneTask(curr);
            break;

        }

      }
    }

    m_queue.erase(std::remove_if(std::begin(m_queue), std::end(m_queue),
      [&](CoroutineHandle* curr) {
        return elementsToErase.count(curr) == 1;
      }), std::end(m_queue));

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