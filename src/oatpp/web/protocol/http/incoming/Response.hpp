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

#ifndef oatpp_web_protocol_http_incoming_Response_hpp
#define oatpp_web_protocol_http_incoming_Response_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/incoming/BodyDecoder.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {
  
class Response : public oatpp::base::Controllable {
public:
  OBJECT_POOL(Incoming_Response_Pool, Response, 32)
  SHARED_OBJECT_POOL(Shared_Incoming_Response_Pool, Response, 32)
private:
  v_int32 m_statusCode;
  oatpp::String m_statusDescription;
  http::Protocol::Headers m_headers;
  std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
  
  /**
   * Response should be preconfigured with default BodyDecoder.
   * Entity that created response object is responsible for providing correct BodyDecoder.
   * Custom BodyDecoder can be set on demand
   */
  std::shared_ptr<const http::incoming::BodyDecoder> m_bodyDecoder;
  
  std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
  
public:
  Response(v_int32 statusCode,
           const oatpp::String& statusDescription,
           const http::Protocol::Headers& headers,
           const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
           const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder)
    : m_statusCode(statusCode)
    , m_statusDescription(statusDescription)
    , m_headers(headers)
    , m_bodyStream(bodyStream)
    , m_bodyDecoder(bodyDecoder)
  {}
public:
  
  static std::shared_ptr<Response> createShared(v_int32 statusCode,
                                                const oatpp::String& statusDescription,
                                                const http::Protocol::Headers& headers,
                                                const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                                const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder) {
    return Shared_Incoming_Response_Pool::allocateShared(statusCode, statusDescription, headers, bodyStream, bodyDecoder);
  }
  
  v_int32 getStatusCode() const {
    return m_statusCode;
  }
  
  oatpp::String getStatusDescription() const {
    return m_statusDescription;
  }
  
  const http::Protocol::Headers& getHeaders() const {
    return m_headers;
  }
  
  std::shared_ptr<oatpp::data::stream::InputStream> getBodyStream() const {
    return m_bodyStream;
  }
  
  std::shared_ptr<const http::incoming::BodyDecoder> getBodyDecoder() const {
    return m_bodyDecoder;
  }

  void streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    m_bodyDecoder->decode(m_headers, m_bodyStream, toStream);
  }
  
  oatpp::String readBodyToString() const {
    return m_bodyDecoder->decodeToString(m_headers, m_bodyStream);
  }
  
  template<class Type>
  typename Type::ObjectWrapper readBodyToDto(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return m_bodyDecoder->decodeToDto<Type>(m_headers, m_bodyStream, objectMapper);
  }
  
  // Async
  
  oatpp::async::Action streamBodyAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                       const oatpp::async::Action& actionOnReturn,
                                       const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    return m_bodyDecoder->decodeAsync(parentCoroutine, actionOnReturn, m_headers, m_bodyStream, toStream);
  }
  
  template<typename ParentCoroutineType>
  oatpp::async::Action readBodyToStringAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                             oatpp::async::Action (ParentCoroutineType::*callback)(const oatpp::String&)) const {
    return m_bodyDecoder->decodeToStringAsync(parentCoroutine, callback, m_headers, m_bodyStream);
  }
  
  template<class DtoType, typename ParentCoroutineType>
  oatpp::async::Action readBodyToDtoAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                          oatpp::async::Action (ParentCoroutineType::*callback)(const typename DtoType::ObjectWrapper&),
                                          const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return m_bodyDecoder->decodeToDtoAsync<DtoType>(parentCoroutine, callback, m_headers, m_bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_Response_hpp */
