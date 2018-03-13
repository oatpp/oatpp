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

#include "./HttpConnectionHandler.hpp"

#include "../protocol/http/outgoing/ChunkedBufferBody.hpp"

#include "../protocol/http/incoming/Request.hpp"
#include "../protocol/http/Http.hpp"

#include "./HttpError.hpp"

#include "../../../../oatpp-lib/core/test/Checker.hpp"

#include <errno.h>

namespace oatpp { namespace web { namespace server {
  
bool HttpConnectionHandler::considerConnectionKeepAlive(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                                        const std::shared_ptr<protocol::http::outgoing::Response>& response){
  
  auto& inKeepAlive = request->headers->get(protocol::http::Header::CONNECTION, nullptr);
  
  if(!inKeepAlive.isNull() && base::String::equalsCI_FAST(inKeepAlive, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE)) {
    if(response->headers->putIfNotExists(protocol::http::Header::CONNECTION, inKeepAlive)){
      return true;
    } else {
      auto& outKeepAlive = response->headers->get(protocol::http::Header::CONNECTION, nullptr);
      return (!outKeepAlive.isNull() && base::String::equalsCI_FAST(outKeepAlive, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE));
    }
  } else if(!response->headers->putIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE)) {
    auto& outKeepAlive = response->headers->get(protocol::http::Header::CONNECTION, nullptr);
    return (!outKeepAlive.isNull() && base::String::equalsCI_FAST(outKeepAlive, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE));
  }
  
  return false;
  
}
  
std::shared_ptr<protocol::http::outgoing::Response>
HttpConnectionHandler::Task::handleError(const protocol::http::Status& status, const base::String::SharedWrapper& message){
  return m_errorHandler->handleError(status, message);
}
  
std::shared_ptr<protocol::http::outgoing::Response>
HttpConnectionHandler::Task::processRequest(p_char8 buffer,
                                            v_int32 bufferSize,
                                            const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream,
                                            bool& keepAlive) {
  
  keepAlive = false;
  auto readCount = m_connection->read(buffer, bufferSize);
  if(readCount > 0) {
    
    oatpp::parser::ParsingCaret caret(buffer, bufferSize);
    auto line = protocol::http::Protocol::parseRequestStartingLine(caret);
    
    if(!line){
      return handleError(protocol::http::Status::CODE_400, "Can't read starting line");
    }
    
    auto route = m_router->getRoute(line->method, line->path);
    
    if(!route.isNull()) {
      
      oatpp::web::protocol::http::Status error;
      auto headers = protocol::http::Protocol::parseHeaders(caret, error);
      
      if(error.code != 0){
        return handleError(error, " Can't parse headers");
      }
      
      auto bodyStream = inStream;
      bodyStream->setBufferPosition(caret.getPosition(), (v_int32) readCount);
      
      auto request = protocol::http::incoming::Request::createShared(line, route.matchMap, headers, bodyStream);
      std::shared_ptr<protocol::http::outgoing::Response> response;
      try{
        response = route.processUrl(request);
      } catch (HttpError& error) {
        return handleError(error.getStatus(), error.getMessage());
      } catch (std::exception& error) {
        return handleError(protocol::http::Status::CODE_500, error.what());
      } catch (...) {
        return handleError(protocol::http::Status::CODE_500, "Unknown error");
      }
      
      response->headers->putIfNotExists(protocol::http::Header::SERVER,
                                        protocol::http::Header::Value::SERVER);
      
      keepAlive = considerConnectionKeepAlive(request, response);
      return response;
      
    } else {
      return handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
    }
    
  } else {
    return nullptr;
  }
  
}
  
void HttpConnectionHandler::Task::run(){
  
  //oatpp::test::PerformanceChecker checker("task checker");
  
  v_int32 bufferSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
  v_char8 buffer [bufferSize];
  auto outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  auto inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  
  bool keepAlive = true;
  
  do {
    
    auto response = processRequest(buffer, bufferSize, inStream, keepAlive);
    if(response) {
      outStream->setBufferPosition(0, 0);
      response->send(outStream);
      outStream->flush();
    } else {
      return;
    }
    
  } while(keepAlive);
  
}
  
void HttpConnectionHandler::handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection){
  concurrency::Thread thread(Task::createShared(m_router.get(), connection, m_errorHandler));
  thread.detach();
}

}}}
