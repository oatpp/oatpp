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

#include "StatefulParser.hpp"

#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/parser/ParsingError.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StatefulParser

StatefulParser::StatefulParser(const oatpp::String& boundary, const std::shared_ptr<Listener>& listener)
  : m_state(STATE_BOUNDARY)
  , m_currPartIndex(0)
  , m_currBoundaryCharIndex(0)
  , m_checkForBoundary(true)
  , m_finishingBoundary(false)
  , m_readingBody(false)
  , m_headerSectionEndAccumulator(0)
  , m_firstBoundarySample("--" + boundary)
  , m_nextBoundarySample("\r\n--" + boundary)
  , m_maxPartHeadersSize(4092)
  , m_listener(listener)
{}

void StatefulParser::onPartHeaders(const Headers& partHeaders) {

  m_currPartIndex ++;

  auto it = partHeaders.find("Content-Disposition");
  if(it != partHeaders.end()) {

    if(m_listener) {
      m_listener->onPartHeaders(partHeaders);
    }

  } else {
    throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::onPartHeaders()]: Error. Missing 'Content-Disposition' header.");
  }

}

void StatefulParser::onPartData(p_char8 data, v_int32 size) {

  if(m_listener) {
    m_listener->onPartData(data, size);
  }

}

v_int32 StatefulParser::parseNext_Boundary(p_char8 data, v_int32 size) {

  p_char8 sampleData = m_nextBoundarySample->getData();
  v_int32 sampleSize = m_nextBoundarySample->getSize();

  if (m_currPartIndex == 0) {
    sampleData = m_firstBoundarySample->getData();
    sampleSize = m_firstBoundarySample->getSize();
  } else {
    sampleData = m_nextBoundarySample->getData();
    sampleSize = m_nextBoundarySample->getSize();
  }

  v_int32 checkSize = sampleSize - m_currBoundaryCharIndex;
  if(checkSize > size) {
    checkSize = size;
  }

  parser::Caret caret(data, size);

  if(caret.isAtText(&sampleData[m_currBoundaryCharIndex], checkSize, true)) {

    m_currBoundaryCharIndex += caret.getPosition();

    if(m_currBoundaryCharIndex == sampleSize) {
      m_state = STATE_AFTER_BOUNDARY;
      m_currBoundaryCharIndex = 0;
      m_readingBody = false;
      if(m_currPartIndex > 0) {
        onPartData(nullptr, 0);
      }
    }

    return caret.getPosition();

  } else if(m_readingBody) {

    if(m_currBoundaryCharIndex > 0) {
      onPartData(sampleData, m_currBoundaryCharIndex);
    } else {
      m_checkForBoundary = false;
    }

    m_state = STATE_DATA;
    m_currBoundaryCharIndex = 0;

    return 0;

  }

  throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_Boundary()]: Error. Invalid state.");

}

v_int32 StatefulParser::parseNext_AfterBoundary(p_char8 data, v_int32 size) {

  if(m_currBoundaryCharIndex == 0) {

    if(data[0] == '-') {
      m_finishingBoundary = true;
    } else if(data[0] != '\r') {
      throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_AfterBoundary()]: Error. Invalid char.");
    }

  }

  if(size > 1 || m_currBoundaryCharIndex == 1) {

    if (m_finishingBoundary && data[1 - m_currBoundaryCharIndex] == '-') {
      auto result = 2 - m_currBoundaryCharIndex;
      m_state = STATE_DONE;
      m_currBoundaryCharIndex = 0;
      return result;
    } else if (!m_finishingBoundary && data[1 - m_currBoundaryCharIndex] == '\n') {
      auto result = 2 - m_currBoundaryCharIndex;
      m_state = STATE_HEADERS;
      m_currBoundaryCharIndex = 0;
      m_headerSectionEndAccumulator = 0;
      return result;
    } else {
      throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_AfterBoundary()]: Error. Invalid trailing char.");
    }

  }

  m_currBoundaryCharIndex = 1;
  return 1;

}

v_int32 StatefulParser::parseNext_Headers(p_char8 data, v_int32 size) {

  for(v_int32 i = 0; i < size; i ++) {

    m_headerSectionEndAccumulator <<= 8;
    m_headerSectionEndAccumulator |= data[i];

    if(m_headerSectionEndAccumulator == HEADERS_SECTION_END) {

      if(m_headersBuffer.getSize() + i > m_maxPartHeadersSize) {
        throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_Headers()]: Error. Too large heades.");
      }

      m_headersBuffer.write(data, i);

      auto headersText = m_headersBuffer.toString();
      m_headersBuffer.clear();

      protocol::http::Status status;
      parser::Caret caret(headersText);
      Headers headers;

      protocol::http::Parser::parseHeaders(headers, headersText.getPtr(), caret, status);

      onPartHeaders(headers);

      m_state = STATE_DATA;
      m_checkForBoundary = true;

      return i + 1;

    }

  }

  if(m_headersBuffer.getSize() + size > m_maxPartHeadersSize) {
    throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_Headers()]: Error. Too large heades.");
  }

  m_headersBuffer.write(data, size);

  return size;
}

v_int32 StatefulParser::parseNext_Data(p_char8 data, v_int32 size) {

  parser::Caret caret(data, size);

  bool rFound = caret.findChar('\r');
  if(rFound && !m_checkForBoundary) {
    caret.inc();
    rFound = caret.findChar('\r');
  }

  m_checkForBoundary = true;

  if(rFound) {
    if(caret.getPosition() > 0) {
      onPartData(data, caret.getPosition());
    }
    m_state = STATE_BOUNDARY;
    m_readingBody = true;
    return caret.getPosition();
  } else {
    onPartData(data, size);
  }

  return size;
}

v_int32 StatefulParser::parseNext(p_char8 data, v_int32 size) {

  v_int32 pos = 0;

  while(pos < size) {

    switch (m_state) {
      case STATE_BOUNDARY:
        pos += parseNext_Boundary(&data[pos], size - pos);
        break;
      case STATE_AFTER_BOUNDARY:
        pos += parseNext_AfterBoundary(&data[pos], size - pos);
        break;
      case STATE_HEADERS:
        pos += parseNext_Headers(&data[pos], size - pos);
        break;
      case STATE_DATA:
        pos += parseNext_Data(&data[pos], size - pos);
        break;
      case STATE_DONE:
        return pos;
      default:
        throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext()]: Error. Invalid state.");
    }

  }

  return pos;

}

bool StatefulParser::finished() {
  return m_state == STATE_DONE;
}

}}}}