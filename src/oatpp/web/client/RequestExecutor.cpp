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

#include "RequestExecutor.hpp"

namespace oatpp { namespace web { namespace client {

RequestExecutor::RequestExecutionError::RequestExecutionError(v_int32 errorCode, const char* message, v_int32 readErrorCode)
  : std::runtime_error(message)
  , m_errorCode(errorCode)
  , m_message(message)
  , m_readErrorCode(readErrorCode)
{}

v_int32 RequestExecutor::RequestExecutionError::getErrorCode() const {
  return m_errorCode;
}

const char* RequestExecutor::RequestExecutionError::getMessage() const {
  return m_message;
}

v_int32 RequestExecutor::RequestExecutionError::getReadErrorCode() const {
  return m_readErrorCode;
}

}}}