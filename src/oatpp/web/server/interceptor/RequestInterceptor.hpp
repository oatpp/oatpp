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

#ifndef oatpp_web_server_interceptor_RequestInterceptor_hpp
#define oatpp_web_server_interceptor_RequestInterceptor_hpp

#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace server { namespace interceptor {

/**
 * RequestInterceptor.
 */
class RequestInterceptor {
public:
  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::incoming::Request;.
   */
  typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
public:
  
  /**
   * Default virtual destructor.
   */
  virtual ~RequestInterceptor() = default;
  
  /**
   *
   *  This method should not do any "heavy" nor I/O operations
   *  as it is used for both "Simple" and "Async" API
   *  NOT FOR I/O operations!!!
   *
   *  - return nullptr to continue.
   *  - return OutgoingResponse to send response immediately
   *
   *  possible usage ex: return 301 - redirect if needed
   *
   */
  virtual std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) = 0;
  
};
  
}}}}

#endif /* oatpp_web_server_interceptor_RequestInterceptor_hpp */
