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

#include "./AsyncHttpConnectionHandler.hpp"

namespace oatpp { namespace web { namespace server {

const v_int32 AsyncHttpConnectionHandler::THREAD_NUM_DEFAULT = OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT;

AsyncHttpConnectionHandler::AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                                                       v_int32 threadCount)
  : m_executor(std::make_shared<oatpp::async::Executor>(threadCount))
  , m_router(router)
  , m_errorHandler(handler::DefaultErrorHandler::createShared())
  , m_bodyDecoder(std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>())
{
  m_executor->detach();
}

AsyncHttpConnectionHandler::AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                                                       const std::shared_ptr<oatpp::async::Executor>& executor)
  : m_executor(executor)
  , m_router(router)
  , m_errorHandler(handler::DefaultErrorHandler::createShared())
  , m_bodyDecoder(std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>())
{}

std::shared_ptr<AsyncHttpConnectionHandler> AsyncHttpConnectionHandler::createShared(const std::shared_ptr<HttpRouter>& router, v_int32 threadCount){
  return std::make_shared<AsyncHttpConnectionHandler>(router, threadCount);
}

std::shared_ptr<AsyncHttpConnectionHandler> AsyncHttpConnectionHandler::createShared(const std::shared_ptr<HttpRouter>& router, const std::shared_ptr<oatpp::async::Executor>& executor){
  return std::make_shared<AsyncHttpConnectionHandler>(router, executor);
}

void AsyncHttpConnectionHandler::setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
  m_errorHandler = errorHandler;
  if(!m_errorHandler) {
    m_errorHandler = handler::DefaultErrorHandler::createShared();
  }
}

void AsyncHttpConnectionHandler::addRequestInterceptor(const std::shared_ptr<handler::RequestInterceptor>& interceptor) {
  m_requestInterceptors.pushBack(interceptor);
}

void AsyncHttpConnectionHandler::handleConnection(const std::shared_ptr<IOStream>& connection,
                                                  const std::shared_ptr<const ParameterMap>& params)
{

  connection->setOutputStreamIOMode(oatpp::data::stream::IOMode::NON_BLOCKING);
  connection->setInputStreamIOMode(oatpp::data::stream::IOMode::NON_BLOCKING);
  
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

void AsyncHttpConnectionHandler::stop() {
  m_executor->stop();
}
  
}}}

