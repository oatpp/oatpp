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

#ifndef oatpp_web_protocol_http_outgoing_Response_hpp
#define oatpp_web_protocol_http_outgoing_Response_hpp

#include "./Body.hpp"
#include "./../Http.hpp"
#include "../../../../../core/src/async/Routine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class Response : public oatpp::base::Controllable {
public:
  typedef oatpp::collection::ListMap<base::String::PtrWrapper, base::String::PtrWrapper> Headers;
public:
  OBJECT_POOL(Outgoing_Response_Pool, Response, 32)
  SHARED_OBJECT_POOL(Shared_Outgoing_Response_Pool, Response, 32)
public:
  Response(const Status& pStatus,
           const std::shared_ptr<Body>& pBody)
    : status(pStatus)
    , headers(Headers::createShared())
    , body(pBody)
  {}
public:
  
  static std::shared_ptr<Response> createShared(const Status& status,
                                          const std::shared_ptr<Body>& body) {
    return Shared_Outgoing_Response_Pool::allocateShared(status, body);
  }
  
  const Status status;
  const std::shared_ptr<Headers> headers;
  const std::shared_ptr<Body> body;
  
  void send(const std::shared_ptr<data::stream::OutputStream>& stream);
  
  oatpp::async::Action sendAsync(const std::shared_ptr<data::stream::OutputStream>& stream);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Response_hpp */
