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

#ifndef oatpp_web_protocol_http_incoming_Response_hpp
#define oatpp_web_protocol_http_incoming_Response_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/incoming/BodyDecoder.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {
  
class Response : public oatpp::base::Controllable {
public:
  OBJECT_POOL(Incoming_Response_Pool, Response, 32)
  SHARED_OBJECT_POOL(Shared_Incoming_Response_Pool, Response, 32)
public:
  Response(v_int32 pStatusCode,
           const oatpp::String& pStatusDescription,
           const std::shared_ptr<http::Protocol::Headers>& pHeaders,
           const std::shared_ptr<oatpp::data::stream::InputStream>& pBodyStream)
    : statusCode(pStatusCode)
    , statusDescription(pStatusDescription)
    , headers(pHeaders)
    , bodyStream(pBodyStream)
  {}
public:
  
  static std::shared_ptr<Response> createShared(v_int32 statusCode,
                                          const oatpp::String& statusDescription,
                                          const std::shared_ptr<http::Protocol::Headers>& headers,
                                          const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) {
    return Shared_Incoming_Response_Pool::allocateShared(statusCode, statusDescription, headers, bodyStream);
  }
  
  const v_int32 statusCode;
  const oatpp::String statusDescription;
  const std::shared_ptr<http::Protocol::Headers> headers;
  const std::shared_ptr<oatpp::data::stream::InputStream> bodyStream;
  
  void streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    protocol::http::incoming::BodyDecoder::decode(headers, bodyStream, toStream);
  }
  
  oatpp::String readBodyToString() const {
    return protocol::http::incoming::BodyDecoder::decodeToString(headers, bodyStream);
  }
  
  template<class Type>
  typename Type::PtrWrapper readBodyToDto(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return objectMapper->readFromString<Type>
    (protocol::http::incoming::BodyDecoder::decodeToString(headers, bodyStream));
  }
  
  // Async
  
  oatpp::async::Action streamBodyAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                       const oatpp::async::Action& actionOnReturn,
                                       const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    return protocol::http::incoming::BodyDecoder::decodeAsync(parentCoroutine, actionOnReturn, headers, bodyStream, toStream);
  }
  
  template<typename ParentCoroutineType>
  oatpp::async::Action readBodyToStringAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                             oatpp::async::Action (ParentCoroutineType::*callback)(const oatpp::String&)) const {
    return protocol::http::incoming::BodyDecoder::decodeToStringAsync(parentCoroutine, callback, headers, bodyStream);
  }
  
  template<class DtoType, typename ParentCoroutineType>
  oatpp::async::Action readBodyToDtoAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                          oatpp::async::Action (ParentCoroutineType::*callback)(const typename DtoType::PtrWrapper&),
                                          const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return protocol::http::incoming::BodyDecoder::decodeToDtoAsync<DtoType>(parentCoroutine, callback, headers, bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_Response_hpp */
