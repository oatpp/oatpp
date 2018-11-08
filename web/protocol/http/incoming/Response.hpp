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
           const std::shared_ptr<oatpp::data::stream::InputStream>& pBodyStream,
           const std::shared_ptr<const http::incoming::BodyDecoder>& pBodyDecoder)
    : statusCode(pStatusCode)
    , statusDescription(pStatusDescription)
    , headers(pHeaders)
    , bodyStream(pBodyStream)
    , bodyDecoder(pBodyDecoder)
  {}
public:
  
  static std::shared_ptr<Response> createShared(v_int32 statusCode,
                                                const oatpp::String& statusDescription,
                                                const std::shared_ptr<http::Protocol::Headers>& headers,
                                                const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                                const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder) {
    return Shared_Incoming_Response_Pool::allocateShared(statusCode, statusDescription, headers, bodyStream, bodyDecoder);
  }
  
  const v_int32 statusCode;
  const oatpp::String statusDescription;
  const std::shared_ptr<http::Protocol::Headers> headers;
  const std::shared_ptr<oatpp::data::stream::InputStream> bodyStream;
  
  /**
   * Response should be preconfigured with default BodyDecoder.
   * Entity that created response object is responsible for providing correct BodyDecoder.
   * Custom BodyDecoder can be set on demand
   */
  std::shared_ptr<const http::incoming::BodyDecoder> bodyDecoder;
  
  void streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    bodyDecoder->decode(headers, bodyStream, toStream);
  }
  
  oatpp::String readBodyToString() const {
    return bodyDecoder->decodeToString(headers, bodyStream);
  }
  
  template<class Type>
  typename Type::ObjectWrapper readBodyToDto(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return bodyDecoder->decodeToDto<Type>(headers, bodyStream, objectMapper);
  }
  
  // Async
  
  oatpp::async::Action streamBodyAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                       const oatpp::async::Action& actionOnReturn,
                                       const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    return bodyDecoder->decodeAsync(parentCoroutine, actionOnReturn, headers, bodyStream, toStream);
  }
  
  template<typename ParentCoroutineType>
  oatpp::async::Action readBodyToStringAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                             oatpp::async::Action (ParentCoroutineType::*callback)(const oatpp::String&)) const {
    return bodyDecoder->decodeToStringAsync(parentCoroutine, callback, headers, bodyStream);
  }
  
  template<class DtoType, typename ParentCoroutineType>
  oatpp::async::Action readBodyToDtoAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                          oatpp::async::Action (ParentCoroutineType::*callback)(const typename DtoType::ObjectWrapper&),
                                          const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return bodyDecoder->decodeToDtoAsync<DtoType>(parentCoroutine, callback, headers, bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_Response_hpp */
