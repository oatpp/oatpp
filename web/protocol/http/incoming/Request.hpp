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

#ifndef oatpp_web_protocol_http_incoming_Request_hpp
#define oatpp_web_protocol_http_incoming_Request_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/incoming/BodyDecoder.hpp"
#include "oatpp/web/url/mapping/Pattern.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {
  
class Request : public oatpp::base::Controllable {
public:
  OBJECT_POOL(Incoming_Request_Pool, Request, 32)
  SHARED_OBJECT_POOL(Shared_Incoming_Request_Pool, Request, 32)
private:
  http::RequestStartingLine m_startingLine;
  url::mapping::Pattern::MatchMap m_pathVariables;
  http::Protocol::Headers m_headers;
  std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
  
  /**
   * Request should be preconfigured with default BodyDecoder.
   * Custom BodyDecoder can be set on demand
   */
  std::shared_ptr<const http::incoming::BodyDecoder> m_bodyDecoder;
public:
  /*
  Request(const std::shared_ptr<const http::incoming::BodyDecoder>& pBodyDecoder)
    : bodyDecoder(pBodyDecoder)
  {}
   */
  
  Request(const http::RequestStartingLine& startingLine,
          const url::mapping::Pattern::MatchMap& pathVariables,
          const http::Protocol::Headers& headers,
          const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
          const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder)
    : m_startingLine(startingLine)
    , m_pathVariables(pathVariables)
    , m_headers(headers)
    , m_bodyStream(bodyStream)
    , m_bodyDecoder(bodyDecoder)
  {}
public:
  
  static std::shared_ptr<Request> createShared(const http::RequestStartingLine& startingLine,
                                               const url::mapping::Pattern::MatchMap& pathVariables,
                                               const http::Protocol::Headers& headers,
                                               const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                               const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder) {
    return Shared_Incoming_Request_Pool::allocateShared(startingLine, pathVariables, headers, bodyStream, bodyDecoder);
  }
  
  const http::RequestStartingLine& getStartingLine() const {
    return m_startingLine;
  }
  
  const url::mapping::Pattern::MatchMap& getPathVariables() const {
    return m_pathVariables;
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
  
  oatpp::String getHeader(const oatpp::String& headerName) const{
    auto it = m_headers.find(headerName);
    if(it != m_headers.end()) {
      return it->second.toString();
    }
    return nullptr;
  }
  
  oatpp::String getPathVariable(const oatpp::data::share::StringKeyLabel& name) const {
    return m_pathVariables.getVariable(name);
  }
  
  oatpp::String getPathTail() const {
    return m_pathVariables.getTail();
  }
  
  void streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    m_bodyDecoder->decode(m_headers, m_bodyStream, toStream);
  }
  
  oatpp::String readBodyToString() const {
    return m_bodyDecoder->decodeToString(m_headers, m_bodyStream);
  }
  
  template<class Type>
  typename Type::ObjectWrapper readBodyToDto(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return objectMapper->readFromString<Type>(m_bodyDecoder->decodeToString(m_headers, m_bodyStream));
  }
  
  template<class Type>
  void readBodyToDto(oatpp::data::mapping::type::PolymorphicWrapper<Type>& objectWrapper,
                     const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    objectWrapper = objectMapper->readFromString<Type>(m_bodyDecoder->decodeToString(m_headers, m_bodyStream));
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

#endif /* oatpp_web_protocol_http_incoming_Request_hpp */

