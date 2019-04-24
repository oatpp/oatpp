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

#include "oatpp/core/async/Processor.hpp"

#include <chrono>

#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BSD

#include <sys/event.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace oatpp { namespace async { namespace worker {

IOEventWorker::IOEventWorker()
  : Worker(Type::IO)
  , m_running(true)
  , m_eventQueueHandle(-1)
  , m_inEvents(nullptr)
  , m_inEventsCount(0)
  , m_outEvents(nullptr)
{

  OATPP_LOGD("IOEventWorker", "created");

  std::thread thread(&IOEventWorker::work, this);
  thread.detach();

}

IOEventWorker::~IOEventWorker() {
  if(m_inEvents != nullptr) {
    delete[] m_inEvents;
  }

  if(m_outEvents != nullptr) {
    delete[] m_outEvents;
  }
}

void IOEventWorker::initEventQueue() {

  m_eventQueueHandle = ::kqueue();
  if(m_eventQueueHandle == -1) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::initEventQueue()]: Error. Call to ::kqueue() failed.");
  }

  m_outEvents = (p_char8)(new struct kevent[MAX_EVENTS]);

}

void IOEventWorker::triggerWakeup() {

  struct kevent event;
  memset(&event, 0, sizeof(event));
  event.ident = 0;
  event.filter = EVFILT_USER;
  event.fflags = NOTE_TRIGGER;

  auto res = kevent(m_eventQueueHandle, &event, 1, nullptr, 0, NULL);

  if(res < 0) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::triggerWakeup()]: Error. trigger wakeup failed.");
  }

}

void IOEventWorker::setTriggerEvent(p_char8 eventPtr) {

  struct kevent* event = (struct kevent*) eventPtr;

  std::memset(event, 0, sizeof(struct kevent));

  event->ident = 0;
  event->filter = EVFILT_USER;
  event->flags = EV_ADD | EV_CLEAR;

}

void IOEventWorker::setCoroutineEvent(AbstractCoroutine* coroutine, p_char8 eventPtr) {

  auto& action = getCoroutineScheduledAction(coroutine);

  struct kevent* event = (struct kevent*) eventPtr;

  std::memset(event, 0, sizeof(struct kevent));

  event->ident = action.getIOHandle();
  event->flags = EV_ADD | EV_ONESHOT;
  event->udata = coroutine;

  switch(action.getIOEventType()) {

    case Action::IO_EVENT_READ:
      event->filter = EVFILT_READ;
      break;

    case Action::IO_EVENT_WRITE:
      event->filter = EVFILT_WRITE;
      break;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::pushCoroutineToQueue()]: Error. Unknown Action Event Type.");

  }

  switch(action.getType()) {

    case Action::TYPE_IO_WAIT:
      return;

    case Action::TYPE_IO_REPEAT:
      return;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::pushCoroutineToQueue()]: Error. Unknown Action.");

  }

}

void IOEventWorker::pushTasks(oatpp::collection::FastQueue<AbstractCoroutine>& tasks) {
  if(tasks.first != nullptr) {
    {
      std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
      oatpp::collection::FastQueue<AbstractCoroutine>::moveAll(tasks, m_backlog);
    }
    triggerWakeup();
  }
}

void IOEventWorker::pushOneTask(AbstractCoroutine* task) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> guard(m_backlogLock);
    m_backlog.pushBack(task);
  }
  triggerWakeup();
}

void IOEventWorker::consumeBacklog() {

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);

  m_inEventsCount = m_backlog.count + 1;
  m_inEvents = (p_char8)(new struct kevent[m_inEventsCount]);
  v_int32 eventSize = sizeof(struct kevent);

  setTriggerEvent(&m_inEvents[0]);

  auto curr = m_backlog.first;
  v_int32 i = 1;
  while(curr != nullptr) {
    setCoroutineEvent(curr, &m_inEvents[i * eventSize]);
    curr = nextCoroutine(curr);
    ++i;
  }

  m_backlog.first = nullptr;
  m_backlog.last = nullptr;
  m_backlog.count = 0;

}

void IOEventWorker::waitEvents() {

  auto eventsCount = kevent(m_eventQueueHandle, (struct kevent*)m_inEvents, m_inEventsCount, (struct kevent*)m_outEvents, MAX_EVENTS, NULL);

  if(eventsCount < 0) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::waitEvents()]: Error. Event loop failed.");
  }

  v_int32 eventSize = sizeof(struct kevent);

  oatpp::collection::FastQueue<AbstractCoroutine> m_repeatQueue;

  //OATPP_LOGD("IOEventWorker", "eventsCount=%d", eventsCount);

  for(v_int32 i = 0; i < eventsCount; i ++) {

    struct kevent* event = (struct kevent *)&m_outEvents[i * eventSize];
    auto coroutine = (AbstractCoroutine*) event->udata;

    if((event->flags & EV_ERROR) > 0) {
      OATPP_LOGD("Error", "data='%s'", strerror(event->data));
      continue;
    }

    if(coroutine != nullptr) {

      Action action = coroutine->iterate();

      switch(action.getType()) {

        case Action::TYPE_IO_WAIT:
          setCoroutineScheduledAction(coroutine, std::move(action));
          m_repeatQueue.pushBack(coroutine);
          break;

        case Action::TYPE_IO_REPEAT:
          setCoroutineScheduledAction(coroutine, std::move(action));
          m_repeatQueue.pushBack(coroutine);
          break;

        default:
          setCoroutineScheduledAction(coroutine, std::move(action));
          getCoroutineProcessor(coroutine)->pushOneTaskFromIO(coroutine);

      }

    }

  }

  if(m_repeatQueue.count > 0) {
    {
      std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);
      oatpp::collection::FastQueue<AbstractCoroutine>::moveAll(m_repeatQueue, m_backlog);
    }
  }

}

void IOEventWorker::work() {

  initEventQueue();

  while(m_running) {
    consumeBacklog();
    //OATPP_LOGD("IOEventWorker", "Waiting events...");
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