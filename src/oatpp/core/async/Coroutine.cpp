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

#include "Coroutine.hpp"

namespace oatpp { namespace async {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Action

Action Action::clone(const Action& action) {
  Action result(action.m_type);
  result.m_data = action.m_data;
  return result;
}

Action Action::createActionByType(v_int32 type) {
  return Action(type);
}

Action Action::createIOWaitAction(v_io_handle ioHandle, Action::IOEventType ioEventType) {
  Action result(TYPE_IO_WAIT);
  result.m_data.ioData.ioHandle = ioHandle;
  result.m_data.ioData.ioEventType = ioEventType;
  return result;
}

Action Action::createIORepeatAction(v_io_handle ioHandle, Action::IOEventType ioEventType) {
  Action result(TYPE_IO_REPEAT);
  result.m_data.ioData.ioHandle = ioHandle;
  result.m_data.ioData.ioEventType = ioEventType;
  return result;
}

Action Action::createWaitRepeatAction(v_int64 timePointMicroseconds) {
  Action result(TYPE_WAIT_REPEAT);
  result.m_data.timePointMicroseconds = timePointMicroseconds;
  return result;
}

Action Action::createWaitListAction(CoroutineWaitList* waitList) {
  Action result(TYPE_WAIT_LIST);
  result.m_data.waitList = waitList;
  return result;
}

Action Action::createWaitListActionWithTimeout(CoroutineWaitList* waitList, const std::chrono::steady_clock::time_point& timeout) {
  Action result(TYPE_WAIT_LIST_WITH_TIMEOUT);
  result.m_data.waitListWithTimeout.waitList = waitList;
  result.m_data.waitListWithTimeout.timeoutTimeSinceEpochMS = std::chrono::duration_cast<std::chrono::milliseconds>(timeout.time_since_epoch()).count();
  return result;
}

Action::Action()
  : m_type(TYPE_NONE)
{}

Action::Action(AbstractCoroutine* coroutine)
  : m_type(TYPE_COROUTINE)
{
  m_data.coroutine = coroutine;
}

Action::Action(const FunctionPtr& functionPtr)
  : m_type(TYPE_YIELD_TO)
{
  m_data.fptr = functionPtr;
}

Action::Action(Error* error)
  : m_type(TYPE_ERROR)
{
  m_data.error = error;
}

Action::Action(v_int32 type)
  : m_type(type)
{}

Action::Action(Action&& other)
  : m_type(other.m_type)
  , m_data(other.m_data)
{
  other.m_type = TYPE_NONE;
}

Action::~Action() {
  free();
}

void Action::free() {
  switch(m_type) {
    case TYPE_COROUTINE:
      delete m_data.coroutine;
      break;

    case TYPE_ERROR:
      delete m_data.error;
      break;
  }
  m_type = TYPE_NONE;
}

Action& Action::operator=(Action&& other) {
  free();
  m_type = other.m_type;
  m_data = other.m_data;
  other.m_data.fptr = nullptr;
  return *this;
}

bool Action::isError() const {
  return m_type == TYPE_ERROR;
}

bool Action::isNone() const {
  return m_type == TYPE_NONE;
}

v_int32 Action::getType() const {
  return m_type;
}

v_int64 Action::getTimePointMicroseconds() const {
  return m_data.timePointMicroseconds;
}

oatpp::v_io_handle Action::getIOHandle() const {
  return m_data.ioData.ioHandle;
}

Action::IOEventType Action::getIOEventType() const {
  return m_data.ioData.ioEventType;
}

v_int32 Action::getIOEventCode() const {
  return m_type | m_data.ioData.ioEventType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CoroutineStarter

CoroutineStarter::CoroutineStarter(AbstractCoroutine* coroutine)
  : m_first(coroutine)
  , m_last(coroutine)
{}

CoroutineStarter::CoroutineStarter(CoroutineStarter&& other)
  : m_first(other.m_first)
  , m_last(other.m_last)
{
  other.m_first = nullptr;
  other.m_last = nullptr;
}

CoroutineStarter::~CoroutineStarter() {
  freeCoroutines();
}

/*
 * Move assignment operator.
 */
CoroutineStarter& CoroutineStarter::operator=(CoroutineStarter&& other) {
  if (this == std::addressof(other)) return *this;

  freeCoroutines();
  m_first = other.m_first;
  m_last = other.m_last;
  other.m_first = nullptr;
  other.m_last = nullptr;
  return *this;
}

Action CoroutineStarter::next(Action&& action) {
  if(m_last == nullptr) {
    return std::forward<Action>(action);
  }
  m_last->m_parentReturnAction = std::forward<Action>(action);
  Action result = m_first;
  m_first = nullptr;
  m_last = nullptr;
  return result;
}

CoroutineStarter& CoroutineStarter::next(CoroutineStarter&& starter) {
  if(m_last == nullptr) {
    m_first = starter.m_first;
    m_last = starter.m_last;
  } else {
    m_last->m_parentReturnAction = starter.m_first;
    m_last = starter.m_last;
  }
  starter.m_first = nullptr;
  starter.m_last = nullptr;
  return *this;
}

void CoroutineStarter::freeCoroutines()
{
  if (m_first != nullptr) {
    auto curr = m_first;
    while (curr != nullptr) {
      AbstractCoroutine* next = nullptr;
      if (curr->m_parentReturnAction.m_type == Action::TYPE_COROUTINE) {
        next = curr->m_parentReturnAction.m_data.coroutine;
      }
      delete curr;
      curr = next;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CoroutineHandle

CoroutineHandle::CoroutineHandle(Processor* processor, AbstractCoroutine* rootCoroutine)
  : _PP(processor)
  , _CP(rootCoroutine)
  , _FP(&AbstractCoroutine::act)
  , _SCH_A(Action::TYPE_NONE)
  , _ref(nullptr)
{}

CoroutineHandle::~CoroutineHandle() {
  delete _CP;
}

Action CoroutineHandle::takeAction(Action&& action) {

  //v_int32 iterations = 0;

  while (true) {

    switch (action.m_type) {

      case Action::TYPE_COROUTINE: {
        action.m_data.coroutine->m_parent = _CP;
        action.m_data.coroutine->m_parentReturnFP = _FP;
        _CP = action.m_data.coroutine;
        _FP = &AbstractCoroutine::act;
        action.m_type = Action::TYPE_NONE;
        return std::forward<oatpp::async::Action>(action);
      }

      case Action::TYPE_FINISH: {
        /* Please note that savedCP->m_parentReturnAction should not be "REPEAT nor WAIT_RETRY" */
        /* as funtion pointer (FP) is invalidated */
        action = std::move(_CP->m_parentReturnAction);
        AbstractCoroutine* savedCP = _CP;
        _FP = _CP->m_parentReturnFP;
        _CP = _CP->m_parent;
        delete savedCP;
        continue;
      }

      case Action::TYPE_YIELD_TO: {
        _FP = action.m_data.fptr;
        //break;
        return std::forward<oatpp::async::Action>(action);
      }

//      case Action::TYPE_REPEAT: {
//        break;
//      }
//
//      case Action::TYPE_IO_REPEAT: {
//        break;
//      }

      case Action::TYPE_ERROR: {
        Action newAction = _CP->handleError(action.m_data.error);

        if (newAction.m_type == Action::TYPE_ERROR) {
          AbstractCoroutine* savedCP = _CP;
          _CP = _CP->m_parent;
          delete savedCP;
          if (newAction.m_data.error == action.m_data.error) {
            newAction.m_type = Action::TYPE_NONE;
          } else {
            action = std::move(newAction);
          }
          if(_CP == nullptr) {
            delete action.m_data.error;
            action.m_type = Action::TYPE_NONE;
            return std::forward<oatpp::async::Action>(action);
          }
        } else {
          action = std::move(newAction);
        }

        continue;
      }

      default:
        return std::forward<oatpp::async::Action>(action);

    };

//    action = iterate();
//    ++ iterations;

  }

  return std::forward<oatpp::async::Action>(action);

}

Action CoroutineHandle::iterate() {
  try {
    return _CP->call(_FP);
  } catch (std::exception& e) {
    return new Error(e.what());
  } catch (...) {
    return new Error("[oatpp::async::CoroutineHandle::iterate()]: Error. Unknown Exception.");
  }
}

Action CoroutineHandle::iterateAndTakeAction() {
  return takeAction(iterate());
}

bool CoroutineHandle::finished() const {
  return _CP == nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AbstractCoroutine

AbstractCoroutine::AbstractCoroutine()
  : m_parent(nullptr)
  , m_parentReturnAction(Action(Action::TYPE_NONE))
{}

Action AbstractCoroutine::handleError(Error* error) {
  return Action(error);
}

AbstractCoroutine* AbstractCoroutine::getParent() const {
  return m_parent;
}

Action AbstractCoroutine::repeat() {
  return Action::createActionByType(Action::TYPE_REPEAT);
}

Action AbstractCoroutine::waitRepeat(const std::chrono::duration<v_int64, std::micro>& timeout) {
  auto startTime = std::chrono::system_clock::now();
  auto end = startTime + timeout;
  std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch());
  return Action::createWaitRepeatAction(ms.count());
}

CoroutineStarter AbstractCoroutine::waitFor(const std::chrono::duration<v_int64, std::micro>& timeout) {

  class WaitingCoroutine : public Coroutine<WaitingCoroutine> {
  private:
    std::chrono::duration<v_int64, std::micro> m_duration;
    bool m_wait;
  public:

    WaitingCoroutine(const std::chrono::duration<v_int64, std::micro>& duration)
      : m_duration(duration)
      , m_wait(true)
    {}

    Action act() override {
      if(m_wait) {
        m_wait = false;
        return waitRepeat(m_duration);
      }
      return finish();
    }

  };

  return WaitingCoroutine::start(timeout);

}

Action AbstractCoroutine::ioWait(v_io_handle ioHandle, Action::IOEventType ioEventType) {
  return Action::createIOWaitAction(ioHandle, ioEventType);
}

Action AbstractCoroutine::ioRepeat(v_io_handle ioHandle, Action::IOEventType ioEventType) {
  return Action::createIORepeatAction(ioHandle, ioEventType);
}

Action AbstractCoroutine::error(Error* error) {
  return error;
}

}}
