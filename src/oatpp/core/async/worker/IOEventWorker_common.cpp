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

#include "IOEventWorker.hpp"

#include <unistd.h>

namespace oatpp { namespace async { namespace worker {

IOEventWorker::IOEventWorker()
  : Worker(Type::IO)
  , m_running(true)
  , m_eventQueueHandle(-1)
  , m_wakeupTrigger(-1)
  , m_inEvents(nullptr)
  , m_inEventsCount(0)
  , m_outEvents(nullptr)
{

  std::thread thread(&IOEventWorker::work, this);
  thread.detach();

}

IOEventWorker::~IOEventWorker() {

  if (m_inEvents != nullptr) {
    delete[] m_inEvents;
  }

  if (m_outEvents != nullptr) {
    delete[] m_outEvents;
  }

  if(m_eventQueueHandle >=0) {
    ::close(m_eventQueueHandle);
  }

  if(m_wakeupTrigger >= 0) {
    ::close(m_wakeupTrigger);
  }

}

void IOEventWorker::pushTasks(oatpp::collection::FastQueue<AbstractCoroutine> &tasks) {
  if (tasks.first != nullptr) {
    {
      std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
      oatpp::collection::FastQueue<AbstractCoroutine>::moveAll(tasks, m_backlog);
    }
    triggerWakeup();
  }
}

void IOEventWorker::pushOneTask(AbstractCoroutine *task) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.pushBack(task);
  }
  triggerWakeup();
}

void IOEventWorker::work() {

  initEventQueue();

  while (m_running) {
    consumeBacklog();
    waitEvents();
  }

}

void IOEventWorker::stop() {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);
    m_running = false;
  }
  triggerWakeup();
}

}}}
