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

#define AUTHORIZATION(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_AUTHORIZATION, OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_INFO, TYPE, (__VA_ARGS__))

// AUTHORIZATION MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_1(TYPE, NAME) \
auto __param_str_val_##NAME = __request->getHeader(oatpp::web::protocol::http::Header::AUTHORIZATION); \
std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> __param_aosp_val_##NAME = ApiController::handleDefaultAuthorization(__param_str_val_##NAME); \
TYPE NAME = std::static_pointer_cast<TYPE::element_type>(__param_aosp_val_##NAME);

#define OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_2(TYPE, NAME, AUTH_HANDLER) \
auto __param_str_val_##NAME = __request->getHeader(oatpp::web::protocol::http::Header::AUTHORIZATION); \
std::shared_ptr<oatpp::web::server::handler::AuthorizationHandler> __auth_handler_##NAME = AUTH_HANDLER; \
std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> __param_aosp_val_##NAME = __auth_handler_##NAME->handleAuthorization(__param_str_val_##NAME); \
TYPE NAME = std::static_pointer_cast<TYPE::element_type>(__param_aosp_val_##NAME);

#define OATPP_MACRO_API_CONTROLLER_AUTHORIZATION(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// __INFO

#define OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_INFO_1(TYPE, NAME) \
auto __param_obj_##NAME = ApiController::getDefaultAuthorizationHandler(); \
if(__param_obj_##NAME) { \
  info->headers.add(oatpp::web::protocol::http::Header::AUTHORIZATION, oatpp::String::Class::getType()); \
  info->headers[oatpp::web::protocol::http::Header::AUTHORIZATION].description = __param_obj_##NAME ->getScheme(); \
  info->authorization = __param_obj_##NAME ->getScheme(); \
} else { \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "No authorization handler set up in controller before controller was added to router or swagger-doc."); \
}

#define OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_INFO_2(TYPE, NAME, AUTH_HANDLER) \
std::shared_ptr<oatpp::web::server::handler::AuthorizationHandler> __auth_handler_##NAME = AUTH_HANDLER; \
if(__auth_handler_##NAME) { \
  info->headers.add(oatpp::web::protocol::http::Header::AUTHORIZATION, oatpp::String::Class::getType()); \
  info->headers[oatpp::web::protocol::http::Header::AUTHORIZATION].description = __auth_handler_##NAME->getScheme(); \
  info->authorization = __auth_handler_##NAME->getScheme(); \
} else { \
  throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Invalid authorization handler given (or not set up) in AUTHORIZATION(TYPE, NAME, AUTH_HANDLER) before controller was added to router or swagger-doc."); \
}

#define OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_INFO(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_AUTHORIZATION_INFO_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)
