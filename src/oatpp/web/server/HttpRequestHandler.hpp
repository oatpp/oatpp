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

#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

namespace oatpp { namespace web { namespace server {

/**
 * HTTP request handler.
 */
class HttpRequestHandler {
public:

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Status;.
   */
  typedef oatpp::web::protocol::http::Status Status;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Header;.
   */
  typedef oatpp::web::protocol::http::Header Header;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Headers;.
   */
  typedef oatpp::web::protocol::http::Headers Headers;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::QueryParams;.
   */
  typedef oatpp::web::protocol::http::QueryParams QueryParams;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::incoming::Request;.
   */
  typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::ResponseFactory;.
   */
  typedef oatpp::web::protocol::http::outgoing::ResponseFactory ResponseFactory;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::HttpError;.
   */
  typedef oatpp::web::protocol::http::HttpError HttpError;

public:

  /**
   * Handle incoming http request. <br>
   * *Implement this method.*
   * @param request - incoming http request. &id:oatpp::web::protocol::http::incoming::Request;.
   * @return - outgoing http response. &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  virtual std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) {
    (void)request;
    throw HttpError(Status::CODE_501, "Endpoint not implemented.", {});
  }

  /**
   * Handle incoming http request in Asynchronous manner. <br>
   * *Implement this method.*
   * @param request - &id:oatpp::web::protocol::http::incoming::Request;.
   * @return - &id:oatpp::async::CoroutineStarterForResult; of &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
  handleAsync(const std::shared_ptr<IncomingRequest>& request) {
    (void)request;
    throw HttpError(Status::CODE_501, "Asynchronous endpoint not implemented.", {});
  }

  /**
   * You have to provide a definition for destructors, otherwise its undefined behaviour.
   */
  virtual ~HttpRequestHandler() = default;
};

}}}

#endif // oatpp_web_server_HttpRequestHandler_hpp
