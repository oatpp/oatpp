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

#include "Multipart.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/encoding/Base64.hpp"
#include "oatpp/core/utils/Random.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {


Multipart::Multipart(const oatpp::String& boundary)
  : m_boundary(boundary)
{}


Multipart::Multipart(const Headers& requestHeaders){

  auto it = requestHeaders.find("Content-Type");
  if(it != requestHeaders.end()) {

    oatpp::web::protocol::http::HeaderValueData valueData;
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, it->second, ';');

    m_boundary = valueData.getTitleParamValue("boundary");

    if(!m_boundary) {
      throw std::runtime_error("[oatpp::web::mime::multipart::Multipart::Multipart()]: Error. Boundary not defined.");
    }

  } else {
    throw std::runtime_error("[oatpp::web::mime::multipart::Multipart::Multipart()]: Error. 'Content-Type' header is missing.");
  }

}

std::shared_ptr<Multipart> Multipart::createSharedWithRandomBoundary(v_int32 boundarySize) {
  auto boundary = generateRandomBoundary(boundarySize);
  return std::make_shared<Multipart>(boundary);
}

oatpp::String Multipart::getBoundary() {
  return m_boundary;
}

void Multipart::addPart(const std::shared_ptr<Part>& part) {

  if(part->getName()) {
    auto it = m_namedParts.find(part->getName());
    if(it != m_namedParts.end()) {
      throw std::runtime_error("[oatpp::web::mime::multipart::Multipart::addPart()]: Error. Part with such name already exists.");
    }
    m_namedParts.insert({part->getName(), part});
  }

  m_parts.push_back(part);

}

std::shared_ptr<Part> Multipart::getNamedPart(const oatpp::String& name) {

  auto it = m_namedParts.find(name);
  if(it != m_namedParts.end()) {
    return it->second;
  }

  return nullptr;

}

const std::list<std::shared_ptr<Part>>& Multipart::getAllParts() {
  return m_parts;
}

v_int32 Multipart::count() {
  return m_parts.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions

oatpp::String generateRandomBoundary(v_int32 boundarySize) {
  v_char8 buffer[boundarySize];
  utils::random::Random::randomBytes(buffer, boundarySize);
  return encoding::Base64::encode(buffer, boundarySize, encoding::Base64::ALPHABET_BASE64_URL_SAFE);
}

}}}}