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

#ifndef oatpp_web_server_AsyncHttpConnectionHandler_hpp
#define oatpp_web_server_AsyncHttpConnectionHandler_hpp

#include "./HttpProcessor.hpp"
#include "./handler/ErrorHandler.hpp"
#include "./HttpRouter.hpp"

#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"

#include "oatpp/network/server/ConnectionHandler.hpp"
#include "oatpp/network/Connection.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"
#include "oatpp/core/async/Executor.hpp"

namespace oatpp { namespace web { namespace server {
  
class AsyncHttpConnectionHandler : public base::Controllable, public network::server::ConnectionHandler {
private:
  typedef oatpp::web::protocol::http::incoming::BodyDecoder BodyDecoder;
private:
  std::shared_ptr<oatpp::async::Executor> m_executor;
private:
  std::shared_ptr<HttpRouter> m_router;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  HttpProcessor::RequestInterceptors m_requestInterceptors;
  std::shared_ptr<const BodyDecoder> m_bodyDecoder; // TODO make bodyDecoder configurable here
public:
  
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             v_int32 threadCount = OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT)
    : m_executor(std::make_shared<oatpp::async::Executor>(threadCount))
    , m_router(router)
    , m_errorHandler(handler::DefaultErrorHandler::createShared())
    , m_bodyDecoder(std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>())
  {
    m_executor->detach();
  }
  
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             const std::shared_ptr<oatpp::async::Executor>& executor)
    : m_executor(executor)
    , m_router(router)
    , m_errorHandler(handler::DefaultErrorHandler::createShared())
    , m_bodyDecoder(std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>())
  {}
public:
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  v_int32 threadCount = OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT){
    return std::make_shared<AsyncHttpConnectionHandler>(router, threadCount);
  }
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  const std::shared_ptr<oatpp::async::Executor>& executor){
    return std::make_shared<AsyncHttpConnectionHandler>(router, executor);
  }
  
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
    m_errorHandler = errorHandler;
    if(!m_errorHandler) {
      m_errorHandler = handler::DefaultErrorHandler::createShared();
    }
  }
  
  void addRequestInterceptor(const std::shared_ptr<handler::RequestInterceptor>& interceptor) {
    m_requestInterceptors.pushBack(interceptor);
  }
  
  void handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection) override;
  
  void stop() override {
    m_executor->stop();
  }
  
};
  
}}}

#endif /* oatpp_web_server_AsyncHttpConnectionHandler_hpp */

