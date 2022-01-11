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

#include "CommunicationError.hpp"

namespace oatpp { namespace web { namespace protocol {
  
CommunicationError::CommunicationError(oatpp::v_io_size ioStatus, const oatpp::String& message)
  :std::runtime_error(*message)
  , m_ioStatus(ioStatus)
  , m_message(message)
{}
  
oatpp::v_io_size CommunicationError::getIOStatus() {
  return m_ioStatus;
}

oatpp::String& CommunicationError::getMessage(){
  return m_message;
}
  
}}}
