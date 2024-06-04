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

#ifndef oatpp_web_server_handler_ErrorHandler_hpp
#define oatpp_web_server_handler_ErrorHandler_hpp

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace server { namespace handler {

/**
 * Error Handler.
 */
class ErrorHandler {
public:
  /**
   * Convenience typedef for Headers. <br>
   * See &id:oatpp::web::protocol::http::Headers;
   */
  typedef web::protocol::http::Headers Headers;
public:
  /**
   * Virtual destructor since the class is meant to be derived from.
   * */
  virtual ~ErrorHandler() = default;

  /**
   * Implement this method!
   * @param exceptionPtr - `std::exception_ptr`.
   * @return - std::shared_ptr to &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  virtual std::shared_ptr<protocol::http::outgoing::Response> handleError(const std::exception_ptr& exceptionPtr) = 0;

};

/**
 * Default Error Handler.
 */
class DefaultErrorHandler : public oatpp::base::Countable, public ErrorHandler {
public:

  struct HttpServerErrorStacktrace {
    std::shared_ptr<protocol::http::incoming::Request> request;
    protocol::http::Status status;
    Headers headers;
    std::list<oatpp::String> stack;
  };

private:
  void unwrapErrorStack(HttpServerErrorStacktrace& stacktrace, const std::exception& e);
public:

  /**
   * Constructor.
   */
  DefaultErrorHandler() = default;

  std::shared_ptr<protocol::http::outgoing::Response> handleError(const std::exception_ptr& error) override;

  /**
   * Reimplement this method for custom error rendering.
   * Render error method.
   * @param stacktrace
   * @return
   */
  virtual std::shared_ptr<protocol::http::outgoing::Response> renderError(const HttpServerErrorStacktrace& stacktrace);

};
  
}}}}

#endif /* oatpp_web_server_handler_ErrorHandler_hpp */
