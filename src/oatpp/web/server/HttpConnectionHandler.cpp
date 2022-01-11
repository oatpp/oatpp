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

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/core/concurrency/Thread.hpp"

#include "oatpp/core/data/buffer/IOBuffer.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"


namespace oatpp { namespace web { namespace server {

void HttpConnectionHandler::onTaskStart(const provider::ResourceHandle<data::stream::IOStream>& connection) {

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_connectionsLock);
  m_connections.insert({(v_uint64) connection.object.get(), connection});

  if(!m_continue.load()) {
    connection.invalidator->invalidate(connection.object);
  }

}

void HttpConnectionHandler::onTaskEnd(const provider::ResourceHandle<data::stream::IOStream>& connection) {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_connectionsLock);
  m_connections.erase((v_uint64) connection.object.get());
}

void HttpConnectionHandler::invalidateAllConnections() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_connectionsLock);
  for(auto& c : m_connections) {
    const auto& handle = c.second;
    handle.invalidator->invalidate(handle.object);
  }
}

v_uint64 HttpConnectionHandler::getConnectionsCount() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_connectionsLock);
  return m_connections.size();
}

HttpConnectionHandler::HttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components)
  : m_components(components)
  , m_continue(true)
{}

std::shared_ptr<HttpConnectionHandler> HttpConnectionHandler::createShared(const std::shared_ptr<HttpRouter>& router){
  return std::make_shared<HttpConnectionHandler>(router);
}

void HttpConnectionHandler::setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
  m_components->errorHandler = errorHandler;
  if(!m_components->errorHandler) {
    m_components->errorHandler = handler::DefaultErrorHandler::createShared();
  }
}

void HttpConnectionHandler::addRequestInterceptor(const std::shared_ptr<interceptor::RequestInterceptor>& interceptor) {
  m_components->requestInterceptors.push_back(interceptor);
}

void HttpConnectionHandler::addResponseInterceptor(const std::shared_ptr<interceptor::ResponseInterceptor>& interceptor) {
  m_components->responseInterceptors.push_back(interceptor);
}
  
void HttpConnectionHandler::handleConnection(const provider::ResourceHandle<data::stream::IOStream>& connection,
                                             const std::shared_ptr<const ParameterMap>& params)
{

  (void)params;

  if (m_continue.load()) {

    connection.object->setOutputStreamIOMode(oatpp::data::stream::IOMode::BLOCKING);
    connection.object->setInputStreamIOMode(oatpp::data::stream::IOMode::BLOCKING);

    /* Create working thread */
    std::thread thread(&HttpProcessor::Task::run, std::move(HttpProcessor::Task(m_components, connection, this)));

    /* Get hardware concurrency -1 in order to have 1cpu free of workers. */
    v_int32 concurrency = oatpp::concurrency::getHardwareConcurrency();
    if (concurrency > 1) {
      concurrency -= 1;
    }

    /* Set thread affinity group CPUs [0..cpu_count - 1]. Leave one cpu free of workers */
    oatpp::concurrency::setThreadAffinityToCpuRange(thread.native_handle(),
                                                    0,
                                                    concurrency - 1 /* -1 because 0-based index */);

    thread.detach();
  }

}

void HttpConnectionHandler::stop() {
  m_continue.store(false);

  /* invalidate all connections */
  invalidateAllConnections();

  /* Wait until all connection-threads are done */
  while(getConnectionsCount() > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

}}}
