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

#ifdef OATPP_IO_EVENT_INTERFACE_EPOLL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// epoll based implementation

#include "oatpp/core/async/Processor.hpp"

#include <unistd.h>
#include <sys/epoll.h>

namespace oatpp { namespace async { namespace worker {

void IOEventWorker::initEventQueue() {

  m_eventQueueHandle = ::epoll_create1(0);

  if(m_eventQueueHandle == -1) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::initEventQueue()]: Error. Call to ::epoll_create1() failed.");
  }

}

void IOEventWorker::triggerWakeup() {


}

void IOEventWorker::setTriggerEvent(p_char8 eventPtr) {


}

void IOEventWorker::setCoroutineEvent(AbstractCoroutine* coroutine, int operation, p_char8 eventPtr) {

  auto& action = getCoroutineScheduledAction(coroutine);

  switch(action.getType()) {

    case Action::TYPE_IO_WAIT: break;
    case Action::TYPE_IO_REPEAT: break;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::pushCoroutineToQueue()]: Error. Unknown Action.");

  }

  struct epoll_event event;
  std::memset(&event, 0, sizeof(struct epoll_event));

  event.data.ptr = coroutine;

  switch(action.getIOEventType()) {

    case Action::IO_EVENT_READ:
      event.events = EPOLLIN  | EPOLLET | EPOLLONESHOT | EPOLLEXCLUSIVE;
      break;

    case Action::IO_EVENT_WRITE:
      event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT | EPOLLEXCLUSIVE;
      break;

    default:
      throw std::runtime_error("[oatpp::async::worker::IOEventWorker::pushCoroutineToQueue()]: Error. Unknown Action Event Type.");

  }

  auto res = epoll_ctl(m_eventQueueHandle, operation, action.getIOHandle(), &event);
  if(res == -1) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::setEpollEvent()]: Error. Call to epoll_ctl failed.");
  }

}

void IOEventWorker::consumeBacklog() {

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_backlogLock);

  auto curr = m_backlog.first;
  while(curr != nullptr) {
    setCoroutineEvent(curr, EPOLL_CTL_ADD, nullptr);
    curr = nextCoroutine(curr);
  }

  m_backlog.first = nullptr;
  m_backlog.last = nullptr;
  m_backlog.count = 0;

}

void IOEventWorker::waitEvents() {

  auto eventsCount = epoll_wait(m_eventQueueHandle, (struct epoll_event*)m_outEvents, MAX_EVENTS, -1);

  if(eventsCount < 0) {
    throw std::runtime_error("[oatpp::async::worker::IOEventWorker::waitEvents()]: Error. Event loop failed.");
  }

  for(v_int32 i = 0; i < eventsCount; i ++) {

    struct epoll_event* event = (struct epoll_event*)&m_outEvents[i * sizeof(struct epoll_event)];
    auto coroutine = (AbstractCoroutine*) event->data.ptr;

    if(coroutine != nullptr) {

      Action action = coroutine->iterate();

      switch(action.getType()) {

        case Action::TYPE_IO_WAIT:
          setCoroutineScheduledAction(coroutine, std::move(action));
          setCoroutineEvent(coroutine, EPOLL_CTL_MOD, nullptr);
          break;

        case Action::TYPE_IO_REPEAT:
          setCoroutineScheduledAction(coroutine, std::move(action));
          setCoroutineEvent(coroutine, EPOLL_CTL_MOD, nullptr);
          break;

        default:
          setCoroutineScheduledAction(coroutine, std::move(action));
          getCoroutineProcessor(coroutine)->pushOneTaskFromIO(coroutine);
          setCoroutineEvent(coroutine, EPOLL_CTL_DEL, nullptr);

      }

    }

  }

}

}}}

#endif // #ifdef OATPP_IO_EVENT_INTERFACE_EPOLL