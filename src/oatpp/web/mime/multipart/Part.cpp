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

#include "Part.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/parser/Caret.hpp"

#include <cstring>

namespace oatpp { namespace web { namespace mime { namespace multipart {

Part::Part(const Headers &headers, const std::shared_ptr<data::resource::Resource>& payload)
  : m_headers(headers)
  , m_payload(payload)
{

  auto contentDisposition = m_headers.getAsMemoryLabel<oatpp::data::share::StringKeyLabel>("Content-Disposition");
  if(contentDisposition) {

    oatpp::web::protocol::http::HeaderValueData valueData;
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, contentDisposition, ';');

    m_name = valueData.getTitleParamValue("name");
    m_filename = valueData.getTitleParamValue("filename");

  }

}

void Part::setPayload(const std::shared_ptr<data::resource::Resource>& payload) {
  m_payload = payload;
}

std::shared_ptr<data::resource::Resource> Part::getPayload() {
  return m_payload;
}

oatpp::String Part::getName() const {
  return m_name;
}


oatpp::String Part::getFilename() const {
  return m_filename;
}


const Part::Headers& Part::getHeaders() const {
  return m_headers;
}

oatpp::String Part::getHeader(const oatpp::data::share::StringKeyLabelCI& headerName) const {
  return m_headers.get(headerName);
}

void Part::putHeader(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value) {
  m_headers.put(key, value);
}

bool Part::putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value) {
  return m_headers.putIfNotExists(key, value);
}

void Part::setTag(const char* tagName, const std::shared_ptr<oatpp::base::Countable>& tagObject) {
  m_tagName = tagName;
  m_tagObject = tagObject;
}

const char* Part::getTagName() {
  return m_tagName;
}

std::shared_ptr<oatpp::base::Countable> Part::getTagObject() {
  return m_tagObject;
}

void Part::clearTag() {
  m_tagName = nullptr;
  m_tagObject.reset();
}

}}}}
