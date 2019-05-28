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

#ifndef oatpp_web_server_HttpRequestHandler_hpp
#define oatpp_web_server_HttpRequestHandler_hpp

#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

namespace oatpp { namespace web { namespace server {

/**
 * HTTP request handler.
 */
class HttpRequestHandler {
public:
  typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
public:

  /**
   * Handle incoming http request. <br>
   * *Implement this method.*
   * @param request - incoming http request. &id:oatpp::web::protocol::http::incoming::Request;.
   * @return - outgoing http response. &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  virtual std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) = 0;

  /**
   * Handle incoming http request in Asynchronous manner. <br>
   * *Implement this method.*
   * @param request - &id:oatpp::web::protocol::http::incoming::Request;.
   * @return - &id:oatpp::async::CoroutineStarterForResult; of &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
  handleAsync(const std::shared_ptr<IncomingRequest>& request) = 0;
};

}}}

#endif // oatpp_web_server_HttpRequestHandler_hpp
