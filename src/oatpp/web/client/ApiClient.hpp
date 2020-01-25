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

#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

#include "oatpp/encoding/Base64.hpp"

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
  typedef std::unordered_map<std::string, oatpp::String> PathVariablesMap;
public:
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
  
  void formatPath(data::stream::ConsistentOutputStream* stream,
                  const PathPattern& pathPattern,
                  const std::shared_ptr<StringToStringMap>& params);
  
  void addPathQueryParams(data::stream::ConsistentOutputStream* stream,
                          const std::shared_ptr<StringToStringMap>& params);

  oatpp::String formatPath(const PathPattern& pathPattern,
                           const std::shared_ptr<StringToStringMap>& pathParams,
                           const std::shared_ptr<StringToStringMap>& queryParams);

  web::protocol::http::Headers mapToHeaders(const std::shared_ptr<StringToStringMap>& params);

protected:
  
  static PathSegment parsePathSegment(p_char8 data, v_buff_size size, v_buff_size& position);
  static PathSegment parseVarSegment(p_char8 data, v_buff_size size, v_buff_size& position);
  static PathPattern parsePathPattern(p_char8 data, v_buff_size size);
  
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

  /**
   * Invalidate connection.
   * @param connectionHandle
   */
  void invalidateConnection(const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle);

  virtual std::shared_ptr<Response> executeRequest(const oatpp::String& method,
                                                   const PathPattern& pathPattern,
                                                   const std::shared_ptr<StringToStringMap>& headers,
                                                   const std::shared_ptr<StringToStringMap>& pathParams,
                                                   const std::shared_ptr<StringToStringMap>& queryParams,
                                                   const std::shared_ptr<RequestExecutor::Body>& body,
                                                   const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);
  
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<Response>&>
  executeRequestAsync(const oatpp::String& method,
                      const PathPattern& pathPattern,
                      const std::shared_ptr<StringToStringMap>& headers,
                      const std::shared_ptr<StringToStringMap>& pathParams,
                      const std::shared_ptr<StringToStringMap>& queryParams,
                      const std::shared_ptr<RequestExecutor::Body>& body,
                      const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);

public:

  template<typename T>
  oatpp::String convertParameterToString(const oatpp::String& typeName, const T& parameter) {
    OATPP_LOGE("[oatpp::web::client::ApiClient::convertParameterToString()]",
              "Error. No conversion from '%s' to '%s' is defined.", typeName->getData(), "oatpp::String");
    throw std::runtime_error("[oatpp::web::client::ApiClient::convertParameterToString()]: Error. "
                             "No conversion from '" + typeName->std_str() + "' to 'oatpp::String' is defined. "
                             "Please define type conversion.");
  }

};

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::String& parameter) {
  (void) typeName;
  return parameter;
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Int8& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::int32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::UInt8& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::uint32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Int16& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::int32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::UInt16& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::uint32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Int32& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::int32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::UInt32& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::uint32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Int64& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::int64ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::UInt64& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::uint64ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Float32& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::float32ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Float64& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::float64ToStr(parameter->getValue());
  }
  return "nullptr";
}

template<>
inline oatpp::String ApiClient::convertParameterToString(const oatpp::String& typeName, const oatpp::Boolean& parameter) {
  (void) typeName;
  if(parameter) {
    return utils::conversion::boolToStr(parameter->getValue());
  }
  return "nullptr";
}
  
}}}

#endif /* oatpp_web_client_Client_hpp */
