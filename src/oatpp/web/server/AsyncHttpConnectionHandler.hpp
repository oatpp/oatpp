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
#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/async/Executor.hpp"
#include "oatpp/concurrency/SpinLock.hpp"

#include <unordered_map>

namespace oatpp { namespace web { namespace server {

/**
 * Asynchronous &id:oatpp::network::ConnectionHandler; for handling http communication.
 */
class AsyncHttpConnectionHandler : public base::Countable, public network::ConnectionHandler, public HttpProcessor::TaskProcessingListener {
protected:

  void onTaskStart(const provider::ResourceHandle<data::stream::IOStream>& connection) override;
  void onTaskEnd(const provider::ResourceHandle<data::stream::IOStream>& connection) override;

  void invalidateAllConnections();

private:
  std::shared_ptr<oatpp::async::Executor> m_executor;
  std::shared_ptr<HttpProcessor::Components> m_components;
  std::atomic_bool m_continue;
  std::unordered_map<v_uint64, provider::ResourceHandle<data::stream::IOStream>> m_connections;
  oatpp::concurrency::SpinLock m_connectionsLock;
public:

  /**
   * Constructor.
   * @param components - &id:oatpp::web::server::HttpProcessor::Components;.
   * @param threadCount - number of threads.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components, v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED);

  /**
   * Constructor.
   * @param components - &id:oatpp::web::server::HttpProcessor::Components;.
   * @param executor - &id:oatpp::async::Executor;.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components, const std::shared_ptr<oatpp::async::Executor>& executor);

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param threadCount - number of threads.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router, v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router), threadCount)
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param executor - &id:oatpp::async::Executor;.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router, const std::shared_ptr<oatpp::async::Executor>& executor)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router), executor)
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param config - &id:oatpp::web::server::HttpProcessor::Config;.
   * @param threadCount - number of threads.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             const std::shared_ptr<HttpProcessor::Config>& config,
                             v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router, config), threadCount)
  {}

  /**
   * Constructor.
   * @param router - &id:oatpp::web::server::HttpRouter; to route incoming requests.
   * @param config - &id:oatpp::web::server::HttpProcessor::Config;.
   * @param executor - &id:oatpp::async::Executor;.
   */
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             const std::shared_ptr<HttpProcessor::Config>& config,
                             const std::shared_ptr<oatpp::async::Executor>& executor)
    : AsyncHttpConnectionHandler(std::make_shared<HttpProcessor::Components>(router, config), executor)
  {}

public:
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED);
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router,
                                                                  const std::shared_ptr<oatpp::async::Executor>& executor);
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpProcessor::Components>& components,
                                                                  const std::shared_ptr<oatpp::async::Executor>& executor);

  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpProcessor::Components>& components,
                                                                  v_int32 threadCount = oatpp::async::Executor::VALUE_SUGGESTED);

  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler);

  /**
   * Add request interceptor. Request interceptors are called before routing happens.
   * If multiple interceptors set then the order of interception is the same as the order of calls to `addRequestInterceptor`.
   * @param interceptor - &id:oatpp::web::server::interceptor::RequestInterceptor;.
   */
  void addRequestInterceptor(const std::shared_ptr<interceptor::RequestInterceptor>& interceptor);

  /**
   * Add response interceptor.
   * If multiple interceptors set then the order of interception is the same as the order of calls to `addResponseInterceptor`.
   * @param interceptor - &id:oatpp::web::server::interceptor::RequestInterceptor;.
   */
  void addResponseInterceptor(const std::shared_ptr<interceptor::ResponseInterceptor>& interceptor);

  
  void handleConnection(const provider::ResourceHandle<IOStream>& connection,
                        const std::shared_ptr<const ParameterMap>& params) override;

  /**
   * Will call m_executor.stop()
   */
  void stop() override;

  /**
   * Get connections count.
   * @return
   */
  v_uint64 getConnectionsCount();
  
};
  
}}}

#endif /* oatpp_web_server_AsyncHttpConnectionHandler_hpp */

