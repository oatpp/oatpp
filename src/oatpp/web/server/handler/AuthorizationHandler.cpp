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

#include "AuthorizationHandler.hpp"

#include "oatpp/encoding/Base64.hpp"
#include "oatpp/core/parser/Caret.hpp"

namespace oatpp { namespace web { namespace server { namespace handler {

AuthorizationHandler::AuthorizationHandler(const oatpp::String& scheme, const oatpp::String& realm)
  : m_scheme(scheme)
  , m_realm(realm)
{}

void AuthorizationHandler::renderAuthenticateHeaderValue(BufferOutputStream& stream) {
  stream << m_scheme << " " << "realm=\"" << m_realm << "\"";
}

void AuthorizationHandler::addErrorResponseHeaders(Headers& headers) {
  BufferOutputStream stream;
  renderAuthenticateHeaderValue(stream);
  headers.put_LockFree(protocol::http::Header::WWW_AUTHENTICATE, stream.toString());
}

oatpp::String AuthorizationHandler::getScheme() {
  return m_scheme;
}

oatpp::String AuthorizationHandler::getRealm() {
  return m_realm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BasicAuthorizationHandler

BasicAuthorizationHandler::BasicAuthorizationHandler(const oatpp::String& realm)
  : AuthorizationHandler("Basic", realm)
{}

std::shared_ptr<handler::AuthorizationObject> BasicAuthorizationHandler::handleAuthorization(const oatpp::String &header) {

  if(header && header->size() > 6 && utils::String::compare(header->data(), 6, "Basic ", 6) == 0) {

    oatpp::String auth = oatpp::encoding::Base64::decode(header->c_str() + 6, header->size() - 6);
    parser::Caret caret(auth);

    if (caret.findChar(':')) {
      oatpp::String userId((const char *) &caret.getData()[0], caret.getPosition());
      oatpp::String password((const char *) &caret.getData()[caret.getPosition() + 1],
                             caret.getDataSize() - caret.getPosition() - 1);
      auto authResult = authorize(userId, password);
      if(authResult) {
        return authResult;
      }

      Headers responseHeaders;
      addErrorResponseHeaders(responseHeaders);
      throw protocol::http::HttpError(protocol::http::Status::CODE_401, "Unauthorized", responseHeaders);

    }

  }

  Headers responseHeaders;
  addErrorResponseHeaders(responseHeaders);
  throw protocol::http::HttpError(protocol::http::Status::CODE_401, "Authorization Required", responseHeaders);

}

std::shared_ptr<AuthorizationObject> BasicAuthorizationHandler::authorize(const oatpp::String &userId,
                                                                          const oatpp::String &password)
{
  auto authorizationObject = std::make_shared<DefaultBasicAuthorizationObject>();
  authorizationObject->userId = userId;
  authorizationObject->password = password;
  return authorizationObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BearerAuthorizationHandler

BearerAuthorizationHandler::BearerAuthorizationHandler(const oatpp::String& realm)
  : AuthorizationHandler("Bearer", realm)
{}

std::shared_ptr<AuthorizationObject> BearerAuthorizationHandler::handleAuthorization(const oatpp::String &header) {

  if(header && header->size() > 7 && utils::String::compare(header->data(), 7, "Bearer ", 7) == 0) {

    oatpp::String token = oatpp::String(header->c_str() + 7, header->size() - 7);

    auto authResult = authorize(token);
    if(authResult) {
      return authResult;
    }

    Headers responseHeaders;
    addErrorResponseHeaders(responseHeaders);
    throw protocol::http::HttpError(protocol::http::Status::CODE_401, "Unauthorized", responseHeaders);

  }

  Headers responseHeaders;
  addErrorResponseHeaders(responseHeaders);
  throw protocol::http::HttpError(protocol::http::Status::CODE_401, "Authorization Required", responseHeaders);

}

std::shared_ptr<AuthorizationObject> BearerAuthorizationHandler::authorize(const oatpp::String& token) {
  auto authObject = std::make_shared<DefaultBearerAuthorizationObject>();
  authObject->token = token;
  return authObject;
}

}}}}