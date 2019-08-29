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

#include <oatpp/encoding/Base64.hpp>
#include "AuthorizationHandler.hpp"

namespace oatpp { namespace web { namespace server { namespace handler {

std::shared_ptr<handler::AuthorizationObject>
DefaultAuthorizationHandler::handleAuthorization(const oatpp::String &header) {
  return defaultAuthorizationObject(header);
}

std::shared_ptr<handler::AuthorizationObject> DefaultAuthorizationHandler::defaultAuthorizationObject(const oatpp::String &header) {
  if(!header->startsWith("Basic ")) {
    return nullptr;
  }

  oatpp::String auth = oatpp::encoding::Base64::decode(header->c_str()+6, header->getSize() - 6);
  const char *pauth = auth->c_str();
  const char *delim = (char*)memchr(pauth, ':', auth->getSize());
  if(delim == nullptr) {
    return nullptr;
  }
  size_t delimPos = delim - pauth;
  std::unique_ptr<v_char8[]> dauth(new v_char8[auth->getSize()+1]);
  memset(dauth.get(), 0, auth->getSize()+1);
  memcpy(dauth.get(), auth->c_str(), auth->getSize());
  dauth.get()[delimPos] = 0;

  auto dto = std::make_shared<handler::DefaultAuthorizationObject>();

  dto->user = (const char*)&dauth[0];
  dto->password = (const char*)&dauth[delimPos + 1];

  return dto;
}

}}}}