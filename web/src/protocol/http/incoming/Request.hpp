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

#include "./../Http.hpp"
#include "./BodyDecoder.hpp"
#include "../../../url/mapping/Pattern.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {
  
class Request : public oatpp::base::Controllable {
public:
  OBJECT_POOL(Incoming_Request_Pool, Request, 32)
  SHARED_OBJECT_POOL(Shared_Incoming_Request_Pool, Request, 32)
public:
  Request(const std::shared_ptr<http::RequestStartingLine>& pStartingLine,
          const std::shared_ptr<url::mapping::Pattern::MatchMap>& pPathVariables,
          const std::shared_ptr<http::Protocol::Headers>& pHeaders,
          const std::shared_ptr<oatpp::data::stream::InputStream>& pBodyStream)
    : startingLine(pStartingLine)
    , pathVariables(pPathVariables)
    , headers(pHeaders)
    , bodyStream(pBodyStream)
  {}
public:
  
  static std::shared_ptr<Request> createShared(const std::shared_ptr<http::RequestStartingLine>& startingLine,
                                         const std::shared_ptr<url::mapping::Pattern::MatchMap>& pathVariables,
                                         const std::shared_ptr<http::Protocol::Headers>& headers,
                                         const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) {
    return Shared_Incoming_Request_Pool::allocateShared(startingLine, pathVariables, headers, bodyStream);
  }
  
  const std::shared_ptr<http::RequestStartingLine> startingLine;
  const std::shared_ptr<url::mapping::Pattern::MatchMap> pathVariables;
  const std::shared_ptr<http::Protocol::Headers> headers;
  const std::shared_ptr<oatpp::data::stream::InputStream> bodyStream;
  
  base::String::SharedWrapper getHeader(const base::String::SharedWrapper& headerName) const{
    auto entry = headers->find(headerName);
    if(entry != nullptr) {
      return entry->getValue();
    }
    return nullptr;
  }
  
  base::String::SharedWrapper getPathVariable(const base::String::SharedWrapper& name) const{
    auto entry = pathVariables->getVariable(name);
    if(entry != nullptr) {
      return entry->getValue();
    }
    return nullptr;
  }
  
  std::shared_ptr<base::String> getPathTail() const{
    return pathVariables->tail;
  }
  
  void streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
    protocol::http::incoming::BodyDecoder::decode(headers, bodyStream, toStream);
  }
  
  std::shared_ptr<oatpp::base::String> readBodyToString() const {
    return protocol::http::incoming::BodyDecoder::decodeToString(headers, bodyStream);
  }
  
  template<class Type>
  typename Type::SharedWrapper readBodyToDTO(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return objectMapper->readFromString<Type>
    (protocol::http::incoming::BodyDecoder::decodeToString(headers, bodyStream));
  }
  
  template<class Type>
  void readBodyToDTO(oatpp::base::SharedWrapper<Type>& objectWrapper,
                     const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    objectWrapper = objectMapper->readFromString<Type>
    (protocol::http::incoming::BodyDecoder::decodeToString(headers, bodyStream));
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_Request_hpp */

