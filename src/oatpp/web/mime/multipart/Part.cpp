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

#include "oatpp/core/parser/Caret.hpp"

#include <cstring>

namespace oatpp { namespace web { namespace mime { namespace multipart {

oatpp::String Part::parseContentDispositionValue(const char* key, p_char8 data, v_int32 size) {

  parser::Caret caret(data, size);

  if(caret.findText(key)) {

    caret.inc(std::strlen(key));

    parser::Caret::Label label(nullptr);

    if(caret.isAtChar('"')) {
      label = caret.parseStringEnclosed('"', '"', '\\');
    } else if(caret.isAtChar('\'')) {
      label = caret.parseStringEnclosed('\'', '\'', '\\');
    } else {
      label = caret.putLabel();
      caret.findCharFromSet(" \t\n\r\f");
      label.end();
    }

    if(label) {

      return label.toString();

    } else {
      throw std::runtime_error("[oatpp::web::mime::multipart::Part::parseContentDispositionValue()]: Error. Can't parse value.");
    }

  }

  return nullptr;

}

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

    m_name = parseContentDispositionValue("name=", it->second.getData(), it->second.getSize());

    if(!m_name) {
      throw std::runtime_error("[oatpp::web::mime::multipart::Part::Part()]: Error. Part name is missing in 'Content-Disposition' header.");
    }

    m_filename = parseContentDispositionValue("filename=", it->second.getData(), it->second.getSize());

  } else {
    throw std::runtime_error("[oatpp::web::mime::multipart::Part::Part()]: Error. Missing 'Content-Disposition' header.");
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
