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

#include "HttpRequestExecutor.hpp"

#include "../protocol/http/outgoing/Request.hpp"
#include "../protocol/http/outgoing/BufferBody.hpp"

#include "../../../../oatpp/network/src/Connection.hpp"
#include "../../../../oatpp/core/src/data/stream/ChunkedBuffer.hpp"
#include "../../../../oatpp/core/src/data/stream/StreamBufferedProxy.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

namespace oatpp { namespace web { namespace client {
  
std::shared_ptr<HttpRequestExecutor::Response>
HttpRequestExecutor::execute(const String::PtrWrapper& method,
                             const String::PtrWrapper& path,
                             const std::shared_ptr<Headers>& headers,
                             const std::shared_ptr<Body>& body) {
  
  //auto stream = oatpp::data::stream::ChunkedBuffer::createShared();
  //stream << "/" << path;
  //ENV::log("HTTP_EXECUTOR", "Execute: '%s'", (const char*) stream->toStringAsString()->getData());
  
  auto connection = m_connectionProvider->getConnection();
  
  if(!connection){
    return nullptr; // TODO ERROR HERE
  }
  
  auto request = oatpp::web::protocol::http::outgoing::Request::createShared(method, path, headers, body);
  request->headers->put(oatpp::web::protocol::http::Header::HOST, m_connectionProvider->getHost());
  
  auto ioBuffer = oatpp::data::buffer::IOBuffer::createShared();
  
  auto upStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(connection, ioBuffer);
  request->send(upStream);
  upStream->flush();
  
  //auto upStream = oatpp::data::stream::ChunkedBuffer::createShared();
  //request->send(upStream);
  //ENV::log("request", "request:'%s'\n", (const char*) upStream->toStringAsString()->getData());
  //upStream->flushToStream(connection);
  
  auto readCount = connection->read(ioBuffer->getData(), ioBuffer->getSize());
  
  //((p_char8) ioBuffer->getData())[readCount] = 0;
  //ENV::log("asd", "response='%s'", (const char*) ioBuffer->getData());
  
  if(readCount > 0) {
    
    oatpp::parser::ParsingCaret caret((p_char8) ioBuffer->getData(), ioBuffer->getSize());
    auto line = protocol::http::Protocol::parseResponseStartingLine(caret);
    if(!line){
      return nullptr; // TODO ERROR HERE
    }
    
    oatpp::web::protocol::http::Status error;
    auto headers = protocol::http::Protocol::parseHeaders(caret, error);
    
    if(error.code != 0){
      return nullptr; // TODO ERROR HERE
    }
    
    auto bodyStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(connection,
                                                                               ioBuffer,
                                                                               caret.getPosition(),
                                                                               (v_int32) readCount);
    
    return Response::createShared(line->statusCode, line->description, headers, bodyStream);
    
  }
  
  return nullptr;
  
}
  
}}}
