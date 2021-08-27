/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include <iterator>
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
  std::lock_guard<oatpp::concurrency::SpinLock> guard{m_backlogLock};
  for (CoroutineHandle* coroutine : m_backlog) {
    delete coroutine;
  }
}


void IOEventWorker::pushTasks(std::vector<CoroutineHandle*> &tasks) {
  if (tasks.empty()) return;
  
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard{m_backlogLock};
    m_backlog.reserve(m_backlog.size() + tasks.size());
    std::move(std::begin(tasks), std::end(tasks), std::back_inserter(m_backlog));
    tasks.clear();
  }
  triggerWakeup();
}

void IOEventWorker::pushOneTask(CoroutineHandle *task) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.push_back(task);
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

void IOEventWorkerForeman::pushTasks(std::vector<CoroutineHandle*>& tasks) {

  std::vector<CoroutineHandle*> readerTasks;
  std::vector<CoroutineHandle*> writerTasks;

  for (CoroutineHandle* coroutine : tasks) {

    auto& action = getCoroutineScheduledAction(coroutine);

    switch(action.getIOEventType()) {

      case Action::IOEventType::IO_EVENT_READ:
        readerTasks.push_back(coroutine);
        break;

      case Action::IOEventType::IO_EVENT_WRITE:
        writerTasks.push_back(coroutine);
        break;

      default:
        throw std::runtime_error("[oatpp::async::worker::IOEventWorkerForeman::pushTasks()]: Error. Unknown Action Event Type.");

    }

  }
  tasks.clear();

  if(!readerTasks.empty()) {
    m_reader.pushTasks(readerTasks);
  }

  if(!writerTasks.empty()) {
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

bool IOEventWorkerForeman::abortCoroutine(v_uint64 coroutineId) {
  bool found = false;
  if (m_reader.abortCoroutine(coroutineId)) {
    found = true;
  }
  if (m_writer.abortCoroutine(coroutineId)) {
    found = true;
  }
  return found;
}

}}}
