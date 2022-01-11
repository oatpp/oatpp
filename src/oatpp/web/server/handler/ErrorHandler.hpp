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
   * Virtual destructor since the class is ment to be derived from.
   * */
  virtual ~ErrorHandler() = default;

  /**
   * Implement this method!
   * @param status - &id:oatpp::web::protocol::http::Status;.
   * @param message - &id:oatpp::String;.
   * @param Headers - &id:oatpp::web::protocol::http::Headers;
   * @return - std::shared_ptr to &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  virtual
  std::shared_ptr<protocol::http::outgoing::Response>
  handleError(const protocol::http::Status& status, const oatpp::String& message, const Headers& headers) = 0;

  /**
   * Convenience method to call `handleError` method with no headers.
   * @param status - &id:oatpp::web::protocol::http::Status;
   * @param message - &id:oatpp::String;.
   * @return - std::shared_ptr to &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  std::shared_ptr<protocol::http::outgoing::Response> handleError(const protocol::http::Status& status, const oatpp::String& message);
  
};

/**
 * Default Error Handler.
 */
class DefaultErrorHandler : public oatpp::base::Countable, public ErrorHandler {
public:
  /**
   * Constructor.
   */
  DefaultErrorHandler()
  {}
public:

  /**
   * Create shared DefaultErrorHandler.
   * @return - `std::shared_ptr` to DefaultErrorHandler.
   */
  static std::shared_ptr<DefaultErrorHandler> createShared() {
    return std::make_shared<DefaultErrorHandler>();
  }

  /**
   * Implementation of &l:ErrorHandler::handleError ();
   * @param status - &id:oatpp::web::protocol::http::Status;.
   * @param message - &id:oatpp::String;.
   * @return - &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  std::shared_ptr<protocol::http::outgoing::Response>
  handleError(const protocol::http::Status& status, const oatpp::String& message, const Headers& headers) override;

};
  
}}}}

#endif /* oatpp_web_server_handler_ErrorHandler_hpp */
