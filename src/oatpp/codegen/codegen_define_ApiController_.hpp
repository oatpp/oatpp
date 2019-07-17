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
 * This file contains "defines" for ApiController code generating macro. <br>
 * Usage:<br>
 *
 * ```cpp
 * #include OATPP_CODEGEN_BEGIN(ApiController)
 * ...
 * // Generated Endpoints.
 * ...
 * #include OATPP_CODEGEN_END(ApiController)
 * ```
 *
 *
 * *For details see:*
 * <ul>
 *   <li>[ApiController component](https://oatpp.io/docs/components/api-controller/)</li>
 *   <li>&id:oatpp::web::server::api::ApiController;</li>
 * </ul>
 */

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

#define OATPP_MACRO_API_CONTROLLER_PARAM_MACRO(MACRO, INFO, TYPE, NAME, PARAM_LIST) MACRO(TYPE, NAME, PARAM_LIST)
#define OATPP_MACRO_API_CONTROLLER_PARAM_INFO(MACRO, INFO, TYPE, NAME, PARAM_LIST) INFO(TYPE, NAME, PARAM_LIST)
#define OATPP_MACRO_API_CONTROLLER_PARAM_TYPE(MACRO, INFO, TYPE, NAME, PARAM_LIST) TYPE
#define OATPP_MACRO_API_CONTROLLER_PARAM_NAME(MACRO, INFO, TYPE, NAME, PARAM_LIST) NAME
#define OATPP_MACRO_API_CONTROLLER_PARAM_TYPE_STR(MACRO, INFO, TYPE, NAME, PARAM_LIST) #TYPE
#define OATPP_MACRO_API_CONTROLLER_PARAM_NAME_STR(MACRO, INFO, TYPE, NAME, PARAM_LIST) #NAME
#define OATPP_MACRO_API_CONTROLLER_PARAM(MACRO, INFO, TYPE, NAME, PARAM_LIST) (MACRO, INFO, TYPE, NAME, PARAM_LIST)

#define REQUEST(TYPE, NAME) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_REQUEST, OATPP_MACRO_API_CONTROLLER_REQUEST_INFO, TYPE, NAME, ())

#define HEADER(TYPE, NAME, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_HEADER, OATPP_MACRO_API_CONTROLLER_HEADER_INFO, TYPE, NAME, (__VA_ARGS__))

#define PATH(TYPE, NAME, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_PATH, OATPP_MACRO_API_CONTROLLER_PATH_INFO, TYPE, NAME, (__VA_ARGS__))

#define QUERIES(TYPE, NAME) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_QUERIES, OATPP_MACRO_API_CONTROLLER_QUERIES_INFO, TYPE, NAME, ())

#define QUERY(TYPE, NAME, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_QUERY, OATPP_MACRO_API_CONTROLLER_QUERY_INFO, TYPE, NAME, (__VA_ARGS__))

#define BODY_STRING(TYPE, NAME) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_BODY_STRING, OATPP_MACRO_API_CONTROLLER_BODY_STRING_INFO, TYPE, NAME, ())

#define BODY_DTO(TYPE, NAME) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_BODY_DTO, OATPP_MACRO_API_CONTROLLER_BODY_DTO_INFO, TYPE, NAME, ())


// REQUEST MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_REQUEST(TYPE, NAME, PARAM_LIST) \
TYPE NAME = __request;

#define OATPP_MACRO_API_CONTROLLER_REQUEST_INFO(TYPE, NAME, PARAM_LIST)


// HEADER MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_HEADER_0(TYPE, NAME, PARAM_LIST) \
auto __param_str_val_##NAME = __request->getHeader(#NAME); \
if(!__param_str_val_##NAME){ \
  return ApiController::handleError(Status::CODE_400, "Missing HEADER parameter '" #NAME "'"); \
} \
bool __param_validation_check_##NAME; \
TYPE NAME = TYPE::Class::parseFromString(__param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  return ApiController::handleError(Status::CODE_400, "Invalid HEADER parameter '" #NAME "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_HEADER_1(TYPE, NAME, PARAM_LIST) \
auto __param_str_val_##NAME = __request->getHeader(OATPP_MACRO_FIRSTARG PARAM_LIST); \
if(!__param_str_val_##NAME){ \
  return ApiController::handleError(Status::CODE_400, \
  oatpp::String("Missing HEADER parameter '") + OATPP_MACRO_FIRSTARG PARAM_LIST + "'"); \
} \
bool __param_validation_check_##NAME; \
TYPE NAME = TYPE::Class::parseFromString(__param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  return ApiController::handleError(Status::CODE_400, \
                                    oatpp::String("Invalid HEADER parameter '") + \
                                    OATPP_MACRO_FIRSTARG PARAM_LIST + \
                                    "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_HEADER_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_HEADER_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_HEADER_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_HEADER_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CONTROLLER_HEADER(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_HEADER_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// __INFO

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO_0(TYPE, NAME, PARAM_LIST) \
info->headers.add(#NAME, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO_1(TYPE, NAME, PARAM_LIST) \
info->headers.add(OATPP_MACRO_FIRSTARG PARAM_LIST, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_HEADER_INFO_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_HEADER_INFO_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_HEADER_INFO_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// PATH MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_PATH_0(TYPE, NAME, PARAM_LIST) \
auto __param_str_val_##NAME = __request->getPathVariable(#NAME); \
if(!__param_str_val_##NAME){ \
  return ApiController::handleError(Status::CODE_400, "Missing PATH parameter '" #NAME "'"); \
} \
bool __param_validation_check_##NAME; \
TYPE NAME = TYPE::Class::parseFromString(__param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  return ApiController::handleError(Status::CODE_400, "Invalid PATH parameter '" #NAME "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_PATH_1(TYPE, NAME, PARAM_LIST) \
auto __param_str_val_##NAME = __request->getPathVariable(OATPP_MACRO_FIRSTARG PARAM_LIST); \
if(!__param_str_val_##NAME){ \
  return ApiController::handleError(Status::CODE_400, \
  oatpp::String("Missing PATH parameter '") + OATPP_MACRO_FIRSTARG PARAM_LIST + "'"); \
} \
bool __param_validation_check_##NAME; \
TYPE NAME = TYPE::Class::parseFromString(__param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  return ApiController::handleError(Status::CODE_400, \
                                    oatpp::String("Invalid PATH parameter '") + \
                                    OATPP_MACRO_FIRSTARG PARAM_LIST + \
                                    "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_PATH_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_PATH_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_PATH_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_PATH_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CONTROLLER_PATH(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_PATH_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// __INFO

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO_0(TYPE, NAME, PARAM_LIST) \
info->pathParams.add(#NAME, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO_1(TYPE, NAME, PARAM_LIST) \
info->pathParams.add(OATPP_MACRO_FIRSTARG PARAM_LIST, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_PATH_INFO_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_PATH_INFO_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_PATH_INFO_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// QUERIES MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_QUERIES(TYPE, NAME, PARAM_LIST) \
TYPE NAME = __request->getQueryParameters();

#define OATPP_MACRO_API_CONTROLLER_QUERIES_INFO(TYPE, NAME, PARAM_LIST)

// QUERY MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_QUERY_0(TYPE, NAME, PARAM_LIST) \
auto __param_str_val_##NAME = __request->getQueryParameter(#NAME); \
if(!__param_str_val_##NAME){ \
  return ApiController::handleError(Status::CODE_400, "Missing QUERY parameter '" #NAME "'"); \
} \
bool __param_validation_check_##NAME; \
TYPE NAME = TYPE::Class::parseFromString(__param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  return ApiController::handleError(Status::CODE_400, "Invalid QUERY parameter '" #NAME "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_QUERY_1(TYPE, NAME, PARAM_LIST) \
auto __param_str_val_##NAME = __request->getQueryParameter(OATPP_MACRO_FIRSTARG PARAM_LIST); \
if(!__param_str_val_##NAME){ \
  return ApiController::handleError(Status::CODE_400, \
  oatpp::String("Missing QUERY parameter '") + OATPP_MACRO_FIRSTARG PARAM_LIST + "'"); \
} \
bool __param_validation_check_##NAME; \
TYPE NAME = TYPE::Class::parseFromString(__param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  return ApiController::handleError(Status::CODE_400, \
                                    oatpp::String("Invalid QUERY parameter '") + \
                                    OATPP_MACRO_FIRSTARG PARAM_LIST + \
                                    "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_QUERY_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_QUERY_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_QUERY_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_QUERY_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CONTROLLER_QUERY(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_QUERY_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// __INFO

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_0(TYPE, NAME, PARAM_LIST) \
info->queryParams.add(#NAME, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_1(TYPE, NAME, PARAM_LIST) \
info->queryParams.add(OATPP_MACRO_FIRSTARG PARAM_LIST, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_CHOOSER(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_QUERY_INFO_##HAS_ARGS (TYPE, NAME, PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, HAS_ARGS) \
OATPP_MACRO_API_CONTROLLER_QUERY_INFO_CHOOSER (TYPE, NAME, PARAM_LIST, HAS_ARGS)

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO(TYPE, NAME, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_QUERY_INFO_CHOOSER_EXP(TYPE, NAME, PARAM_LIST, OATPP_MACRO_HAS_ARGS PARAM_LIST);

// BODY_STRING MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_BODY_STRING(TYPE, NAME, PARAM_LIST) \
TYPE NAME = __request->readBodyToString();

// __INFO

#define OATPP_MACRO_API_CONTROLLER_BODY_STRING_INFO(TYPE, NAME, PARAM_LIST) \
info->body.name = #NAME; \
info->body.type = oatpp::data::mapping::type::__class::String::getType();

// BODY_DTO MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_BODY_DTO(TYPE, NAME, PARAM_LIST) \
TYPE NAME; \
__request->readBodyToDto(NAME, getDefaultObjectMapper().get()); \
if(!NAME) { \
  return ApiController::handleError(Status::CODE_400, "Missing valid body parameter '" #NAME "'"); \
}

// __INFO

#define OATPP_MACRO_API_CONTROLLER_BODY_DTO_INFO(TYPE, NAME, PARAM_LIST) \
info->body.name = #NAME; \
info->body.type = TYPE::Class::getType();

// FOR EACH // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_DECL_FIRST(INDEX, COUNT, X) \
OATPP_MACRO_API_CONTROLLER_PARAM_TYPE X OATPP_MACRO_API_CONTROLLER_PARAM_NAME X

#define OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_DECL_REST(INDEX, COUNT, X) \
, OATPP_MACRO_API_CONTROLLER_PARAM_TYPE X OATPP_MACRO_API_CONTROLLER_PARAM_NAME X

#define OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_API_CONTROLLER_PARAM_MACRO X

#define OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_CALL_FIRST(INDEX, COUNT, X) \
OATPP_MACRO_API_CONTROLLER_PARAM_NAME X

#define OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_CALL_REST(INDEX, COUNT, X) \
, OATPP_MACRO_API_CONTROLLER_PARAM_NAME X

#define OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_INFO(INDEX, COUNT, X) \
OATPP_MACRO_API_CONTROLLER_PARAM_INFO X

// ENDPOINT_INFO MACRO // ------------------------------------------------------

#define ENDPOINT_INFO(NAME) \
\
std::shared_ptr<Endpoint::Info> Z__ENDPOINT_CREATE_ADDITIONAL_INFO_##NAME() { \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  Z__ENDPOINT_ADD_INFO_##NAME(info); \
  return info; \
} \
\
const std::shared_ptr<Endpoint::Info> Z__ENDPOINT_ADDITIONAL_INFO_##NAME = Z__ENDPOINT_CREATE_ADDITIONAL_INFO_##NAME(); \
\
void Z__ENDPOINT_ADD_INFO_##NAME(const std::shared_ptr<Endpoint::Info>& info)

// ENDPOINT MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_DEFAULTS(NAME, METHOD, PATH, LIST) \
\
template<class T> \
static typename Handler<T>::Method Z__ENDPOINT_METHOD_##NAME(T* controller) { \
  return &T::Z__PROXY_METHOD_##NAME; \
} \
\
std::shared_ptr<Endpoint::Info> Z__EDNPOINT_INFO_GET_INSTANCE_##NAME() { \
  std::shared_ptr<Endpoint::Info> info = getEndpointInfo(#NAME); \
  if(!info){ \
    info = Endpoint::Info::createShared(); \
    setEndpointInfo(#NAME, info); \
  } \
  return info; \
}

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_0(NAME, METHOD, PATH, LIST)  \
\
std::shared_ptr<Endpoint::Info> Z__CREATE_ENDPOINT_INFO_##NAME() { \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  info->name = #NAME; \
  info->path = PATH; \
  info->method = METHOD; \
  return info; \
} \
\
const std::shared_ptr<Endpoint> Z__ENDPOINT_##NAME = createEndpoint(m_endpoints, \
                                                        this, \
                                                        Z__ENDPOINT_METHOD_##NAME(this), \
                                                        nullptr, \
                                                        Z__CREATE_ENDPOINT_INFO_##NAME());

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_0(NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_DEFAULTS(NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_0(NAME, METHOD, PATH, LIST) \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__PROXY_METHOD_##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) \
{ \
  return NAME(); \
} \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> NAME()

////////////////////

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_1(NAME, METHOD, PATH, LIST)  \
\
std::shared_ptr<Endpoint::Info> Z__CREATE_ENDPOINT_INFO_##NAME() { \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  info->name = #NAME; \
  info->path = PATH; \
  info->method = METHOD; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_INFO, LIST) \
  return info; \
} \
\
const std::shared_ptr<Endpoint> Z__ENDPOINT_##NAME = createEndpoint(m_endpoints, \
                                                        this, \
                                                        Z__ENDPOINT_METHOD_##NAME(this), \
                                                        nullptr, \
                                                        Z__CREATE_ENDPOINT_INFO_##NAME());

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_1(NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_DEFAULTS(NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_1(NAME, METHOD, PATH, LIST) \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__PROXY_METHOD_##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) \
{ \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_PUT, LIST) \
  return NAME( \
    OATPP_MACRO_FOREACH_FIRST_AND_REST( \
      OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_CALL_FIRST, \
      OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_CALL_REST, \
      LIST \
    ) \
  ); \
} \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> NAME(\
  OATPP_MACRO_FOREACH_FIRST_AND_REST( \
    OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_DECL_FIRST, \
    OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_DECL_REST, \
    LIST \
  ) \
)

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_(X, NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_##X(NAME, METHOD, PATH, LIST)

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT__(X, NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_(X, NAME, METHOD, PATH, LIST)

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT___(NAME, METHOD, PATH, LIST) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT__(OATPP_MACRO_HAS_ARGS LIST, NAME, METHOD, PATH, LIST)

/**
 * Codegen macoro to be used in `oatpp::web::server::api::ApiController` to generate Endpoint.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.).
 * @param PATH - Path to endpoint (without host).
 * @param NAME - Name of the generated method.
 * @return - std::shared_ptr to &id:oatpp::web::protocol::http::outgoing::Response;.
 */
#define ENDPOINT(METHOD, PATH, NAME, ...) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT___(NAME, METHOD, PATH, (__VA_ARGS__))

// ENDPOINT ASYNC MACRO // ------------------------------------------------------

/*
 *  1 - Method to obtain endpoint call function ptr
 *  2 - Endpoint info singleton
 */
#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_ASYNC_DECL_DEFAULTS(NAME, METHOD, PATH) \
template<class T> \
static typename Handler<T>::MethodAsync Z__ENDPOINT_METHOD_##NAME(T* controller) { \
  return &T::Z__PROXY_METHOD_##NAME; \
} \
\
std::shared_ptr<Endpoint::Info> Z__EDNPOINT_INFO_GET_INSTANCE_##NAME() { \
  std::shared_ptr<Endpoint::Info> info = getEndpointInfo(#NAME); \
  if(!info){ \
    info = Endpoint::Info::createShared(); \
    setEndpointInfo(#NAME, info); \
  } \
  return info; \
}

/*
 *  1 - Endpoint info instance
 *  2 - Endpoint instance
 */
#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_ASYNC_DECL(NAME, METHOD, PATH)  \
\
std::shared_ptr<Endpoint::Info> Z__CREATE_ENDPOINT_INFO_##NAME() { \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  info->name = #NAME; \
  info->path = PATH; \
  info->method = METHOD; \
  return info; \
} \
\
const std::shared_ptr<Endpoint> Z__ENDPOINT_##NAME = createEndpoint(m_endpoints, \
                                                                    this, \
                                                                    nullptr, \
                                                                    Z__ENDPOINT_METHOD_##NAME(this), \
                                                                    Z__CREATE_ENDPOINT_INFO_##NAME());

/**
 * Codegen macoro to be used in `oatpp::web::server::api::ApiController` to generate Asynchronous Endpoint.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.).
 * @param PATH - Path to endpoint (without host).
 * @param NAME - Name of the generated method.
 * @return - &id:oatpp::async::Action;.
 */
#define ENDPOINT_ASYNC(METHOD, PATH, NAME) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_ASYNC_DECL_DEFAULTS(NAME, METHOD, PATH) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_ASYNC_DECL(NAME, METHOD, PATH) \
\
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&> \
Z__PROXY_METHOD_##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) \
{ \
  return NAME::startForResult(this, __request); \
} \
\
class NAME : public HandlerCoroutine<NAME, __ControllerType>

/**
 * Auxiliary codegen macro for `ENDPOINT_ASYNC` to generate correct constructor for Asynchronous Endpoint Coroutine.
 * @NAME - Name of the endpoint. Exact the same name as was passed to `ENDPOINT_ASYNC` macro.
 */
#define ENDPOINT_ASYNC_INIT(NAME) \
public: \
\
  NAME(__ControllerType* pController, \
       const std::shared_ptr<IncomingRequest>& pRequest) \
    : HandlerCoroutine(pController, pRequest) \
  {}
