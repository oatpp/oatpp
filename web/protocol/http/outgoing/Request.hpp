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

#ifndef oatpp_web_protocol_http_outgoing_Request_hpp
#define oatpp_web_protocol_http_outgoing_Request_hpp

#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class Request : public oatpp::base::Controllable {
public:
  typedef protocol::http::Protocol::Headers Headers;
public:
  OBJECT_POOL(Outgoing_Request_Pool, Request, 32)
  SHARED_OBJECT_POOL(Shared_Outgoing_Request_Pool, Request, 32)
public:
  Request() {}
  
  Request(const base::String::PtrWrapper& pMethod,
          const base::String::PtrWrapper& pPath,
          const std::shared_ptr<Headers>& pHeaders,
          const std::shared_ptr<Body>& pBody)
    : method(pMethod)
    , path(pPath)
    , headers((!pHeaders) ? (Headers::createShared()) : (pHeaders))
    , body(pBody)
  {}
  
public:
  
  static std::shared_ptr<Request> createShared(const base::String::PtrWrapper& method,
                                         const base::String::PtrWrapper& path,
                                         const std::shared_ptr<Headers>& headers,
                                         const std::shared_ptr<Body>& body) {
    return Shared_Outgoing_Request_Pool::allocateShared(method, path, headers, body);
  }
  
  const base::String::PtrWrapper method;
  const base::String::PtrWrapper path;
  
  const std::shared_ptr<Headers> headers;
  const std::shared_ptr<Body> body;
  
  void send(const std::shared_ptr<data::stream::OutputStream>& stream);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Request_hpp */
