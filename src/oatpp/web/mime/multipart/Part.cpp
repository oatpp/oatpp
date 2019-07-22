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

Part::Part(const Headers &headers,
           const std::shared_ptr<data::stream::InputStream> &inputStream,
           const oatpp::String inMemoryData,
           data::v_io_size knownSize)
  : m_headers(headers)
  , m_inputStream(inputStream)
  , m_inMemoryData(inMemoryData)
  , m_knownSize(knownSize)
{

  auto it = m_headers.find("Content-Disposition");
  if(it != m_headers.end()) {

    oatpp::web::protocol::http::HeaderValueData valueData;
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, it->second, ';');

    m_name = valueData.getTitleParamValue("name");
    m_filename = valueData.getTitleParamValue("filename");

  }

}


Part::Part(const Headers& headers) : Part(headers, nullptr, nullptr, -1) {}

void Part::setDataInfo(const std::shared_ptr<data::stream::InputStream>& inputStream,
                       const oatpp::String inMemoryData,
                       data::v_io_size knownSize)
{
  m_inputStream = inputStream;
  m_inMemoryData = inMemoryData;
  m_knownSize = knownSize;
}

void Part::setDataInfo(const std::shared_ptr<data::stream::InputStream>& inputStream) {
  m_inputStream = inputStream;
  m_inMemoryData = nullptr;
  m_knownSize = -1;
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

oatpp::String Part::getHeader(const oatpp::data::share::StringKeyLabelCI_FAST &headerName) const {
  auto it = m_headers.find(headerName);
  if(it != m_headers.end()) {
    return it->second.toString();
  }
  return nullptr;
}

void Part::putHeader(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  m_headers[key] = value;
}

bool Part::putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  auto it = m_headers.find(key);
  if(it == m_headers.end()) {
    m_headers.insert({key, value});
    return true;
  }
  return false;
}

std::shared_ptr<data::stream::InputStream> Part::getInputStream() const {
  return m_inputStream;
}

oatpp::String Part::getInMemoryData() const {
  return m_inMemoryData;
}

data::v_io_size Part::getKnownSize() const {
  return m_knownSize;
}

}}}}
