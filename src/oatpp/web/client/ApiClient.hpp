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

#include "oatpp/data/share/StringTemplate.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "oatpp/Types.hpp"

#include "oatpp/utils/Conversion.hpp"
#include "oatpp/base/Log.hpp"

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
   * Convenience typedef for &id:oatpp::data::type::String;.
   */
  typedef oatpp::String String;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int8;.
   */
  typedef oatpp::Int8 Int8;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt8;.
   */
  typedef oatpp::UInt8 UInt8;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int16;.
   */
  typedef oatpp::Int16 Int16;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt16;.
   */
  typedef oatpp::UInt16 UInt16;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int32;.
   */
  typedef oatpp::Int32 Int32;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt32;.
   */
  typedef oatpp::UInt32 UInt32;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int64;.
   */
  typedef oatpp::Int64 Int64;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt64;.
   */
  typedef oatpp::UInt64 UInt64;

  /**
   * Convenience typedef for &id:oatpp::data::type::Float32;.
   */
  typedef oatpp::Float32 Float32;

  /**
   * Convenience typedef for &id:atpp::data::type::Float64;.
   */
  typedef oatpp::Float64 Float64;

  /**
   * Convenience typedef for &id:oatpp::data::type::Boolean;.
   */
  typedef oatpp::Boolean Boolean;

  template <class T>
  using Enum = oatpp::data::type::Enum<T>;

  template <class T>
  using Object = oatpp::Object<T>;
public:

  typedef oatpp::data::share::StringTemplate StringTemplate;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Headers;.
   */
  typedef oatpp::web::protocol::http::Headers Headers;

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

  struct PathTemplateExtra {
    oatpp::String name;
    bool hasQueryParams;
  };

protected:

  StringTemplate parsePathTemplate(const oatpp::String& name, const oatpp::String& text);

  oatpp::String formatPath(const StringTemplate& pathTemplate,
                           const std::unordered_map<oatpp::String, oatpp::String>& pathParams,
                           const std::unordered_map<oatpp::String, oatpp::String>& queryParams);

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
                                                 const std::shared_ptr<data::mapping::ObjectMapper>& objectMapper) {
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

  /**
   * Get ObjectMapper that was passed to constructor.
   * @return - &id:oatpp::data::mapping::ObjectMapper;.
   */
  std::shared_ptr<oatpp::data::mapping::ObjectMapper> getObjectMapper();

  virtual std::shared_ptr<Response> executeRequest(const oatpp::String& method,
                                                   const StringTemplate& pathTemplate,
                                                   const Headers& headers,
                                                   const std::unordered_map<oatpp::String, oatpp::String>& pathParams,
                                                   const std::unordered_map<oatpp::String, oatpp::String>& queryParams,
                                                   const std::shared_ptr<RequestExecutor::Body>& body,
                                                   const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);
  
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<Response>&>
  executeRequestAsync(const oatpp::String& method,
                      const StringTemplate& pathTemplate,
                      const Headers& headers,
                      const std::unordered_map<oatpp::String, oatpp::String>& pathParams,
                      const std::unordered_map<oatpp::String, oatpp::String>& queryParams,
                      const std::shared_ptr<RequestExecutor::Body>& body,
                      const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);

public:

  template<typename T>
  struct TypeInterpretation {

    static oatpp::String toString(const oatpp::String& typeName, const T& parameter) {

      (void) parameter;

      OATPP_LOGe("[oatpp::web::client::ApiClient::TypeInterpretation::toString()]",
                 "Error. No conversion from '{}' to '{}' is defined.", typeName, "oatpp::String")

      throw std::runtime_error(
        "[oatpp::web::client::ApiClient::TypeInterpretation::toString()]: Error. "
        "No conversion from '" + *typeName + "' to 'oatpp::String' is defined. "
        "Please define type conversion."
      );

    }

  };

};

template<>
struct ApiClient::TypeInterpretation<oatpp::String> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::String &parameter) {
    (void) typeName;
    return parameter;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Int8> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Int8 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::int32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::UInt8> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::UInt8 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::uint32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Int16> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Int16 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::int32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::UInt16> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::UInt16 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::uint32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Int32> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Int32 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::int32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::UInt32> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::UInt32 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::uint32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Int64> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Int64 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::int64ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::UInt64> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::UInt64 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::uint64ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Float32> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Float32 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::float32ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Float64> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Float64 &parameter) {
    (void) typeName;
    if (parameter) {
      return utils::Conversion::float64ToStr(*parameter);
    }
    return nullptr;
  }
};

template<>
struct ApiClient::TypeInterpretation<oatpp::Boolean> {
  static oatpp::String toString(const oatpp::String &typeName, const oatpp::Boolean &parameter) {
    (void) typeName;
    if(parameter != nullptr) {
        return utils::Conversion::boolToStr(*parameter);
    }
    return nullptr;
  }
};

template<class T, class I>
struct ApiClient::TypeInterpretation<data::type::EnumObjectWrapper<T, I>> {

  typedef data::type::EnumObjectWrapper<T, I> EnumOW;
  typedef typename I::UnderlyingTypeObjectWrapper UTOW;

  static oatpp::String toString(const oatpp::String &typeName, const EnumOW &parameter) {

    data::type::EnumInterpreterError error = data::type::EnumInterpreterError::OK;
    const auto& value = I::toInterpretation(parameter, false, error);

    switch(error){
      case data::type::EnumInterpreterError::OK: break;
      case data::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
        throw std::runtime_error(
          "[oatpp::web::client::ApiClient::TypeInterpretation::toString()]: Error. Enum constraint violation - NotNull."
        );
      case data::type::EnumInterpreterError::TYPE_MISMATCH_ENUM:
      case data::type::EnumInterpreterError::TYPE_MISMATCH_ENUM_VALUE:
      case data::type::EnumInterpreterError::ENTRY_NOT_FOUND:
      default:
        throw std::runtime_error(
          "[oatpp::web::client::ApiClient::TypeInterpretation::toString()]: Error. Can't interpret Enum."
        );
    }

    return ApiClient::TypeInterpretation<UTOW>::toString(typeName, value.template cast<UTOW>());

  }

};
  
}}}

#endif /* oatpp_web_client_Client_hpp */
