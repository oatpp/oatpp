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
#include "oatpp/utils/Random.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

Multipart::Multipart(const oatpp::String& boundary)
  : m_boundary(boundary)
{}

oatpp::String Multipart::getBoundary() {
  return m_boundary;
}

std::shared_ptr<Part> Multipart::readNextPartSimple() {
  async::Action action;
  auto result = readNextPart(action);
  if(!action.isNone()) {
    throw std::runtime_error("[oatpp::web::mime::multipart::Multipart::readNextPartSimple()]. Error."
                             "Async method is called for non-async API.");
  }
  return result;
}

void Multipart::writeNextPartSimple(const std::shared_ptr<Part>& part) {
  async::Action action;
  writeNextPart(part, action);
  if(!action.isNone()) {
    throw std::runtime_error("[oatpp::web::mime::multipart::Multipart::writeNextPartSimple()]. Error."
                             "Async method is called for non-async API.");
  }
}

oatpp::String Multipart::generateRandomBoundary(v_int32 boundarySize) {
  std::unique_ptr<v_char8[]> buffer(new v_char8[static_cast<unsigned long>(boundarySize)]);
  utils::Random::randomBytes(buffer.get(), boundarySize);
  return encoding::Base64::encode(buffer.get(), boundarySize, encoding::Base64::ALPHABET_BASE64_URL_SAFE);
}

oatpp::String Multipart::parseBoundaryFromHeaders(const Headers& requestHeaders) {

  oatpp::String boundary;
  auto contentType = requestHeaders.getAsMemoryLabel<oatpp::data::share::StringKeyLabel>("Content-Type");

  if(contentType) {
    oatpp::web::protocol::http::HeaderValueData valueData;
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, contentType, ';');
    boundary = valueData.getTitleParamValue("boundary");
  }

  return boundary;

}

}}}}
