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

#ifndef oatpp_web_server_AsyncHttpConnectionHandler_hpp
#define oatpp_web_server_AsyncHttpConnectionHandler_hpp

#include "./HttpProcessor.hpp"

#include "./handler/ErrorHandler.hpp"

#include "./HttpRouter.hpp"

#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"

#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/Connection.hpp"

#include "oatpp/core/concurrency/Thread.hpp"
#include "oatpp/core/concurrency/Runnable.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"
#include "oatpp/core/async/Processor.hpp"

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace web { namespace server {
  
class AsyncHttpConnectionHandler : public base::Controllable, public network::server::ConnectionHandler {
private:
  
  class Task : public base::Controllable, public concurrency::Runnable {
  public:
    typedef oatpp::collection::LinkedList<std::shared_ptr<HttpProcessor::ConnectionState>> Connections;
  private:
    void consumeConnections(oatpp::async::Processor& processor);
  private:
    oatpp::concurrency::SpinLock::Atom m_atom;
    Connections m_connections;
  private:
    bool m_isRunning;
    HttpRouter* m_router;
    std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder> m_bodyDecoder;
    std::shared_ptr<handler::ErrorHandler> m_errorHandler;
    HttpProcessor::RequestInterceptors* m_requestInterceptors;
    std::mutex m_taskMutex;
    std::condition_variable m_taskCondition;
  public:
    Task(HttpRouter* router,
         const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder,
         const std::shared_ptr<handler::ErrorHandler>& errorHandler,
         HttpProcessor::RequestInterceptors* requestInterceptors)
      : m_atom(false)
      , m_isRunning(true)
      , m_router(router)
      , m_bodyDecoder(bodyDecoder)
      , m_errorHandler(errorHandler)
      , m_requestInterceptors(requestInterceptors)
    {}
  public:
    
    static std::shared_ptr<Task> createShared(HttpRouter* router,
                                              const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder,
                                              const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                                              HttpProcessor::RequestInterceptors* requestInterceptors){
      return std::make_shared<Task>(router, bodyDecoder, errorHandler, requestInterceptors);
    }
    
    void run() override;
    
    void addConnection(const std::shared_ptr<HttpProcessor::ConnectionState>& connectionState){
      oatpp::concurrency::SpinLock lock(m_atom);
      m_connections.pushBack(connectionState);
      m_taskCondition.notify_one();
    }
    
    void stop() {
      m_isRunning = false;
    }
    
  };
  
private:
  std::shared_ptr<HttpRouter> m_router;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  HttpProcessor::RequestInterceptors m_requestInterceptors;
  std::atomic<v_int32> m_taskBalancer;
  v_int32 m_threadCount;
  std::shared_ptr<Task>* m_tasks;
public:
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router,
                             v_int32 threadCount = OATPP_ASYNC_HTTP_CONNECTION_HANDLER_THREAD_NUM_DEFAULT)
    : m_router(router)
    , m_errorHandler(handler::DefaultErrorHandler::createShared())
    , m_taskBalancer(0)
    , m_threadCount(threadCount)
  {
    
    // TODO make bodyDecoder configurable here
    std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder> bodyDecoder =
    std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>();
    
    m_tasks = new std::shared_ptr<Task>[m_threadCount];
    for(v_int32 i = 0; i < m_threadCount; i++) {
      auto task = Task::createShared(m_router.get(), bodyDecoder, m_errorHandler, &m_requestInterceptors);
      m_tasks[i] = task;
      concurrency::Thread thread(task);
      thread.detach();
    }
  }
public:
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router){
    return std::make_shared<AsyncHttpConnectionHandler>(router);
  }
  
  ~AsyncHttpConnectionHandler(){
    delete [] m_tasks;
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
  
  void stop() {
    for(v_int32 i = 0; i < m_threadCount; i ++) {
      m_tasks[i]->stop();
    }
  }
  
};
  
}}}

#endif /* oatpp_web_server_AsyncHttpConnectionHandler_hpp */

