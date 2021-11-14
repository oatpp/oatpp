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

#if defined(WIN32) || defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

namespace oatpp { namespace async { namespace worker {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IOEventWorker

IOEventWorker::IOEventWorker(IOEventWorkerForeman* foreman, Action::IOEventType specialization)
  : Worker(Type::IO)
  , m_foreman(foreman)
  , m_specialization(specialization)
  , m_running(true)
  , m_eventQueueHandle(INVALID_IO_HANDLE)
  , m_wakeupTrigger(INVALID_IO_HANDLE)
  , m_inEvents(nullptr)
  , m_inEventsCount(0)
  , m_inEventsCapacity(0)
  , m_outEvents(nullptr)
{
  m_thread = std::thread(&IOEventWorker::run, this);
}


IOEventWorker::~IOEventWorker() {
#if !defined(WIN32) && !defined(_WIN32)
  if(m_eventQueueHandle >=0) {
    ::close(m_eventQueueHandle);
  }

  if(m_wakeupTrigger >= 0) {
    ::close(m_wakeupTrigger);
  }
#endif
}


void IOEventWorker::pushTasks(utils::FastQueue<CoroutineHandle> &tasks) {
  if (tasks.first != nullptr) {
    {
      std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
      utils::FastQueue<CoroutineHandle>::moveAll(tasks, m_backlog);
    }
    triggerWakeup();
  }
}

void IOEventWorker::pushOneTask(CoroutineHandle *task) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.pushBack(task);
  }
  triggerWakeup();
}

void IOEventWorker::run() {

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

void IOEventWorker::join() {
  m_thread.join();
}

void IOEventWorker::detach() {
  m_thread.detach();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IOEventWorkerForeman

IOEventWorkerForeman::IOEventWorkerForeman()
  : Worker(Type::IO)
  , m_reader(this, Action::IOEventType::IO_EVENT_READ)
  , m_writer(this, Action::IOEventType::IO_EVENT_WRITE)
{}

IOEventWorkerForeman::~IOEventWorkerForeman() {
}

void IOEventWorkerForeman::pushTasks(utils::FastQueue<CoroutineHandle>& tasks) {

  utils::FastQueue<CoroutineHandle> readerTasks;
  utils::FastQueue<CoroutineHandle> writerTasks;

  while(tasks.first != nullptr) {

    CoroutineHandle* coroutine = tasks.popFront();
    auto& action = getCoroutineScheduledAction(coroutine);

    switch(action.getIOEventType()) {

      case Action::IOEventType::IO_EVENT_READ:
        readerTasks.pushBack(coroutine);
        break;

      case Action::IOEventType::IO_EVENT_WRITE:
        writerTasks.pushBack(coroutine);
        break;

      default:
        throw std::runtime_error("[oatpp::async::worker::IOEventWorkerForeman::pushTasks()]: Error. Unknown Action Event Type.");

    }

  }

  if(readerTasks.first != nullptr) {
    m_reader.pushTasks(readerTasks);
  }

  if(writerTasks.first != nullptr) {
    m_writer.pushTasks(writerTasks);
  }

}

void IOEventWorkerForeman::pushOneTask(CoroutineHandle* task) {

  auto& action = getCoroutineScheduledAction(task);

  switch(action.getIOEventType()) {

    case Action::IOEventType::IO_EVENT_READ:
      m_reader.pushOneTask(task);
      break;

    case Action::IOEventType::IO_EVENT_WRITE:
      m_writer.pushOneTask(task);
      break;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorkerForeman::pushTasks()]: Error. Unknown Action Event Type.");

  }

}

void IOEventWorkerForeman::stop() {
  m_writer.stop();
  m_reader.stop();
}

void IOEventWorkerForeman::join() {
  m_reader.join();
  m_writer.join();
}

void IOEventWorkerForeman::detach() {
  m_reader.detach();
  m_writer.detach();
}

}}}
