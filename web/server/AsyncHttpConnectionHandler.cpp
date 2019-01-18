/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "./AsyncHttpConnectionHandler.hpp"

#include "oatpp/web/protocol/http/outgoing/ChunkedBufferBody.hpp"

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/network/Connection.hpp"
#include "oatpp/test/Checker.hpp"

#include <errno.h>

namespace oatpp { namespace web { namespace server {

void AsyncHttpConnectionHandler::handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection){
  
  auto ioBuffer = oatpp::data::buffer::IOBuffer::createShared();
  auto outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(connection, ioBuffer);
  auto inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(connection, ioBuffer);
  
  m_executor->execute<HttpProcessor::Coroutine>(m_router.get(),
                                                m_bodyDecoder,
                                                m_errorHandler,
                                                &m_requestInterceptors,
                                                connection,
                                                ioBuffer,
                                                outStream,
                                                inStream);
  
}
  
}}}

