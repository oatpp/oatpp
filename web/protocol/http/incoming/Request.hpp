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
public:
  /*
  Request(const std::shared_ptr<const http::incoming::BodyDecoder>& pBodyDecoder)
    : bodyDecoder(pBodyDecoder)
  {}
   */
  
  Request(const http::RequestStartingLine& pStartingLine,
          const url::mapping::Pattern::MatchMap& pPathVariables,
          const http::Protocol::Headers& pHeaders,
          const std::shared_ptr<oatpp::data::stream::InputStream>& pBodyStream,
          const std::shared_ptr<const http::incoming::BodyDecoder>& pBodyDecoder)
    : startingLine(pStartingLine)
    , pathVariables(pPathVariables)
    , headers(pHeaders)
    , bodyStream(pBodyStream)
    , bodyDecoder(pBodyDecoder)
  {}
public:
  
  static std::shared_ptr<Request> createShared(const http::RequestStartingLine& startingLine,
                                               const url::mapping::Pattern::MatchMap& pathVariables,
                                               const http::Protocol::Headers& headers,
                                               const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                               const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder) {
    return Shared_Incoming_Request_Pool::allocateShared(startingLine, pathVariables, headers, bodyStream, bodyDecoder);
  }
  
  http::RequestStartingLine startingLine;
  url::mapping::Pattern::MatchMap pathVariables;
  http::Protocol::Headers headers;
  std::shared_ptr<oatpp::data::stream::InputStream> bodyStream;
  
  /**
   * Request should be preconfigured with default BodyDecoder.
   * Custom BodyDecoder can be set on demand
   */
  std::shared_ptr<const http::incoming::BodyDecoder> bodyDecoder;
  
  oatpp::String getHeader(const oatpp::String& headerName) const{
    auto it = headers.find(headerName);
    if(it != headers.end()) {
      return it->second.toString();
    }
    return nullptr;
  }
  
  oatpp::String getPathVariable(const oatpp::data::share::StringKeyLabel& name) const {
    return pathVariables.getVariable(name);
  }
  
  oatpp::String getPathTail() const {
    return pathVariables.getTail();
  }
  
  void streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    bodyDecoder->decode(headers, bodyStream, toStream);
  }
  
  oatpp::String readBodyToString() const {
    return bodyDecoder->decodeToString(headers, bodyStream);
  }
  
  template<class Type>
  typename Type::ObjectWrapper readBodyToDto(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return objectMapper->readFromString<Type>(bodyDecoder->decodeToString(headers, bodyStream));
  }
  
  template<class Type>
  void readBodyToDto(oatpp::data::mapping::type::PolymorphicWrapper<Type>& objectWrapper,
                     const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    objectWrapper = objectMapper->readFromString<Type>(bodyDecoder->decodeToString(headers, bodyStream));
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

#endif /* oatpp_web_protocol_http_incoming_Request_hpp */

