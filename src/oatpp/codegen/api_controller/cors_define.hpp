/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
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

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_ORIGIN "*"
#define OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS"
#define OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization"
#define OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE "1728000"

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY(ARG_ORIGIN, ARG_METHODS, ARG_HEADERS, ARG_MAX_AGE) \
  resp->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_ORIGIN, ARG_ORIGIN); \
  resp->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_METHODS, ARG_METHODS); \
  resp->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_HEADERS, ARG_HEADERS);\
  resp->putHeaderIfNotExists(oatpp::web::protocol::http::Header::CORS_MAX_AGE, ARG_MAX_AGE);

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_MACRO_1(ENDPOINTNAME, ...) \
ENDPOINT_INTERCEPTOR(ENDPOINTNAME, CORS) { \
  auto resp = (this->*intercepted)(request); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_ORIGIN, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_METHODS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
} \
ENDPOINT_INFO(ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { info->hide = true; } \
ENDPOINT("OPTIONS", Z__ENDPOINT_##ENDPOINTNAME->info()->path, ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { \
  auto resp = createResponse(Status::CODE_204, ""); \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_ORIGIN, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_METHODS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
}

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_MACRO_2(ENDPOINTNAME, ORIGIN) \
ENDPOINT_INTERCEPTOR(ENDPOINTNAME, CORS) { \
  auto resp = (this->*intercepted)(request); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
      ORIGIN, \
      OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_METHODS, \
      OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS, \
      OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
} \
ENDPOINT_INFO(ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { info->hide = true; } \
ENDPOINT("OPTIONS", Z__ENDPOINT_##ENDPOINTNAME->info()->path, ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { \
  auto resp = createResponse(Status::CODE_204, ""); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
    ORIGIN, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_METHODS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
}

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_MACRO_3(ENDPOINTNAME, ORIGIN, METHODS) \
ENDPOINT_INTERCEPTOR(ENDPOINTNAME, CORS) { \
  auto resp = (this->*intercepted)(request); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
      ORIGIN, \
      METHODS, \
      OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS, \
      OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
} \
ENDPOINT_INFO(ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { info->hide = true; } \
ENDPOINT("OPTIONS", Z__ENDPOINT_##ENDPOINTNAME->info()->path, ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { \
  auto resp = createResponse(Status::CODE_204, ""); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
    ORIGIN, \
    METHODS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_HEADERS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
}

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_MACRO_4(ENDPOINTNAME, ORIGIN, METHODS, HEADERS) \
ENDPOINT_INTERCEPTOR(ENDPOINTNAME, CORS) { \
  auto resp = (this->*intercepted)(request); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
      ORIGIN, \
      METHODS, \
      HEADERS, \
      OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
} \
ENDPOINT_INFO(ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { info->hide = true; } \
ENDPOINT("OPTIONS", Z__ENDPOINT_##ENDPOINTNAME->info()->path, ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { \
  auto resp = createResponse(Status::CODE_204, ""); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY( \
    ORIGIN, \
    METHODS, \
    HEADERS, \
    OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY_DEFAULT_MAX_AGE \
  ) \
  return resp; \
}

#define OATPP_MACRO_API_CONTROLLER_ADDCORS_MACRO_5(ENDPOINTNAME, ORIGIN, METHODS, HEADERS, MAX_AGE) \
ENDPOINT_INTERCEPTOR(ENDPOINTNAME, CORS) { \
  auto resp = (this->*intercepted)(request); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY(ORIGIN, METHODS, HEADERS, MAX_AGE) \
  return resp; \
} \
ENDPOINT_INFO(ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { info->hide = true; } \
ENDPOINT("OPTIONS", Z__ENDPOINT_##ENDPOINTNAME->info()->path, ZZ__CORS_OPTIONS_ENDPOINT_##ENDPOINTNAME) { \
  auto resp = createResponse(Status::CODE_204, ""); \
  OATPP_MACRO_API_CONTROLLER_ADDCORS_BODY(ORIGIN, METHODS, HEADERS, MAX_AGE) \
  return resp; \
}

#define ADDCORS(...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_ADDCORS_MACRO_, (__VA_ARGS__)) (__VA_ARGS__))
