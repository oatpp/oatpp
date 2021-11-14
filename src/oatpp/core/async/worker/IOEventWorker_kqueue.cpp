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

#ifdef OATPP_IO_EVENT_INTERFACE_KQUEUE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kqueue based implementation

#include "oatpp/core/async/Processor.hpp"

#include <sys/event.h>

namespace oatpp { namespace async { namespace worker {

void IOEventWorker::initEventQueue() {

  m_eventQueueHandle = ::kqueue();
  if(m_eventQueueHandle == -1) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::initEventQueue()]: Error. Call to ::kqueue() failed.");
  }

  m_outEvents = std::unique_ptr<v_char8[]>(new (std::nothrow) v_char8[MAX_EVENTS * sizeof(struct kevent)]);
  if(!m_outEvents) {
    OATPP_LOGE("[oatpp::async::worker::IOEventWorker::initEventQueue()]",
               "Error. Unable to allocate %d bytes for events.", MAX_EVENTS * sizeof(struct kevent));
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::initEventQueue()]: Error. Unable to allocate memory for events.");
  }

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

void IOEventWorker::setCoroutineEvent(CoroutineHandle* coroutine, int operation, p_char8 eventPtr) {

  (void)operation;
  auto& action = getCoroutineScheduledAction(coroutine);

  switch(action.getType()) {

    case Action::TYPE_IO_WAIT: break;
    case Action::TYPE_IO_REPEAT: break;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::pushCoroutineToQueue()]: Error. Unknown Action.");

  }

  struct kevent* event = (struct kevent*) eventPtr;
  std::memset(event, 0, sizeof(struct kevent));

  event->ident = action.getIOHandle();
  event->flags = EV_ADD | EV_ONESHOT;
  event->udata = coroutine;

  switch(action.getIOEventType()) {

    case Action::IOEventType::IO_EVENT_READ:
      event->filter = EVFILT_READ;
      break;

    case Action::IOEventType::IO_EVENT_WRITE:
      event->filter = EVFILT_WRITE;
      break;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::pushCoroutineToQueue()]: Error. Unknown Action Event Type.");

  }

}

void IOEventWorker::consumeBacklog() {

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);

  m_inEventsCount = m_backlog.count + 1;

  if(m_inEventsCapacity < m_inEventsCount) {

    m_inEventsCapacity = m_inEventsCount;

    m_inEvents = std::unique_ptr<v_char8[]>(new (std::nothrow) v_char8[m_inEventsCapacity * sizeof(struct kevent)]);
    if(!m_inEvents) {
      OATPP_LOGE("[oatpp::async::worker::IOEventWorker::consumeBacklog()]",
                 "Error. Unable to allocate %d bytes for events.", m_inEventsCapacity * sizeof(struct kevent));
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::consumeBacklog()]: Error. Unable to allocate memory for events.");
    }

  }

  setTriggerEvent(&m_inEvents[0]);

  auto curr = m_backlog.first;
  v_int32 i = 1;
  while(curr != nullptr) {
    setCoroutineEvent(curr, 0, &m_inEvents[i * sizeof(struct kevent)]);
    curr = nextCoroutine(curr);
    ++i;
  }

  m_backlog.first = nullptr;
  m_backlog.last = nullptr;
  m_backlog.count = 0;

}

void IOEventWorker::waitEvents() {

  auto eventsCount = kevent(m_eventQueueHandle, (struct kevent*)m_inEvents.get(), m_inEventsCount, (struct kevent*)m_outEvents.get(), MAX_EVENTS, NULL);

  if((eventsCount < 0) && (errno != EINTR)) {
    OATPP_LOGE("[oatpp::async::worker::IOEventWorker::waitEvents()]", "Error:\n"
               "errno=%d\n"
               "in-events=%d\n"
               "foreman=%d\n"
               "this=%d\n"
               "specialization=%d",
               errno, m_inEventsCount, m_foreman, this, m_specialization);
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::waitEvents()]: Error. Event loop failed.");
  }

  utils::FastQueue<CoroutineHandle> repeatQueue;
  utils::FastQueue<CoroutineHandle> popQueue;

  for(v_int32 i = 0; i < eventsCount; i ++) {

    struct kevent* event = (struct kevent *)&m_outEvents[i * sizeof(struct kevent)];
    auto coroutine = (CoroutineHandle*) event->udata;

    if((event->flags & EV_ERROR) > 0) {
      OATPP_LOGD("Error", "data='%s'", strerror((int)event->data));
      continue;
    }

    if(coroutine != nullptr) {

      Action action = coroutine->iterate();

      switch(action.getIOEventCode() | m_specialization) {

        case Action::CODE_IO_WAIT_READ:
          setCoroutineScheduledAction(coroutine, std::move(action));
          repeatQueue.pushBack(coroutine);
          break;

        case Action::CODE_IO_WAIT_WRITE:
          setCoroutineScheduledAction(coroutine, std::move(action));
          repeatQueue.pushBack(coroutine);
          break;

        case Action::CODE_IO_REPEAT_READ:
          setCoroutineScheduledAction(coroutine, std::move(action));
          repeatQueue.pushBack(coroutine);
          break;

        case Action::CODE_IO_REPEAT_WRITE:
          setCoroutineScheduledAction(coroutine, std::move(action));
          repeatQueue.pushBack(coroutine);
          break;

        case Action::CODE_IO_WAIT_RESCHEDULE:
          setCoroutineScheduledAction(coroutine, std::move(action));
          popQueue.pushBack(coroutine);
          break;

        case Action::CODE_IO_REPEAT_RESCHEDULE:
          setCoroutineScheduledAction(coroutine, std::move(action));
          popQueue.pushBack(coroutine);
          break;

        default:
          setCoroutineScheduledAction(coroutine, std::move(action));
          getCoroutineProcessor(coroutine)->pushOneTask(coroutine);

      }

    }

  }

  if(repeatQueue.count > 0) {
    {
      std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);
      utils::FastQueue<CoroutineHandle>::moveAll(repeatQueue, m_backlog);
    }
  }

  if(popQueue.count > 0) {
    m_foreman->pushTasks(popQueue);
  }

}

}}}

#endif // #ifdef OATPP_IO_EVENT_INTERFACE_KQUEUE
