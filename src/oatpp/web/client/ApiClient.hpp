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

#ifndef oatpp_web_client_Client_hpp
#define oatpp_web_client_Client_hpp

#include "./RequestExecutor.hpp"

#include "oatpp/web/protocol/http/incoming/Response.hpp"

#include "oatpp/web/protocol/http/outgoing/DtoBody.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"

#include "oatpp/core/collection/ListMap.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

#include "oatpp/core/base/Countable.hpp"


#include <string>
#include <list>
#include <unordered_map>

namespace oatpp { namespace web { namespace client {

/**
 * ApiClient class provides convenient Retrofit-like interface over the oatpp::web::client::RequestExecutor.
 */
class ApiClient : public oatpp::base::Countable {
public:
  static constexpr const char* const TAG = "Client";
protected:
  typedef oatpp::collection::ListMap<
    oatpp::String,
    oatpp::data::mapping::type::AbstractObjectWrapper
  > StringToParamMap;
protected:
  typedef std::unordered_map<std::string, oatpp::String> PathVariablesMap;
private:
  typedef oatpp::collection::ListMap<
    oatpp::String,
    oatpp::String
  > StringToStringMap;
public:
  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Status;.
   */
  typedef oatpp::web::protocol::http::Status Status;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Header;.
   */
  typedef oatpp::web::protocol::http::Header Header;
public:
  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::String;.
   */
  typedef oatpp::data::mapping::type::String String;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Int32;.
   */
  typedef oatpp::data::mapping::type::Int32 Int32;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Int64;.
   */
  typedef oatpp::data::mapping::type::Int64 Int64;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Float32;.
   */
  typedef oatpp::data::mapping::type::Float32 Float32;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Float64;.
   */
  typedef oatpp::data::mapping::type::Float64 Float64;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Boolean;.
   */
  typedef oatpp::data::mapping::type::Boolean Boolean;
public:

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::incoming::Response;.
   */
  typedef oatpp::web::protocol::http::incoming::Response Response;
public:

  /**
   * Convenience typedef for &id:oatpp::web::client::RequestExecutor::AsyncCallback;.
   */
  typedef RequestExecutor::AsyncCallback AsyncCallback;
protected:
  
  class PathSegment {
  public:
    constexpr static const v_int32 SEG_PATH = 0;
    constexpr static const v_int32 SEG_VAR = 1;
  public:
    PathSegment(const std::string& pText, v_int32 pType)
      : text (pText)
      , type (pType)
    {}
    const std::string text;
    const v_int32 type;
  };
  
  typedef std::list<PathSegment> PathPattern;
  
private:
  
  void formatPath(oatpp::data::stream::OutputStream* stream,
                  const PathPattern& pathPattern,
                  const std::shared_ptr<StringToParamMap>& params);
  
  void addPathQueryParams(oatpp::data::stream::OutputStream* stream,
                          const std::shared_ptr<StringToParamMap>& params);
  
  oatpp::web::protocol::http::Headers convertParamsMap(const std::shared_ptr<StringToParamMap>& params);
  
protected:
  
  static PathSegment parsePathSegment(p_char8 data, v_int32 size, v_int32& position);
  static PathSegment parseVarSegment(p_char8 data, v_int32 size, v_int32& position);
  static PathPattern parsePathPattern(p_char8 data, v_int32 size);
  
protected:
  std::shared_ptr<RequestExecutor> m_requestExecutor;
  std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
  
public:
  ApiClient(const std::shared_ptr<RequestExecutor>& requestExecutor,
            const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
    : m_requestExecutor(requestExecutor)
    , m_objectMapper(objectMapper)
  {}
public:
  
  static std::shared_ptr<ApiClient> createShared(const std::shared_ptr<RequestExecutor>& requestExecutor,
                                           const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) {
    return std::make_shared<ApiClient>(requestExecutor, objectMapper);
  }
  
protected:
  
  virtual oatpp::String formatPath(const PathPattern& pathPattern,
                                   const std::shared_ptr<StringToParamMap>& pathParams,
                                   const std::shared_ptr<StringToParamMap>& queryParams);
  
public:

  /**
   * Call &id:oatpp::web::client::RequestExecutor::getConnection;.
   * @return - &id:oatpp::web::client::RequestExecutor::ConnectionHandle;.
   */
  virtual std::shared_ptr<RequestExecutor::ConnectionHandle> getConnection();

  /**
   * Call &id:oatpp::web::client::RequestExecutor::getConnectionAsync;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<RequestExecutor::ConnectionHandle>&> getConnectionAsync();


  virtual std::shared_ptr<Response> executeRequest(const oatpp::String& method,
                                                   const PathPattern& pathPattern,
                                                   const std::shared_ptr<StringToParamMap>& headers,
                                                   const std::shared_ptr<StringToParamMap>& pathParams,
                                                   const std::shared_ptr<StringToParamMap>& queryParams,
                                                   const std::shared_ptr<RequestExecutor::Body>& body,
                                                   const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);
  
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<Response>&>
  executeRequestAsync(const oatpp::String& method,
                      const PathPattern& pathPattern,
                      const std::shared_ptr<StringToParamMap>& headers,
                      const std::shared_ptr<StringToParamMap>& pathParams,
                      const std::shared_ptr<StringToParamMap>& queryParams,
                      const std::shared_ptr<RequestExecutor::Body>& body,
                      const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);
  
};
  
}}}

#endif /* oatpp_web_client_Client_hpp */
