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

#ifndef oatpp_test_web_app_Client_hpp
#define oatpp_test_web_app_Client_hpp

#include "./DTOs.hpp"

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"
#include "oatpp/encoding/Base64.hpp"
#include "oatpp/macro/codegen.hpp"

namespace oatpp { namespace test { namespace web { namespace app {
  
class Client : public oatpp::web::client::ApiClient {
public:
  typedef oatpp::web::protocol::http::outgoing::MultipartBody MultipartBody;
public:

#include OATPP_CODEGEN_BEGIN(ApiClient)
  
  API_CLIENT_INIT(Client)
  
  API_CALL("GET", "/", getRoot)
  API_CALL("GET", "/availability", getAvailability)
  API_CALL("GET", "/cors", getCors)
  API_CALL("OPTIONS", "/cors", optionsCors)
  API_CALL("GET", "/cors-origin", getCorsOrigin)
  API_CALL("GET", "/cors-origin-methods", getCorsOriginMethods)
  API_CALL("GET", "/cors-origin-methods-headers", getCorsOriginMethodsHeader)
  API_CALL("GET", "params/{param}", getWithParams, PATH(String, param))
  API_CALL("GET", "queries", getWithQueries, QUERY(String, name), QUERY(Int32, age))
  API_CALL("GET", "queries/optional", getWithOptQueries, QUERY(String, name))
  API_CALL("GET", "queries/map", getWithQueriesMap, QUERY(String, key1), QUERY(Int32, key2), QUERY(Float32, key3))
  API_CALL("GET", "headers", getWithHeaders, HEADER(String, param, "X-TEST-HEADER"))
  API_CALL("POST", "body", postBody, BODY_STRING(String, body))
  API_CALL("POST", "body-dto", postBodyDto, BODY_DTO(Object<TestDto>, body))

  API_CALL("GET", "enum/as-string", getHeaderEnumAsString, HEADER(Enum<AllowedPathParams>::AsString, enumValue, "enum"))
  API_CALL("GET", "enum/as-number", getHeaderEnumAsNumber, HEADER(Enum<AllowedPathParams>::AsNumber, enumValue, "enum"))

  API_CALL("POST", "echo", echoBody, BODY_STRING(String, body))
  API_CALL("POST", "testBodyIsNull1", testBodyIsNull1, BODY_STRING(String, body))
  API_CALL("POST", "testBodyIsNull2", testBodyIsNull2, BODY_STRING(String, body))
  API_CALL("GET", "header-value-set", headerValueSet, HEADER(String, valueSet, "X-VALUE-SET"))

  API_CALL("GET", "default-basic-authorization", defaultBasicAuthorization, AUTHORIZATION_BASIC(String, authString))
  API_CALL("GET", "default-basic-authorization", defaultBasicAuthorizationWithoutHeader)
  API_CALL("GET", "basic-authorization", customBasicAuthorization, AUTHORIZATION_BASIC(String, authString))
  API_CALL("GET", "basic-authorization", customBasicAuthorizationWithoutHeader)

  API_CALL("GET", "bearer-authorization", bearerAuthorization, AUTHORIZATION(String, authString, "Bearer"))

  API_CALL("GET", "chunked/{text-value}/{num-iterations}", getChunked, PATH(String, text, "text-value"), PATH(Int32, numIterations, "num-iterations"))
  API_CALL("POST", "test/multipart/{chunk-size}", multipartTest, PATH(Int32, chunkSize, "chunk-size"), BODY(std::shared_ptr<MultipartBody>, body))

  API_CALL("GET", "test/interceptors", getInterceptors)

  API_CALL("GET", "test/errorhandling", getCaughtError)

  API_CALL_HEADERS(getDefaultHeaders1) {
    headers.put("X-DEFAULT", "hello_1");
  }
  API_CALL("GET", "default_headers", getDefaultHeaders1)

  API_CALL_HEADERS(getDefaultHeaders2) {
    headers.put("X-DEFAULT", "hello_2");
  }
  API_CALL("GET", "default_headers/{param}", getDefaultHeaders2, PATH(String, param))

  API_CALL("GET", "bundle", getBundle)

  API_CALL("GET", "host_header", getHostHeader)

  API_CALL_ASYNC("GET", "/", getRootAsync)
  API_CALL_ASYNC("GET", "/", getRootAsyncWithCKA, HEADER(String, connection, "Connection"))
  API_CALL_ASYNC("GET", "params/{param}", getWithParamsAsync, PATH(String, param))
  API_CALL_ASYNC("GET", "queries", getWithQueriesAsync, QUERY(String, name), QUERY(Int32, age))
  API_CALL_ASYNC("GET", "queries/map", getWithQueriesMapAsync, QUERY(String, key1), QUERY(Int32, key2), QUERY(Float32, key3))
  API_CALL_ASYNC("GET", "headers", getWithHeadersAsync, HEADER(String, param, "X-TEST-HEADER"))
  API_CALL_ASYNC("POST", "body", postBodyAsync, BODY_STRING(String, body))
  API_CALL_ASYNC("POST", "echo", echoBodyAsync, BODY_STRING(String, body))

  API_CALL_ASYNC("GET", "header-value-set", headerValueSetAsync, HEADER(String, valueSet, "X-VALUE-SET"))

  API_CALL_ASYNC("GET", "chunked/{text-value}/{num-iterations}", getChunkedAsync, PATH(String, text, "text-value"), PATH(Int32, numIterations, "num-iterations"))

  API_CALL_HEADERS(GetDefaultHeaders3) {
    headers.put("X-DEFAULT", "hello_3");
  }
  API_CALL_ASYNC("GET", "default_headers", GetDefaultHeaders3)

  API_CALL_HEADERS(GetDefaultHeaders4) {
    headers.put("X-DEFAULT", "hello_4");
  }
  API_CALL_ASYNC("GET", "default_headers/{param}", GetDefaultHeaders4, PATH(String, param))

#include OATPP_CODEGEN_END(ApiClient)
};
  
}}}}

#endif /* oatpp_test_web_app_Client_hpp */
