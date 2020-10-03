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

#define OATPP_MACRO_API_CLIENT_PARAM_MACRO(MACRO, TYPE, PARAM_LIST) MACRO(TYPE, PARAM_LIST)
#define OATPP_MACRO_API_CLIENT_PARAM_TYPE(MACRO, TYPE, PARAM_LIST) const TYPE&
#define OATPP_MACRO_API_CLIENT_PARAM_NAME(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG PARAM_LIST
#define OATPP_MACRO_API_CLIENT_PARAM_TYPE_STR(MACRO, TYPE, PARAM_LIST) #TYPE
#define OATPP_MACRO_API_CLIENT_PARAM_NAME_STR(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG_STR PARAM_LIST
#define OATPP_MACRO_API_CLIENT_PARAM(MACRO, TYPE, PARAM_LIST) (MACRO, TYPE, PARAM_LIST)

#define HEADER(TYPE, ...)               OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_HEADER, TYPE, (__VA_ARGS__))
#define PATH(TYPE, ...)                 OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_PATH, TYPE, (__VA_ARGS__))
#define QUERY(TYPE, ...)                OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_QUERY, TYPE, (__VA_ARGS__))
#define BODY(TYPE, ...)                 OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_BODY, TYPE, (__VA_ARGS__))
#define BODY_DTO(TYPE, ...)             OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_BODY_DTO, TYPE, (__VA_ARGS__))
#define BODY_STRING(TYPE, ...)          OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_BODY_STRING, TYPE, (__VA_ARGS__))
#define AUTHORIZATION(TYPE, ...)        OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_AUTHORIZATION, TYPE, (__VA_ARGS__))
#define AUTHORIZATION_BASIC(TYPE, ...)  OATPP_MACRO_API_CLIENT_PARAM(OATPP_MACRO_API_CLIENT_AUTHORIZATION_BASIC, TYPE, (__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_API_CLIENT_MACRO_SELECTOR(MACRO, TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (TYPE, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

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

#define OATPP_MACRO_API_CLIENT_HEADER_1(TYPE, NAME) \
__headers.put_LockFree(#NAME, ApiClient::TypeInterpretation<TYPE>::toString(#TYPE, NAME));

#define OATPP_MACRO_API_CLIENT_HEADER_2(TYPE, NAME, QUALIFIER) \
__headers.put_LockFree(QUALIFIER, ApiClient::TypeInterpretation<TYPE>::toString(#TYPE, NAME));

#define OATPP_MACRO_API_CLIENT_HEADER(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_MACRO_SELECTOR(OATPP_MACRO_API_CLIENT_HEADER_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// PATH MACRO

#define OATPP_MACRO_API_CLIENT_PATH_1(TYPE, NAME) \
__pathParams.insert({#NAME, ApiClient::TypeInterpretation<TYPE>::toString(#TYPE, NAME)});

#define OATPP_MACRO_API_CLIENT_PATH_2(TYPE, NAME, QUALIFIER) \
__pathParams.insert({QUALIFIER, ApiClient::TypeInterpretation<TYPE>::toString(#TYPE, NAME)});

#define OATPP_MACRO_API_CLIENT_PATH(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_MACRO_SELECTOR(OATPP_MACRO_API_CLIENT_PATH_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// QUERY MACRO

#define OATPP_MACRO_API_CLIENT_QUERY_1(TYPE, NAME) \
__queryParams.insert({#NAME, ApiClient::TypeInterpretation<TYPE>::toString(#TYPE, NAME)});

#define OATPP_MACRO_API_CLIENT_QUERY_2(TYPE, NAME, QUALIFIER) \
__queryParams.insert({QUALIFIER, ApiClient::TypeInterpretation<TYPE>::toString(#TYPE, NAME)});

#define OATPP_MACRO_API_CLIENT_QUERY(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_MACRO_SELECTOR(OATPP_MACRO_API_CLIENT_QUERY_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// BODY MACRO

#define OATPP_MACRO_API_CLIENT_BODY(TYPE, PARAM_LIST) \
__body = OATPP_MACRO_FIRSTARG PARAM_LIST;

// BODY_DTO MACRO

#define OATPP_MACRO_API_CLIENT_BODY_DTO(TYPE, PARAM_LIST) \
__body = oatpp::web::protocol::http::outgoing::BufferBody::createShared( \
    m_objectMapper->writeToString(OATPP_MACRO_FIRSTARG PARAM_LIST), \
    m_objectMapper->getInfo().http_content_type \
  );

// BODY_STRING MACRO

#define OATPP_MACRO_API_CLIENT_BODY_STRING(TYPE, PARAM_LIST) \
__body = oatpp::web::protocol::http::outgoing::BufferBody::createShared(OATPP_MACRO_FIRSTARG PARAM_LIST);

// AUTHORIZATION MACRO

#define OATPP_MACRO_API_CLIENT_AUTHORIZATION_2(TYPE, TOKEN, SCHEME) \
__headers.put_LockFree("Authorization", String(SCHEME " ") + String(TOKEN));

#define OATPP_MACRO_API_CLIENT_AUTHORIZATION(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_MACRO_SELECTOR(OATPP_MACRO_API_CLIENT_AUTHORIZATION_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// AUTHORIZATION_BASIC MACRO

#define OATPP_MACRO_API_CLIENT_AUTHORIZATION_BASIC_1(TYPE, TOKEN) \
__headers.put_LockFree("Authorization", String("Basic ") + oatpp::encoding::Base64::encode(TOKEN));

#define OATPP_MACRO_API_CLIENT_AUTHORIZATION_BASIC(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CLIENT_MACRO_SELECTOR(OATPP_MACRO_API_CLIENT_AUTHORIZATION_BASIC_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// FOR EACH

#define OATPP_MACRO_API_CLIENT_PARAM_DECL(INDEX, COUNT, X) \
OATPP_MACRO_API_CLIENT_PARAM_TYPE X OATPP_MACRO_API_CLIENT_PARAM_NAME X,

#define OATPP_MACRO_API_CLIENT_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_API_CLIENT_PARAM_MACRO X

// API_CALL MACRO

#define OATPP_API_CALL_0(NAME, METHOD, PATH) \
const oatpp::data::share::StringTemplate Z_PATH_TEMPLATE_##NAME = parsePathTemplate(#NAME, PATH); \
\
static void Z_ADD_HEADERS_##NAME(oatpp::web::client::ApiClient::Headers& headers, ...) { \
  (void) headers; \
} \
\
std::shared_ptr<oatpp::web::protocol::http::incoming::Response> NAME( \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  oatpp::web::client::ApiClient::Headers __headers; \
  Z_ADD_HEADERS_##NAME(__headers, 1); \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body; \
  return executeRequest(METHOD, \
                        Z_PATH_TEMPLATE_##NAME, \
                        __headers, \
                        {}, \
                        {}, \
                        body, \
                        __connectionHandle); \
}

#define OATPP_API_CALL_1(NAME, METHOD, PATH, ...) \
const oatpp::data::share::StringTemplate Z_PATH_TEMPLATE_##NAME = parsePathTemplate(#NAME, PATH); \
\
static void Z_ADD_HEADERS_##NAME(oatpp::web::client::ApiClient::Headers& headers, ...) { \
  (void) headers; \
} \
\
std::shared_ptr<oatpp::web::protocol::http::incoming::Response> NAME(\
OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_DECL, __VA_ARGS__) \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  oatpp::web::client::ApiClient::Headers __headers; \
  Z_ADD_HEADERS_##NAME(__headers, 1); \
  std::unordered_map<oatpp::String, oatpp::String> __pathParams; \
  std::unordered_map<oatpp::String, oatpp::String> __queryParams; \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> __body; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_PUT, __VA_ARGS__) \
  return executeRequest(METHOD, \
                        Z_PATH_TEMPLATE_##NAME, \
                        __headers, \
                        __pathParams, \
                        __queryParams, \
                        __body, \
                        __connectionHandle); \
}

// Chooser

#define OATPP_API_CALL_MACRO_0(METHOD, PATH, NAME) \
OATPP_API_CALL_0(NAME, METHOD, PATH)

#define OATPP_API_CALL_MACRO_1(METHOD, PATH, NAME, ...) \
OATPP_API_CALL_1(NAME, METHOD, PATH, __VA_ARGS__)

/**
 * Codegen macoro to be used in `oatpp::web::client::ApiClient` to generate REST API-Calls.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.)
 * @param PATH - Path to endpoint (without host)
 * @param NAME - Name of the generated method
 * @return - std::shared_ptr to &id:oatpp::web::protocol::http::incoming::Response;
 */
#define API_CALL(METHOD, PATH, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_API_CALL_MACRO_, (__VA_ARGS__)) (METHOD, PATH, __VA_ARGS__))

// API_CALL_ASYNC MACRO

#define OATPP_API_CALL_ASYNC_0(NAME, METHOD, PATH) \
const oatpp::data::share::StringTemplate Z_PATH_TEMPLATE_##NAME = parsePathTemplate(#NAME, PATH); \
\
static void Z_ADD_HEADERS_##NAME(oatpp::web::client::ApiClient::Headers& headers, ...) { \
  (void) headers; \
} \
\
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&> NAME( \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  oatpp::web::client::ApiClient::Headers __headers; \
  Z_ADD_HEADERS_##NAME(__headers, 1); \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body; \
  return executeRequestAsync(METHOD, \
                             Z_PATH_TEMPLATE_##NAME, \
                             __headers, \
                             {}, \
                             {}, \
                             body, \
                             __connectionHandle); \
}

#define OATPP_API_CALL_ASYNC_1(NAME, METHOD, PATH, ...) \
const oatpp::data::share::StringTemplate Z_PATH_TEMPLATE_##NAME = parsePathTemplate(#NAME, PATH); \
\
static void Z_ADD_HEADERS_##NAME(oatpp::web::client::ApiClient::Headers& headers, ...) { \
  (void) headers; \
} \
\
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&> NAME(\
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_DECL, __VA_ARGS__) \
  const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr \
) { \
  oatpp::web::client::ApiClient::Headers __headers; \
  Z_ADD_HEADERS_##NAME(__headers, 1); \
  std::unordered_map<oatpp::String, oatpp::String> __pathParams; \
  std::unordered_map<oatpp::String, oatpp::String> __queryParams; \
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> __body; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CLIENT_PARAM_PUT, __VA_ARGS__) \
  return executeRequestAsync(METHOD, \
                             Z_PATH_TEMPLATE_##NAME, \
                             __headers, \
                             __pathParams, \
                             __queryParams, \
                             __body, \
                             __connectionHandle); \
}

#define OATPP_API_CALL_ASYNC_MACRO_0(METHOD, PATH, NAME) \
OATPP_API_CALL_ASYNC_0(NAME, METHOD, PATH)

#define OATPP_API_CALL_ASYNC_MACRO_1(METHOD, PATH, NAME, ...) \
OATPP_API_CALL_ASYNC_1(NAME, METHOD, PATH, __VA_ARGS__)

/**
 * Codegen macro to be used in `oatpp::web::client::ApiClient` to generate Asynchronous REST API-Calls.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.)
 * @param PATH - Path to endpoint (without host)
 * @param NAME - Name of the generated method
 * @return - &id:oatpp::async::CoroutineStarterForResult;<const std::shared_ptr<&id:oatpp::web::protocol::http::incoming::Response;>&>.
 */
#define API_CALL_ASYNC(METHOD, PATH, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_API_CALL_ASYNC_MACRO_, (__VA_ARGS__)) (METHOD, PATH, __VA_ARGS__))

/**
 * Codegen macro to add default headers to API_CALL
 */
#define API_CALL_HEADERS(NAME) \
\
static void Z_ADD_HEADERS_##NAME(oatpp::web::client::ApiClient::Headers& headers, int) { \
  Z_ADD_HEADERS_##NAME(headers); /* call first method */ \
  Z_ADD_DEFAULT_HEADERS_##NAME(headers); \
} \
\
static void Z_ADD_DEFAULT_HEADERS_##NAME(oatpp::web::client::ApiClient::Headers& headers)
