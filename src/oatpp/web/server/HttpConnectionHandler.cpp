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

#include "./HttpConnectionHandler.hpp"

#include "oatpp/web/protocol/http/outgoing/ChunkedBufferBody.hpp"
#include "oatpp/web/protocol/http/outgoing/CommunicationUtils.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/core/concurrency/Thread.hpp"

namespace oatpp { namespace web { namespace server {

HttpConnectionHandler::Task::Task(HttpRouter* router,
                                  const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                  const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder,
                                  const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                                  HttpProcessor::RequestInterceptors* requestInterceptors)
  : m_router(router)
  , m_connection(connection)
  , m_bodyDecoder(bodyDecoder)
  , m_errorHandler(errorHandler)
  , m_requestInterceptors(requestInterceptors)
{}

std::shared_ptr<HttpConnectionHandler::Task>
HttpConnectionHandler::Task::createShared(HttpRouter* router,
                                          const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                          const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder,
                                          const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                                          HttpProcessor::RequestInterceptors* requestInterceptors) {
  return std::make_shared<Task>(router, connection, bodyDecoder, errorHandler, requestInterceptors);
}

void HttpConnectionHandler::Task::run(){
  
  v_int32 bufferSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
  v_char8 buffer [bufferSize];
  
  auto outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  auto inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  
  v_int32 connectionState = oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_CLOSE;
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response;
  do {
  
    response = HttpProcessor::processRequest(m_router, m_connection, m_bodyDecoder, m_errorHandler, m_requestInterceptors, buffer, bufferSize, inStream, connectionState);
    
    if(response) {
      outStream->setBufferPosition(0, 0, false);
      response->send(outStream.get());
      outStream->flush();
    } else {
      return;
    }
    
  } while(connectionState == oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_KEEP_ALIVE);
  
  if(connectionState == oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_UPGRADE) {
    auto handler = response->getConnectionUpgradeHandler();
    if(handler) {
      handler->handleConnection(m_connection, response->getConnectionUpgradeParameters());
    } else {
      OATPP_LOGD("[oatpp::web::server::HttpConnectionHandler::Task::run()]", "Warning. ConnectionUpgradeHandler not set!");
    }
  }
  
}

HttpConnectionHandler::HttpConnectionHandler(const std::shared_ptr<HttpRouter>& router)
  : m_router(router)
  , m_bodyDecoder(std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>())
  , m_errorHandler(handler::DefaultErrorHandler::createShared())
{}

std::shared_ptr<HttpConnectionHandler> HttpConnectionHandler::createShared(const std::shared_ptr<HttpRouter>& router){
  return std::make_shared<HttpConnectionHandler>(router);
}

void HttpConnectionHandler::setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
  m_errorHandler = errorHandler;
  if(!m_errorHandler) {
    m_errorHandler = handler::DefaultErrorHandler::createShared();
  }
}

void HttpConnectionHandler::addRequestInterceptor(const std::shared_ptr<handler::RequestInterceptor>& interceptor) {
  m_requestInterceptors.pushBack(interceptor);
}
  
void HttpConnectionHandler::handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                             const std::shared_ptr<const ParameterMap>& params)
{

  connection->setOutputStreamIOMode(oatpp::data::stream::IOMode::BLOCKING);
  connection->setInputStreamIOMode(oatpp::data::stream::IOMode::BLOCKING);

  /* Create working thread */
  std::thread thread(&Task::run, Task(m_router.get(), connection, m_bodyDecoder, m_errorHandler, &m_requestInterceptors));
  
  /* Get hardware concurrency -1 in order to have 1cpu free of workers. */
  v_int32 concurrency = oatpp::concurrency::getHardwareConcurrency();
  if(concurrency > 1) {
    concurrency -= 1;
  }
  
  /* Set thread affinity group CPUs [0..cpu_count - 1]. Leave one cpu free of workers */
  oatpp::concurrency::setThreadAffinityToCpuRange(thread.native_handle(), 0, concurrency - 1 /* -1 because 0-based index */);
  
  thread.detach();
}

void HttpConnectionHandler::stop() {
  // DO NOTHING
}

}}}
