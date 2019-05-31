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

Action Action::createIOWaitAction(data::v_io_handle ioHandle, Action::IOEventType ioEventType) {
  Action result(TYPE_IO_WAIT);
  result.m_data.ioData.ioHandle = ioHandle;
  result.m_data.ioData.ioEventType = ioEventType;
  return result;
}

Action Action::createIORepeatAction(data::v_io_handle ioHandle, Action::IOEventType ioEventType) {
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

Action::Action(AbstractCoroutine* coroutine)
  : m_type(TYPE_COROUTINE)
{
  m_data.coroutine = coroutine;
}

Action::Action(FunctionPtr functionPtr)
  : m_type(TYPE_YIELD_TO)
{
  m_data.fptr = functionPtr;
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
  if(m_type == TYPE_COROUTINE) {
    delete m_data.coroutine;
  }
}

Action& Action::operator=(Action&& other) {
  m_type = other.m_type;
  m_data = other.m_data;
  other.m_data.fptr = nullptr;
  return *this;
}

bool Action::isError() const {
  return m_type == TYPE_ERROR;
}

v_int32 Action::getType() const {
  return m_type;
}

v_int64 Action::getTimePointMicroseconds() const {
  return m_data.timePointMicroseconds;
}

oatpp::data::v_io_handle Action::getIOHandle() const {
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
  if(m_first != nullptr) {
    auto curr = m_first;
    while(curr != nullptr) {
      AbstractCoroutine* next = nullptr;
      if(curr->m_parentReturnAction.m_type == Action::TYPE_COROUTINE) {
        next = curr->m_parentReturnAction.m_data.coroutine;
      }
      delete curr;
      curr = next;
    }
  }
}

/*
 * Move assignment operator.
 */
CoroutineStarter& CoroutineStarter::operator=(CoroutineStarter&& other) {
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
  m_last->m_parentReturnAction = starter.m_first;
  m_last = starter.m_last;
  starter.m_first = nullptr;
  starter.m_last = nullptr;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AbstractCoroutine

std::shared_ptr<const Error> AbstractCoroutine::ERROR_UNKNOWN = std::make_shared<Error>("Unknown Error");

AbstractCoroutine::AbstractCoroutine()
  : _CP(this)
  , _FP(&AbstractCoroutine::act)
  , _ERR(nullptr)
  , _PP(nullptr)
  , _SCH_A(Action::TYPE_NONE)
  , _ref(nullptr)
  , m_parent(nullptr)
  , m_propagatedError(&_ERR)
  , m_parentReturnAction(Action(Action::TYPE_FINISH))
{}

Action AbstractCoroutine::iterate() {
  try {
    return _CP->call(_FP);
  } catch (std::exception& e) {
    *m_propagatedError = std::make_shared<Error>(e.what());
    return Action::TYPE_ERROR;
  } catch (...) {
    *m_propagatedError = ERROR_UNKNOWN;
    return Action::TYPE_ERROR;
  }
};

Action AbstractCoroutine::takeAction(Action&& action) {

  AbstractCoroutine* savedCP;

  switch (action.m_type) {

    case Action::TYPE_COROUTINE:
      action.m_data.coroutine->m_parent = _CP;
      action.m_data.coroutine->m_propagatedError = m_propagatedError;
      _CP = action.m_data.coroutine;
      _FP = action.m_data.coroutine->_FP;

      return std::forward<oatpp::async::Action>(action);

    case Action::TYPE_FINISH:
      if(_CP == this) {
        _CP = nullptr;
        return std::forward<oatpp::async::Action>(action);
      }

      savedCP = _CP;
      _CP = _CP->m_parent;
      _FP = nullptr;
      /* Please note that savedCP->m_parentReturnAction should not be "REPEAT nor WAIT_RETRY" */
      /* as funtion pointer (FP) is invalidated */
      action = takeAction(std::move(savedCP->m_parentReturnAction));
      delete savedCP;

      return std::forward<oatpp::async::Action>(action);

    case Action::TYPE_YIELD_TO:
      _FP = action.m_data.fptr;
      return std::forward<oatpp::async::Action>(action);

    case Action::TYPE_ERROR:
      do {
        action = _CP->handleError(*m_propagatedError);
        if(action.m_type == Action::TYPE_ERROR) {
          if(_CP == this) {
            _CP = nullptr;
            return std::forward<oatpp::async::Action>(action);
          } else {
            savedCP = _CP;
            _CP = _CP->m_parent;
            delete savedCP;
          }
        } else {
          action = takeAction(std::forward<oatpp::async::Action>(action));
        }
      } while (action.m_type == Action::TYPE_ERROR && _CP != nullptr);

      return std::forward<oatpp::async::Action>(action);

  };

  //throw std::runtime_error("[oatpp::async::AbstractCoroutine::takeAction()]: Error. Unknown Action.");
  return std::forward<oatpp::async::Action>(action);

}

Action AbstractCoroutine::handleError(const std::shared_ptr<const Error>& error) {
  return Action::TYPE_ERROR;
}

bool AbstractCoroutine::finished() const {
  return _CP == nullptr;
}

/**
 * Get parent coroutine
 * @return - pointer to a parent coroutine
 */
AbstractCoroutine* AbstractCoroutine::getParent() const {
  return m_parent;
}

Action AbstractCoroutine::error(const std::shared_ptr<const Error>& error) {
  *m_propagatedError = error;
  return Action::TYPE_ERROR;
}

}}
