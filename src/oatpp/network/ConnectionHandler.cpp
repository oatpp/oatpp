/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bamkrs@github.com>
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

#include "ConnectionHandler.hpp"


namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionError

ConnectionError::ConnectionError(oatpp::v_io_size ioStatus, const String &message)
    : std::runtime_error(message->std_str())
    , m_ioStatus(ioStatus)
    , m_message(message) {}

oatpp::v_io_size ConnectionError::getIOStatus() {
  return m_ioStatus;
}

oatpp::String &ConnectionError::getMessage() {
  return m_message;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FatalConnectionError

FatalConnectionError::FatalConnectionError(oatpp::v_io_size ioStatus, const String &message)
    : ConnectionError(ioStatus, message) {}

}}
