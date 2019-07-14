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

#ifndef oatpp_web_protocol_http_incoming_Request_hpp
#define oatpp_web_protocol_http_incoming_Request_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/incoming/BodyDecoder.hpp"
#include "oatpp/web/url/mapping/Pattern.hpp"
#include "oatpp/network/Url.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

/**
 * Class http::incoming::Request AKA IncomingRequest represents client's incoming request.
 */
class Request : public oatpp::base::Countable {
public:
  OBJECT_POOL(Incoming_Request_Pool, Request, 32)
  SHARED_OBJECT_POOL(Shared_Incoming_Request_Pool, Request, 32)
private:

  http::RequestStartingLine m_startingLine;
  url::mapping::Pattern::MatchMap m_pathVariables;
  http::Headers m_headers;
  std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
  
  /*
   * Request should be preconfigured with default BodyDecoder.
   * Custom BodyDecoder can be set on demand
   */
  std::shared_ptr<const http::incoming::BodyDecoder> m_bodyDecoder;

  mutable bool m_queryParamsParsed; // used for lazy parsing of QueryParams
  mutable http::QueryParams m_queryParams;

public:
  
  Request(const http::RequestStartingLine& startingLine,
          const url::mapping::Pattern::MatchMap& pathVariables,
          const http::Headers& headers,
          const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
          const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder);
public:
  
  static std::shared_ptr<Request> createShared(const http::RequestStartingLine& startingLine,
                                               const url::mapping::Pattern::MatchMap& pathVariables,
                                               const http::Headers& headers,
                                               const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                               const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder);

  /**
   * Get map of url query parameters.
   * Query parameters will be lazy parsed from url "tail"
   * Please note: lazy parsing of query parameters is not thread-safe!
   * @return map<key, value> for "&key=value"
   */
  const http::QueryParams& getQueryParameters() const;

  /**
   * Get query parameter value by name
   * @param name
   * @return query parameter value
   */
  oatpp::String getQueryParameter(const oatpp::data::share::StringKeyLabel& name) const;

  /**
   * Get query parameter value by name with defaultValue
   * @param name
   * @param defaultValue
   * @return query parameter value or defaultValue if no such parameter found
   */
  oatpp::String getQueryParameter(const oatpp::data::share::StringKeyLabel& name, const oatpp::String& defaultValue) const;

  /**
   * Get request starting line. (method, path, protocol)
   * @return starting line structure
   */
  const http::RequestStartingLine& getStartingLine() const;

  /**
   * Get path variables according to path-pattern. <br>
   * Ex. given request path="/sum/19/1" for path-pattern="/sum/{a}/{b}" <br>
   * getPathVariables().getVariable("a") == 19, getPathVariables().getVariable("b") == 1.
   *
   * @return url MatchMap
   */
  const url::mapping::Pattern::MatchMap& getPathVariables() const;

  /**
   * Get request's headers map
   * @return Headers map
   */
  const http::Headers& getHeaders() const;

  /**
   * Get request's body stream
   * @return body stream
   */
  std::shared_ptr<oatpp::data::stream::InputStream> getBodyStream() const;

  /**
   * Get body decoder.
   * @return Body decoder
   */
  std::shared_ptr<const http::incoming::BodyDecoder> getBodyDecoder() const;

  /**
   * Get header value
   * @param headerName
   * @return header value
   */
  oatpp::String getHeader(const oatpp::data::share::StringKeyLabelCI_FAST& headerName) const;

  /**
   * Get path variable according to path-pattern
   * @param name
   * @return matched value for path-pattern
   */
  oatpp::String getPathVariable(const oatpp::data::share::StringKeyLabel& name) const;

  /**
   * Get path tail according to path-pattern
   * Ex. given request path="/hello/path/tail" for path-pattern="/hello/\*"
   * tail == "path/tail"
   * note '/' symbol is required before '*'
   * @return matched tail-value for path-pattern
   */
  oatpp::String getPathTail() const;

  /**
   * Transfer body. <br>
   * Read body chunk by chunk and pass chunks to the `writeCallback`.
   * @param writeCallback - &id:oatpp::data::stream::WriteCallback;.
   */
  void transferBody(data::stream::WriteCallback* writeCallback) const;

  /**
   * Stream content of the body-stream to toStream
   * @param toStream
   */
  void transferBodyToStream(oatpp::data::stream::OutputStream* toStream) const;

  /**
   * Transfer body stream to string
   * @return body as string
   */
  oatpp::String readBodyToString() const;

  /**
   * Transfer body to String and parse it as DTO
   * @tparam Type
   * @param objectMapper
   * @return DTO
   */
  template<class Type>
  typename Type::ObjectWrapper readBodyToDto(data::mapping::ObjectMapper* objectMapper) const {
    return objectMapper->readFromString<Type>(m_bodyDecoder->decodeToString(m_headers, m_bodyStream.get()));
  }

  /**
   * Transfer body to String and parse it as DTO
   * (used in ApiController's codegens)
   * @tparam Type
   * @param objectMapper
   * @return DTO
   */
  template<class Type>
  void readBodyToDto(data::mapping::type::PolymorphicWrapper<Type>& objectWrapper,
                     data::mapping::ObjectMapper* objectMapper) const {
    objectWrapper = objectMapper->readFromString<Type>(m_bodyDecoder->decodeToString(m_headers, m_bodyStream.get()));
  }
  
  // Async

  /**
   * Transfer body in Asynchronous manner. <br>
   * Read body chunk by chunk and pass chunks to the `writeCallback`.
   * @param writeCallback - `std::shared_ptr` to &id:oatpp::data::stream::AsyncWriteCallback;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  async::CoroutineStarter transferBodyAsync(const std::shared_ptr<data::stream::AsyncWriteCallback>& writeCallback) const;

  /**
   * Transfer body stream to toStream Async
   * @param toStream
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter transferBodyToStreamAsync(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const;

  /**
   * Transfer body stream to string Async.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  async::CoroutineStarterForResult<const oatpp::String&> readBodyToStringAsync() const;

  /**
   * Transfer body to String and parse it as DTO
   * @tparam DtoType - DTO object type.
   * @param objectMapper
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  template<class DtoType>
  oatpp::async::CoroutineStarterForResult<const typename DtoType::ObjectWrapper&>
  readBodyToDtoAsync(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return m_bodyDecoder->decodeToDtoAsync<DtoType>(m_headers, m_bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_Request_hpp */

