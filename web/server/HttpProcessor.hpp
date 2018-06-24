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

#ifndef oatpp_web_server_HttpProcessor_hpp
#define oatpp_web_server_HttpProcessor_hpp

#include "./HttpRouter.hpp"

#include "./handler/Interceptor.hpp"
#include "./handler/ErrorHandler.hpp"

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/async/Processor.hpp"

namespace oatpp { namespace web { namespace server {
  
class HttpProcessor {
public:
  static const char* RETURN_KEEP_ALIVE;
public:
  typedef oatpp::collection::LinkedList<std::shared_ptr<oatpp::web::server::handler::RequestInterceptor>> RequestInterceptors;
public:
  
  class ConnectionState {
  public:
    SHARED_OBJECT_POOL(ConnectionState_Pool, ConnectionState, 32)
  public:
    
    static std::shared_ptr<ConnectionState> createShared(){
      return ConnectionState_Pool::allocateShared();
    }
    
    std::shared_ptr<oatpp::data::stream::IOStream> connection;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> ioBuffer;
    std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy> outStream;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> inStream;
    
  };
  
public:
  
  class Coroutine : public oatpp::async::Coroutine<HttpProcessor::Coroutine> {
  private:
    Action parseRequest(v_int32 readCount);
  private:
    HttpRouter* m_router;
    std::shared_ptr<handler::ErrorHandler> m_errorHandler;
    RequestInterceptors* m_requestInterceptors;
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> m_ioBuffer;
    std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy> m_outStream;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> m_inStream;
    bool m_keepAlive;
  private:
    oatpp::web::server::HttpRouter::BranchRouter::Route m_currentRoute;
    std::shared_ptr<protocol::http::incoming::Request> m_currentRequest;
    std::shared_ptr<protocol::http::outgoing::Response> m_currentResponse;
  public:
    
    Coroutine(HttpRouter* router,
              const std::shared_ptr<handler::ErrorHandler>& errorHandler,
              RequestInterceptors* requestInterceptors,
              const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
              const std::shared_ptr<oatpp::data::buffer::IOBuffer>& ioBuffer,
              const std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy>& outStream,
              const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream)
      : m_router(router)
      , m_errorHandler(errorHandler)
      , m_requestInterceptors(requestInterceptors)
      , m_connection(connection)
      , m_ioBuffer(ioBuffer)
      , m_outStream(outStream)
      , m_inStream(inStream)
      , m_keepAlive(true)
    {}
    
    Action act() override;
    
    Action onRequestFormed();
    Action onResponse(const std::shared_ptr<protocol::http::outgoing::Response>& response);
    Action onResponseFormed();
    Action onRequestDone();
    
    Action handleError(const oatpp::async::Error& error) override;
    
  };
  
public:
  static bool considerConnectionKeepAlive(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                          const std::shared_ptr<protocol::http::outgoing::Response>& response);
  
  static std::shared_ptr<protocol::http::outgoing::Response>
  processRequest(HttpRouter* router,
                 const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                 const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                 RequestInterceptors* requestInterceptors,
                 void* buffer,
                 v_int32 bufferSize,
                 const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream,
                 bool& keepAlive);
  
};
  
}}}

#endif /* oatpp_web_server_HttpProcessor_hpp */
