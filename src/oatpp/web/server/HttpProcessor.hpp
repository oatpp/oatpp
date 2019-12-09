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

#ifndef oatpp_web_server_HttpProcessor_hpp
#define oatpp_web_server_HttpProcessor_hpp

#include "./HttpRouter.hpp"

#include "./handler/Interceptor.hpp"
#include "./handler/ErrorHandler.hpp"

#include "oatpp/web/protocol/http/incoming/RequestHeadersReader.hpp"

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/CommunicationUtils.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/async/Processor.hpp"

namespace oatpp { namespace web { namespace server {
  
class HttpProcessor {
public:
  typedef oatpp::collection::LinkedList<std::shared_ptr<oatpp::web::server::handler::RequestInterceptor>> RequestInterceptors;
  typedef oatpp::web::protocol::http::incoming::RequestHeadersReader RequestHeadersReader;

public:

  struct Components {

    Components(const std::shared_ptr<HttpRouter>& pRouter,
               const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& pBodyDecoder,
               const std::shared_ptr<handler::ErrorHandler>& pErrorHandler,
               const std::shared_ptr<HttpProcessor::RequestInterceptors>& pRequestInterceptors)
      : router(pRouter)
      , bodyDecoder(pBodyDecoder)
      , errorHandler(pErrorHandler)
      , requestInterceptors(pRequestInterceptors)
    {}

    std::shared_ptr<HttpRouter> router;
    std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder> bodyDecoder;
    std::shared_ptr<handler::ErrorHandler> errorHandler;
    std::shared_ptr<HttpProcessor::RequestInterceptors> requestInterceptors;

  };

private:

  static std::shared_ptr<protocol::http::outgoing::Response>
  processRequest(const std::shared_ptr<Components>& components,
                 RequestHeadersReader& headersReader,
                 const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream,
                 v_int32& connectionState);

public:

  class Task : public base::Countable {
  private:
    std::shared_ptr<Components> m_components;
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
  public:
    Task(const std::shared_ptr<Components>& components,
         const std::shared_ptr<oatpp::data::stream::IOStream>& connection);
  public:
    void run();
  };
  
public:
  
  class Coroutine : public oatpp::async::Coroutine<HttpProcessor::Coroutine> {
  private:
    std::shared_ptr<Components> m_components;
    oatpp::data::stream::BufferOutputStream m_headersInBuffer;
    RequestHeadersReader m_headersReader;
    std::shared_ptr<oatpp::data::stream::BufferOutputStream> m_headersOutBuffer;
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> m_inStream;
    v_int32 m_connectionState;
  private:
    oatpp::web::server::HttpRouter::BranchRouter::Route m_currentRoute;
    std::shared_ptr<protocol::http::incoming::Request> m_currentRequest;
    std::shared_ptr<protocol::http::outgoing::Response> m_currentResponse;
  public:
    
    Coroutine(const std::shared_ptr<Components>& components,
              const std::shared_ptr<oatpp::data::stream::IOStream>& connection);
    
    Action act() override;

    Action parseHeaders();
    
    Action onHeadersParsed(const RequestHeadersReader::Result& headersReadResult);
    
    Action onRequestFormed();
    Action onResponse(const std::shared_ptr<protocol::http::outgoing::Response>& response);
    Action onResponseFormed();
    Action onRequestDone();
    
    Action handleError(Error* error) override;
    
  };
  
};
  
}}}

#endif /* oatpp_web_server_HttpProcessor_hpp */
