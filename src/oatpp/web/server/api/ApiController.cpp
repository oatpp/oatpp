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

#include "ApiController.hpp"

#include "oatpp/web/server/handler/ErrorHandler.hpp"

namespace oatpp { namespace web { namespace server { namespace api {

const Endpoints& ApiController::getEndpoints() {
  return m_endpoints;
}

void ApiController::setEndpointInfo(const std::string& endpointName, const std::shared_ptr<Endpoint::Info>& info){
  m_endpointInfo[endpointName] = info;
}

std::shared_ptr<Endpoint::Info> ApiController::getEndpointInfo(const std::string& endpointName) {
  return m_endpointInfo[endpointName];
}

void ApiController::setEndpointHandler(const std::string& endpointName, const std::shared_ptr<RequestHandler>& handler) {
  m_endpointHandlers[endpointName] = handler;
}

std::shared_ptr<ApiController::RequestHandler> ApiController::getEndpointHandler(const std::string& endpointName) {
  return m_endpointHandlers[endpointName];
}

void ApiController::setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
  m_errorHandler = errorHandler;
  if(!m_errorHandler) {
    m_errorHandler = std::make_shared<handler::DefaultErrorHandler>();
  }
}

std::shared_ptr<ApiController::OutgoingResponse> ApiController::handleError(const std::exception_ptr& exceptionPtr) const {

  if(m_errorHandler) {
    return m_errorHandler->handleError(exceptionPtr);
  }

  if(exceptionPtr) {
    std::rethrow_exception(exceptionPtr);
  }

  throw std::runtime_error("[oatpp::web::server::api::ApiController::handleError()]: Error. 'exceptionPtr' is not set.");

}

void ApiController::setDefaultAuthorizationHandler(const std::shared_ptr<handler::AuthorizationHandler>& authorizationHandler){
  m_defaultAuthorizationHandler = authorizationHandler;
}

std::shared_ptr<handler::AuthorizationHandler> ApiController::getDefaultAuthorizationHandler() {
  return m_defaultAuthorizationHandler;
}

std::shared_ptr<handler::AuthorizationObject> ApiController::handleDefaultAuthorization(const String &authHeader) const {
  if(m_defaultAuthorizationHandler) {
    return m_defaultAuthorizationHandler->handleAuthorization(authHeader);
  }
  // If Authorization is not setup on the server then it's 500
  throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Authorization is not setup.", {});
}

const std::shared_ptr<mime::ContentMappers>& ApiController::getContentMappers() const {
  return m_contentMappers;
}

// Helper methods

std::shared_ptr<ApiController::OutgoingResponse> ApiController::createResponse(const Status& status, const oatpp::String& str) const {
  return ResponseFactory::createResponse(status, str);
}

std::shared_ptr<ApiController::OutgoingResponse> ApiController::createResponse(const ApiController::Status &status) const {
  return ResponseFactory::createResponse(status);
}

std::shared_ptr<ApiController::OutgoingResponse> ApiController::createDtoResponse(const Status& status,
                                                                                  const oatpp::Void& dto,
                                                                                  const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
  return ResponseFactory::createResponse(status, dto, objectMapper);
}

std::shared_ptr<ApiController::OutgoingResponse> ApiController::createDtoResponse(const Status& status, const oatpp::Void& dto) const {
  return ResponseFactory::createResponse(status, dto, m_contentMappers->getDefaultMapper());
}

std::shared_ptr<ApiController::OutgoingResponse> ApiController::createDtoResponse(const Status& status,
                                                                                  const oatpp::Void& dto,
                                                                                  const std::vector<oatpp::String>& acceptableContentTypes) const
{
  auto mapper = m_contentMappers->selectMapper(acceptableContentTypes);
  if(mapper) {
    return ResponseFactory::createResponse(status, dto, mapper);
  }
  throw std::runtime_error("[ApiController::createDtoResponse()]: Unsupported content-type");
}

}}}}
