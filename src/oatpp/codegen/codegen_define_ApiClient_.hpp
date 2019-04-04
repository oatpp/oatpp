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

/**[info]
 * This file contains "defines" for ApiClient code generating macro. <br>
 * Usage:<br>
 *
 * ```cpp
 * #include OATPP_CODEGEN_BEGIN(ApiClient)
 * ...
 * // Generated API-Calls.
 * ...
 * #include OATPP_CODEGEN_END(ApiClient)
 * ```
 *
 *
 * *For details see:*
 * <ul>
 *   <li>[ApiClient component](https://oatpp.io/docs/components/api-client/)</li>
 *   <li>&id:oatpp::web::client::ApiClient;</li>
 * </ul>
 */

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

#define OATPP_MACRO_API_CLIENT_PARAM_MACRO(MACRO, TYPE, NAME, PARAM_LIST) MACRO(TYPE, NAME, PARAM_LIST)
#define OATPP_MACRO_API_CLIENT_PARAM_TYPE(MACRO, TYPE, NAME, PARAM_LIST) TYPE
#define OATPP_MACRO_API_CLIENT_PARAM_NAME(MACRO, TYPE, NAME, PARAM_LIST) NAME
#define OATPP_MACRO_API_CLIENT_PARAM_TYPE_STR(MACRO, TYPE, NAME, PARAM_LIST) #TYPE
#define OATPP_MACRO_API_CLIENT_PARAM_NAME_STR(MACRO, TYPE, NAME, PARAM_LIST) #NAME
#define OATPP_MACRO_API_CLIENT_PARAM(MACRO, TYPE, NAME, PARAM_LIST) (MACRO, TYPE, NAME, PARAM_LIST)

#define HEADER(TYPE, NAME, ...) OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_HEADER, TYPE, NAME, (__VA_ARGS__))
#define PATH(TYPE, NAME, ...)   OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_PATH, TYPE, NAME, (__VA_ARGS__))
#define QUERY(TYPE, NAME, ...)  OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_QUERY, TYPE, NAME, (__VA_ARGS__))
#define BODY(TYPE, NAME)        OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_BODY, TYPE, NAME, ())
#define BODY_DTO(TYPE, NAME)    OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_BODY_DTO, TYPE, NAME, ())
#define BODY_STRING(TYPE, NAME) OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_BODY_STRING, TYPE, NAME, ())

// INIT

/**
 * Codegen macoro to be used in classes extending &id:oatpp::web::client::ApiClient; to generate required fields/methods/constructors for ApiClient.
 * @param NAME - name of the ApiClient class.
 */
#define API_CLIENT_INIT(NAME) \
public: \
  NAME(const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, \
       const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) \
    : oatpp::web::client::ApiClient(requestExecutor, objectMapper) \
  {} \
public: \
  static std::shared_ptr<NAME> createShared(const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, \
                                      const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper){ \
    return std::make_shared<NAME>(requestExecutor, objectMapper); \
  }

// HEADER MACRO

#define OATPP_MACRO_API_CLIENT_HEADER_0(TYPE, NAME, PARAM_LIST) \
__headers->put(#NAME, NAME);

#define OATPP_MACRO_API_CLIENT_HEADER_1(TYPE, NAME, PARAM_LIST) \
__headers->put(OATPP_MACRO_FIRSTARG PARAM_LIST, NAME);

#define OATPP_MACRO_API_CLIENT_HEADER_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CLIENT_HEADER_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CLIENT_HEADER_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CLIENT_HEADER_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CLIENT_HEADER(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_HEADER_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// PATH MACRO

#define OATPP_MACRO_API_CLIENT_PATH_0(TYPE, NAME, PARAM_LIST) \
__pathParams->put(#NAME, NAME);

#define OATPP_MACRO_API_CLIENT_PATH_1(TYPE, NAME, PARAM_LIST) \
__pathParams->put(OATPP_MACRO_FIRSTARG PARAM_LIST, NAME);

#define OATPP_MACRO_API_CLIENT_PATH_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CLIENT_PATH_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CLIENT_PATH_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CLIENT_PATH_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CLIENT_PATH(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_PATH_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// QUERY MACRO

#define OATPP_MACRO_API_CLIENT_QUERY_0(TYPE, NAME, PARAM_LIST) \
__queryParams->put(#NAME, NAME);

#define OATPP_MACRO_API_CLIENT_QUERY_1(TYPE, NAME, PARAM_LIST) \
__queryParams->put(OATPP_MACRO_FIRSTARG PARAM_LIST, NAME);

#define OATPP_MACRO_API_CLIENT_QUERY_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CLIENT_QUERY_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CLIENT_QUERY_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CLIENT_QUERY_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CLIENT_QUERY(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_QUERY_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// BODY MACRO

#define OATPP_MACRO_API_CLIENT_BODY(TYPE, NAME, PARAM_LIST) \
__body = NAME;

// BODY_DTO MACRO

#define OATPP_MACRO_API_CLIENT_BODY_DTO(TYPE, NAME, PARAM_LIST) \
__body = oatpp::web::protocol::http::outgoing::DtoBody::createShared(NAME, m_objectMapper.get());

// BODY_STRING MACRO

#define OATPP_MACRO_API_CLIENT_BODY_STRING(TYPE, NAME, PARAM_LIST) \
__body = oatpp::web::protocol::http::outgoing::BufferBody::createShared(NAME);

// FOR EACH

#define OATPP_MACRO_API_CLIENT_PARAM_DECL(INDEX, COUNT, X) \
OATPP_MACRO_API_CLIENT_PARAM_TYPE X OATPP_MACRO_API_CLIENT_PARAM_NAME X,

#define OATPP_MACRO_API_CLIENT_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_API_CLIENT_PARAM_MACRO X

// API_CALL MACRO

#define OATPP_API_CALL_0(NAME, METHOD, PATH, LIST) \
static PathPattern Z_getPathPattern_##NAME(const oatpp::String& path) { \
  static PathPattern pattern = parsePathPattern(path->getData(), path->getSize()); \
  return pattern; \
} \
\
std::shared_ptr<oatpp::web::protocol::http::incoming::Response> NAME( \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body; \
  return executeRequest(METHOD, \
                        Z_getPathPattern_##NAME(PATH), \
                        nullptr, \
                        nullptr, \
                        nullptr, \
                        body, \
                        __connectionHandle); \
}

#define OATPP_API_CALL_1(NAME, METHOD, PATH, LIST) \
static PathPattern Z_getPathPattern_##NAME(const oatpp::String& path) { \
  static PathPattern pattern = parsePathPattern(path->getData(), path->getSize()); \
  return pattern; \
} \
\
std::shared_ptr<oatpp::web::protocol::http::incoming::Response> NAME(\
OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_DECL, LIST) \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  auto __headers = oatpp::web::client::ApiClient::StringToParamMap::createShared(); \
  auto __pathParams = oatpp::web::client::ApiClient::StringToParamMap::createShared(); \
  auto __queryParams = oatpp::web::client::ApiClient::StringToParamMap::createShared(); \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> __body; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_PUT, LIST) \
  return executeRequest(METHOD, \
                        Z_getPathPattern_##NAME(PATH), \
                        __headers, \
                        __pathParams, \
                        __queryParams, \
                        __body, \
                        __connectionHandle); \
}

#define OATPP_API_CALL_(X, NAME, METHOD, PATH, LIST) OATPP_API_CALL_##X(NAME, METHOD, PATH, LIST)
#define OATPP_API_CALL__(X, NAME, METHOD, PATH, LIST) OATPP_API_CALL_(X, NAME, METHOD, PATH, LIST)
#define OATPP_API_CALL___(NAME, METHOD, PATH, LIST) OATPP_API_CALL__(OATPP_MACRO_HAS_ARGS LIST, NAME, METHOD, PATH, LIST)

/**
 * Codegen macoro to be used in `oatpp::web::client::ApiClient` to generate REST API-Calls.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.)
 * @param PATH - Path to endpoint (without host)
 * @param NAME - Name of the generated method
 * @return - std::shared_ptr to &id:oatpp::web::protocol::http::incoming::Response;
 */
#define API_CALL(METHOD, PATH, NAME, ...) \
OATPP_API_CALL___(NAME, METHOD, PATH, (__VA_ARGS__))

// API_CALL_ASYNC MACRO

#define OATPP_API_CALL_ASYNC_0(NAME, METHOD, PATH, LIST) \
static PathPattern Z_getPathPattern_##NAME(const oatpp::String& path) { \
  static PathPattern pattern = parsePathPattern(path->getData(), path->getSize()); \
  return pattern; \
} \
\
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&> NAME( \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body; \
  return executeRequestAsync(METHOD, \
                             Z_getPathPattern_##NAME(PATH), \
                             nullptr, \
                             nullptr, \
                             nullptr, \
                             body, \
                             __connectionHandle); \
}

#define OATPP_API_CALL_ASYNC_1(NAME, METHOD, PATH, LIST) \
static PathPattern Z_getPathPattern_##NAME(const oatpp::String& path) { \
  static PathPattern pattern = parsePathPattern(path->getData(), path->getSize()); \
  return pattern; \
} \
\
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&> NAME(\
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_DECL, LIST) \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  auto __headers = oatpp::web::client::ApiClient::StringToParamMap::createShared(); \
  auto __pathParams = oatpp::web::client::ApiClient::StringToParamMap::createShared(); \
  auto __queryParams = oatpp::web::client::ApiClient::StringToParamMap::createShared(); \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> __body; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_PUT, LIST) \
  return executeRequestAsync(METHOD, \
                             Z_getPathPattern_##NAME(PATH), \
                             __headers, \
                             __pathParams, \
                             __queryParams, \
                             __body, \
                             __connectionHandle); \
}

#define OATPP_API_CALL_ASYNC_(X, NAME, METHOD, PATH, LIST) OATPP_API_CALL_ASYNC_##X(NAME, METHOD, PATH, LIST)
#define OATPP_API_CALL_ASYNC__(X, NAME, METHOD, PATH, LIST) OATPP_API_CALL_ASYNC_(X, NAME, METHOD, PATH, LIST)
#define OATPP_API_CALL_ASYNC___(NAME, METHOD, PATH, LIST) OATPP_API_CALL_ASYNC__(OATPP_MACRO_HAS_ARGS LIST, NAME, METHOD, PATH, LIST)

/**
 * Codegen macoro to be used in `oatpp::web::client::ApiClient` to generate Asynchronous REST API-Calls.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.)
 * @param PATH - Path to endpoint (without host)
 * @param NAME - Name of the generated method
 * @return - &id:oatpp::async::CoroutineStarterForResult;<const std::shared_ptr<&id:oatpp::web::protocol::http::incoming::Response;>&>.
 */
#define API_CALL_ASYNC(METHOD, PATH, NAME, ...) \
OATPP_API_CALL_ASYNC___(NAME, METHOD, PATH, (__VA_ARGS__))
