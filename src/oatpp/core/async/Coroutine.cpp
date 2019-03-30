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

Action Action::clone(const Action& action) {
  Action result(action.m_type);
  result.m_data = action.m_data;
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
  , m_data()
{}

Action::Action(Action&& other)
  : m_type(other.m_type)
  , m_data(other.m_data)
{
  other.m_data.fptr = nullptr;
}

Action::~Action() {
  if(m_type == TYPE_COROUTINE && m_data.coroutine != nullptr) {
    m_data.coroutine->free();
  }
}

Action& Action::operator=(Action&& other) {
  m_type = other.m_type;
  m_data = other.m_data;
  other.m_data.fptr = nullptr;
  return *this;
}

bool Action::isError() {
  return m_type == TYPE_ERROR;
}

v_int32 Action::getType() {
  return m_type;
}

std::shared_ptr<const Error> AbstractCoroutine::ERROR_UNKNOWN = std::make_shared<Error>("Unknown Error");

Action AbstractCoroutine::takeAction(Action&& action) {

  AbstractCoroutine* savedCP;

  switch (action.m_type) {

    case Action::TYPE_REPEAT: return std::forward<oatpp::async::Action>(action);
    case Action::TYPE_WAIT_RETRY: return std::forward<oatpp::async::Action>(action);

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
      savedCP->free();

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
            _CP->free();
            _CP = _CP->m_parent;
          }
        } else {
          action = takeAction(std::forward<oatpp::async::Action>(action));
        }
      } while (action.m_type == Action::TYPE_ERROR && _CP != nullptr);

      return std::forward<oatpp::async::Action>(action);

  };

  throw std::runtime_error("[oatpp::async::AbstractCoroutine::takeAction()]: Error. Unknown Action.");

}

}}
