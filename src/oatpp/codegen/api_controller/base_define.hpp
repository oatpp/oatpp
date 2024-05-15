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

#define OATPP_MACRO_API_CONTROLLER_PARAM_MACRO(MACRO, INFO, TYPE, PARAM_LIST) MACRO(TYPE, PARAM_LIST)
#define OATPP_MACRO_API_CONTROLLER_PARAM_INFO(MACRO, INFO, TYPE, PARAM_LIST) INFO(TYPE, PARAM_LIST)
#define OATPP_MACRO_API_CONTROLLER_PARAM_TYPE(MACRO, INFO, TYPE, PARAM_LIST) const TYPE&
#define OATPP_MACRO_API_CONTROLLER_PARAM_NAME(MACRO, INFO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG PARAM_LIST
#define OATPP_MACRO_API_CONTROLLER_PARAM_TYPE_STR(MACRO, INFO, TYPE, PARAM_LIST) #TYPE
#define OATPP_MACRO_API_CONTROLLER_PARAM_NAME_STR(MACRO, INFO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG_STR PARAM_LIST
#define OATPP_MACRO_API_CONTROLLER_PARAM(MACRO, INFO, TYPE, PARAM_LIST) (MACRO, INFO, TYPE, PARAM_LIST)

#define REQUEST(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_REQUEST, OATPP_MACRO_API_CONTROLLER_REQUEST_INFO, TYPE, (__VA_ARGS__))

#define HEADER(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_HEADER, OATPP_MACRO_API_CONTROLLER_HEADER_INFO, TYPE, (__VA_ARGS__))

#define PATH(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_PATH, OATPP_MACRO_API_CONTROLLER_PATH_INFO, TYPE, (__VA_ARGS__))

#define QUERIES(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_QUERIES, OATPP_MACRO_API_CONTROLLER_QUERIES_INFO, TYPE, (__VA_ARGS__))

#define QUERY(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_QUERY, OATPP_MACRO_API_CONTROLLER_QUERY_INFO, TYPE, (__VA_ARGS__))

#define BODY_STRING(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_BODY_STRING, OATPP_MACRO_API_CONTROLLER_BODY_STRING_INFO, TYPE, (__VA_ARGS__))

#define BODY_DTO(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_BODY_DTO, OATPP_MACRO_API_CONTROLLER_BODY_DTO_INFO, TYPE, (__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(MACRO, TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (TYPE, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

// REQUEST MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_REQUEST(TYPE, PARAM_LIST) \
const auto& OATPP_MACRO_FIRSTARG PARAM_LIST = __request;

#define OATPP_MACRO_API_CONTROLLER_REQUEST_INFO(TYPE, PARAM_LIST)


// HEADER MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_HEADER_1(TYPE, NAME) \
const auto& __param_str_val_##NAME = __request->getHeader(#NAME); \
if(!__param_str_val_##NAME){                            \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Missing HEADER parameter '" #NAME "'"); \
} \
bool __param_validation_check_##NAME; \
const auto& NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Invalid HEADER parameter '" #NAME "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_HEADER_2(TYPE, NAME, QUALIFIER) \
const auto& __param_str_val_##NAME = __request->getHeader(QUALIFIER); \
if(!__param_str_val_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, oatpp::String("Missing HEADER parameter '") + QUALIFIER + "'"); \
} \
bool __param_validation_check_##NAME; \
const auto& NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, \
                                              oatpp::String("Invalid HEADER parameter '") + \
                                              QUALIFIER + \
                                              "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_HEADER(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_HEADER_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// __INFO

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO_1(TYPE, NAME) \
info->headers.add(#NAME, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO_2(TYPE, NAME, QUALIFIER) \
info->headers.add(QUALIFIER, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_HEADER_INFO(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_HEADER_INFO_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)


// PATH MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_PATH_1(TYPE, NAME) \
const auto& __param_str_val_##NAME = __request->getPathVariable(#NAME); \
if(!__param_str_val_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Missing PATH parameter '" #NAME "'"); \
} \
bool __param_validation_check_##NAME; \
const auto& NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Invalid PATH parameter '" #NAME "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_PATH_2(TYPE, NAME, QUALIFIER) \
const auto& __param_str_val_##NAME = __request->getPathVariable(QUALIFIER); \
if(!__param_str_val_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, \
                                              oatpp::String("Missing PATH parameter '") + QUALIFIER + "'"); \
} \
bool __param_validation_check_##NAME; \
const auto NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, \
                                              oatpp::String("Invalid PATH parameter '") + \
                                              QUALIFIER + \
                                              "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_PATH(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_PATH_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// __INFO

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO_1(TYPE, NAME) \
info->pathParams.add(#NAME, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO_2(TYPE, NAME, QUALIFIER) \
info->pathParams.add(QUALIFIER, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_PATH_INFO(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_PATH_INFO_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// QUERIES MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_QUERIES(TYPE, PARAM_LIST) \
const auto& OATPP_MACRO_FIRSTARG PARAM_LIST = __request->getQueryParameters();

#define OATPP_MACRO_API_CONTROLLER_QUERIES_INFO(TYPE, PARAM_LIST)

// QUERY MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_QUERY_1(TYPE, NAME) \
const auto& __param_str_val_##NAME = __request->getQueryParameter(#NAME); \
if(!__param_str_val_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Missing QUERY parameter '" #NAME "'"); \
} \
bool __param_validation_check_##NAME; \
const auto& NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Invalid QUERY parameter '" #NAME "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_QUERY_2(TYPE, NAME, QUALIFIER) \
const auto& __param_str_val_##NAME = __request->getQueryParameter(QUALIFIER); \
if(!__param_str_val_##NAME) { \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, \
                                              oatpp::String("Missing QUERY parameter '") + QUALIFIER + "'"); \
} \
bool __param_validation_check_##NAME; \
const auto& NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
if(!__param_validation_check_##NAME){ \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, \
                                              oatpp::String("Invalid QUERY parameter '") + \
                                              QUALIFIER + \
                                              "'. Expected type is '" #TYPE "'"); \
}

#define OATPP_MACRO_API_CONTROLLER_QUERY_3(TYPE, NAME, QUALIFIER, DEFAULT) \
TYPE NAME = DEFAULT; \
const auto& __param_str_val_##NAME = __request->getQueryParameter(QUALIFIER); \
if(__param_str_val_##NAME) { \
  bool __param_validation_check_##NAME; \
  NAME = ApiController::TypeInterpretation<TYPE>::fromString(#TYPE, __param_str_val_##NAME, __param_validation_check_##NAME); \
  if(!__param_validation_check_##NAME){ \
    throw oatpp::web::protocol::http::HttpError(Status::CODE_400, \
                                                oatpp::String("Invalid QUERY parameter '") + \
                                                QUALIFIER + \
                                                "'. Expected type is '" #TYPE "'"); \
  } \
}

#define OATPP_MACRO_API_CONTROLLER_QUERY(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_QUERY_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// __INFO

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_1(TYPE, NAME) \
info->queryParams.add(#NAME, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_2(TYPE, NAME, QUALIFIER) \
info->queryParams.add(QUALIFIER, TYPE::Class::getType());

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO_3(TYPE, NAME, QUALIFIER, DEFAULT) \
info->queryParams.add(QUALIFIER, TYPE::Class::getType());                       \
info->queryParams[QUALIFIER].required = false;

#define OATPP_MACRO_API_CONTROLLER_QUERY_INFO(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_QUERY_INFO_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// BODY_STRING MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_BODY_STRING(TYPE, PARAM_LIST) \
const auto& OATPP_MACRO_FIRSTARG PARAM_LIST = __request->readBodyToString();

// __INFO

#define OATPP_MACRO_API_CONTROLLER_BODY_STRING_INFO(TYPE, PARAM_LIST) \
info->body.name = OATPP_MACRO_FIRSTARG_STR PARAM_LIST; \
info->body.required = true; \
info->body.type = oatpp::data::type::__class::String::getType(); \
if(getContentMappers()->getDefaultMapper()) { \
  info->bodyContentType = getContentMappers()->getDefaultMapper()->getInfo().httpContentType; \
}

// BODY_DTO MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_BODY_DTO(TYPE, PARAM_LIST) \
const auto& __bodyMapper = getContentMappers()->selectMapperForContent(__request->getHeader(oatpp::web::protocol::http::Header::CONTENT_TYPE)); \
if(!__bodyMapper) { \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "No suitable mapper found to deserialize the request body."); \
} \
const auto& OATPP_MACRO_FIRSTARG PARAM_LIST = \
__request->readBodyToDto<TYPE>(__bodyMapper.get()); \
if(!OATPP_MACRO_FIRSTARG PARAM_LIST) { \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_400, "Missing valid body parameter '" OATPP_MACRO_FIRSTARG_STR PARAM_LIST "'"); \
}

// __INFO

#define OATPP_MACRO_API_CONTROLLER_BODY_DTO_INFO(TYPE, PARAM_LIST) \
info->body.name = OATPP_MACRO_FIRSTARG_STR PARAM_LIST; \
info->body.required = true; \
info->body.type = TYPE::Class::getType(); \
if(getContentMappers()->getDefaultMapper()) { \
  info->bodyContentType = getContentMappers()->getDefaultMapper()->getInfo().httpContentType; \
}

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
std::shared_ptr<oatpp::web::server::api::Endpoint::Info> Z__ENDPOINT_CREATE_ADDITIONAL_INFO_##NAME() { \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  Z__ENDPOINT_ADD_INFO_##NAME(info); \
  return info; \
} \
\
const std::shared_ptr<oatpp::web::server::api::Endpoint::Info> Z__ENDPOINT_ADDITIONAL_INFO_##NAME = Z__ENDPOINT_CREATE_ADDITIONAL_INFO_##NAME(); \
\
void Z__ENDPOINT_ADD_INFO_##NAME(const std::shared_ptr<oatpp::web::server::api::Endpoint::Info>& info)

// ENDPOINT MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_DEFAULTS(NAME, METHOD, PATH) \
\
template<class T> \
static typename std::shared_ptr<Handler<T>> Z__ENDPOINT_HANDLER_GET_INSTANCE_##NAME(T* controller) { \
  auto handler = std::static_pointer_cast<Handler<T>>(controller->getEndpointHandler(#NAME)); \
  if(!handler) { \
    handler = Handler<T>::createShared(controller, &T::Z__PROXY_METHOD_##NAME, nullptr); \
    controller->setEndpointHandler(#NAME, handler); \
  } \
  return handler; \
} \
\
std::shared_ptr<oatpp::web::server::api::Endpoint::Info> Z__EDNPOINT_INFO_GET_INSTANCE_##NAME() { \
  std::shared_ptr<oatpp::web::server::api::Endpoint::Info> info = getEndpointInfo(#NAME); \
  if(!info){ \
    info = oatpp::web::server::api::Endpoint::Info::createShared(); \
    setEndpointInfo(#NAME, info); \
  } \
  return info; \
}

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_0(NAME, METHOD, PATH)  \
\
EndpointInfoBuilder Z__CREATE_ENDPOINT_INFO_##NAME = [this](){ \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  info->name = #NAME; \
  info->path = ((m_routerPrefix != nullptr) ? m_routerPrefix + PATH : PATH); \
  info->method = METHOD; \
  if (info->path == "") { \
    info->path = "/"; \
  } \
  return info; \
}; \
\
const std::shared_ptr<oatpp::web::server::api::Endpoint> Z__ENDPOINT_##NAME = createEndpoint(m_endpoints, \
                                                        Z__ENDPOINT_HANDLER_GET_INSTANCE_##NAME(this), \
                                                        Z__CREATE_ENDPOINT_INFO_##NAME);

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_0(NAME, METHOD, PATH) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_DEFAULTS(NAME, METHOD, PATH) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_0(NAME, METHOD, PATH) \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__PROXY_METHOD_##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) \
{ \
  (void)__request; \
  return NAME(); \
} \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> NAME()

////////////////////

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_1(NAME, METHOD, PATH, ...)  \
\
EndpointInfoBuilder Z__CREATE_ENDPOINT_INFO_##NAME = [this](){ \
auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  info->name = #NAME; \
  info->path = ((m_routerPrefix != nullptr) ? m_routerPrefix + PATH : PATH); \
  info->method = METHOD; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_INFO, __VA_ARGS__) \
  return info; \
}; \
\
const std::shared_ptr<oatpp::web::server::api::Endpoint> Z__ENDPOINT_##NAME = createEndpoint(m_endpoints, \
                                                        Z__ENDPOINT_HANDLER_GET_INSTANCE_##NAME(this), \
                                                        Z__CREATE_ENDPOINT_INFO_##NAME);

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_1(NAME, METHOD, PATH, ...) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_DEFAULTS(NAME, METHOD, PATH) \
OATPP_MACRO_API_CONTROLLER_ENDPOINT_DECL_1(NAME, METHOD, PATH, __VA_ARGS__) \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__PROXY_METHOD_##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) \
{ \
  OATPP_MACRO_FOREACH(OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_PUT, __VA_ARGS__) \
  return NAME( \
    OATPP_MACRO_FOREACH_FIRST_AND_REST( \
      OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_CALL_FIRST, \
      OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_CALL_REST, \
      __VA_ARGS__ \
    ) \
  ); \
} \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> NAME(\
  OATPP_MACRO_FOREACH_FIRST_AND_REST( \
    OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_DECL_FIRST, \
    OATPP_MACRO_API_CONTROLLER_FOR_EACH_PARAM_DECL_REST, \
    __VA_ARGS__ \
  ) \
)

// Chooser

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_MACRO_0(METHOD, PATH, NAME) \
OATPP_MACRO_EXPAND(OATPP_MACRO_API_CONTROLLER_ENDPOINT_0(NAME, METHOD, PATH))

#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_MACRO_1(METHOD, PATH, NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_API_CONTROLLER_ENDPOINT_1(NAME, METHOD, PATH, __VA_ARGS__))

/**
 * Codegen macro to be used in `oatpp::web::server::api::ApiController` to generate Endpoint.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.).
 * @param PATH - Path to endpoint (without host).
 * @param NAME - Name of the generated method.
 * @return - std::shared_ptr to &id:oatpp::web::protocol::http::outgoing::Response;.
 */
#define ENDPOINT(METHOD, PATH, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_MACRO_API_CONTROLLER_ENDPOINT_MACRO_, (__VA_ARGS__)) (METHOD, PATH, __VA_ARGS__))

/**
 * Endpoint interceptor
 */
#define ENDPOINT_INTERCEPTOR(ENDPOINT_NAME, NAME) \
\
Handler<oatpp::web::server::api::ApiController>::Method \
  Z__INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME = Z__INTERCEPTOR_METHOD_SET_##ENDPOINT_NAME ##_ ##NAME(this); \
\
template<class T> \
Handler<oatpp::web::server::api::ApiController>::Method Z__INTERCEPTOR_METHOD_SET_##ENDPOINT_NAME ##_ ##NAME (T* controller) { \
  return static_cast<Handler<oatpp::web::server::api::ApiController>::Method>( \
    Z__ENDPOINT_HANDLER_GET_INSTANCE_##ENDPOINT_NAME(controller)->setMethod(&T::Z__PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME) \
  ); \
} \
\
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request) { \
  return Z__USER_PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME(this, request); \
} \
\
template<class T> \
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__USER_PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME( \
  T*, \
  const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request \
) { \
  auto intercepted = static_cast<typename Handler<T>::Method>(Z__INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME); \
  return Z__USER_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME <T> (intercepted, request); \
} \
\
template<class T> \
std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> \
Z__USER_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME( \
  typename Handler<T>::Method intercepted, \
  const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request \
)

// ENDPOINT ASYNC MACRO // ------------------------------------------------------

/*
 *  1 - Method to obtain endpoint call function ptr
 *  2 - Endpoint info singleton
 */
#define OATPP_MACRO_API_CONTROLLER_ENDPOINT_ASYNC_DECL_DEFAULTS(NAME, METHOD, PATH) \
\
template<class T> \
static typename std::shared_ptr<Handler<T>> Z__ENDPOINT_HANDLER_GET_INSTANCE_##NAME(T* controller) { \
  auto handler = std::static_pointer_cast<Handler<T>>(controller->getEndpointHandler(#NAME)); \
  if(!handler) { \
    handler = Handler<T>::createShared(controller, nullptr, &T::Z__PROXY_METHOD_##NAME); \
    controller->setEndpointHandler(#NAME, handler); \
  } \
  return handler; \
} \
\
std::shared_ptr<oatpp::web::server::api::Endpoint::Info> Z__EDNPOINT_INFO_GET_INSTANCE_##NAME() { \
  std::shared_ptr<oatpp::web::server::api::Endpoint::Info> info = getEndpointInfo(#NAME); \
  if(!info){ \
    info = oatpp::web::server::api::Endpoint::Info::createShared(); \
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
EndpointInfoBuilder Z__CREATE_ENDPOINT_INFO_##NAME = [this](){ \
  auto info = Z__EDNPOINT_INFO_GET_INSTANCE_##NAME(); \
  info->name = #NAME; \
  info->path = PATH; \
  info->method = METHOD; \
  return info; \
}; \
\
const std::shared_ptr<oatpp::web::server::api::Endpoint> Z__ENDPOINT_##NAME = createEndpoint(m_endpoints, \
                                                                    Z__ENDPOINT_HANDLER_GET_INSTANCE_##NAME(this), \
                                                                    Z__CREATE_ENDPOINT_INFO_##NAME);

/**
 * Codegen macro to be used in `oatpp::web::server::api::ApiController` to generate Asynchronous Endpoint.
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


/**
 * Endpoint interceptor
 */
#define ENDPOINT_INTERCEPTOR_ASYNC(ENDPOINT_NAME, NAME) \
\
Handler<oatpp::web::server::api::ApiController>::MethodAsync \
  Z__INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME = Z__INTERCEPTOR_METHOD_SET_##ENDPOINT_NAME ##_ ##NAME(this); \
\
template<class T> \
Handler<oatpp::web::server::api::ApiController>::MethodAsync Z__INTERCEPTOR_METHOD_SET_##ENDPOINT_NAME ##_ ##NAME (T* controller) { \
  return static_cast<Handler<oatpp::web::server::api::ApiController>::MethodAsync>( \
    Z__ENDPOINT_HANDLER_GET_INSTANCE_##ENDPOINT_NAME(controller)->setMethodAsync(&T::Z__PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME) \
  ); \
} \
\
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&> \
Z__PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request) { \
  return Z__USER_PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME(this, request); \
} \
\
template<class T> \
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&> \
Z__USER_PROXY_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME( \
  T*, \
  const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request \
) { \
  auto intercepted = static_cast<typename Handler<T>::MethodAsync>(Z__INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME); \
  return Z__USER_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME <T> (intercepted, request); \
} \
\
template<class T> \
oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&> \
Z__USER_INTERCEPTOR_METHOD_##ENDPOINT_NAME ##_ ##NAME( \
  typename Handler<T>::MethodAsync intercepted, \
  const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request \
)
