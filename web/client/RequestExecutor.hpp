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

#ifndef oatpp_web_client_RequestExecutor_hpp
#define oatpp_web_client_RequestExecutor_hpp

#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace client {
  
class RequestExecutor {
public:
  typedef oatpp::base::String String;
public:
  typedef oatpp::web::protocol::http::Protocol::Headers Headers;
  typedef oatpp::web::protocol::http::incoming::Response Response;
  typedef oatpp::web::protocol::http::outgoing::Body Body;
public:
  
  virtual std::shared_ptr<Response> execute(const String::PtrWrapper& method,
                                            const String::PtrWrapper& path,
                                            const std::shared_ptr<Headers>& headers,
                                            const std::shared_ptr<Body>& body) = 0;
  
};
  
}}}

#endif /* oatpp_web_client_RequestExecutor_hpp */
