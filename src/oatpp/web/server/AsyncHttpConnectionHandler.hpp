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

#include "oatpp/web/server/HttpProcessor.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/server/handler/Interceptor.hpp"

#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"

#include "oatpp/network/server/ConnectionHandler.hpp"
#include "oatpp/core/async/Executor.hpp"

namespace oatpp { namespace web { namespace server {

/**
 * Asynchronous &id:oatpp::network::server::ConnectionHandler; for handling http communication.
 */
class AsyncHttpConnectionHandler : public base::Countable, public network::server::ConnectionHandler {
private:
  typedef oatpp::web::protocol::http::incoming::BodyDecoder BodyDecoder;
public:
  static const v_int32 THREAD_NUM_DEFAULT;
private:
  std::shared_ptr<oatpp::async::Executor> m_executor;
private:
  std::shared_ptr<HttpRouter> m_router;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  HttpProcessor::RequestInterceptors m_requestInterceptors;
  std::shared_ptr<const BodyDecoder> m_bodyDecoder; // TODO make bodyDecoder configurable here
public:
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router, v_int32 threadCount = THREAD_NUM_DEFAULT);
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router, const std::shared_ptr<oatpp::async::Executor>& executor);
public:
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  v_int32 threadCount = THREAD_NUM_DEFAULT);
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  const std::shared_ptr<oatpp::async::Executor>& executor);
  
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler);
  
  void addRequestInterceptor(const std::shared_ptr<handler::RequestInterceptor>& interceptor);
  
  void handleConnection(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<const ParameterMap>& params) override;

  /**
   * Will call m_executor.stop()
   */
  void stop() override;
  
};
  
}}}

#endif /* oatpp_web_server_AsyncHttpConnectionHandler_hpp */

