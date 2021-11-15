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
// StatefulParser::ListenerCall

void StatefulParser::ListenerCall::setOnHeadersCall() {
  callType = CALL_ON_HEADERS;
  data = nullptr;
  size = 0;
}

void StatefulParser::ListenerCall::setOnDataCall(const char* pData, v_buff_size pSize) {
  callType = CALL_ON_DATA;
  data = pData;
  size = pSize;
}

void StatefulParser::ListenerCall::call(StatefulParser* parser) {

  if(parser->m_listener) {

    switch(callType) {

      case CALL_ON_HEADERS:
        {
          Headers headers;
          parser->parseHeaders(headers);
          parser->m_listener->onPartHeaders(headers);
        }
        break;

      case CALL_ON_DATA:
        parser->m_listener->onPartData(data, size);
        break;

    }

  }

}

async::CoroutineStarter StatefulParser::ListenerCall::callAsync(StatefulParser* parser) {

  if(parser->m_asyncListener) {

    switch(callType) {

      case CALL_ON_HEADERS: {
        Headers headers;
        parser->parseHeaders(headers);
        return parser->m_asyncListener->onPartHeadersAsync(headers);
      }

      case CALL_ON_DATA:
        return parser->m_asyncListener->onPartDataAsync(data, size);

    }

  }

  return nullptr;

}

StatefulParser::ListenerCall::operator bool() const {
  return callType != CALL_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StatefulParser

StatefulParser::StatefulParser(const oatpp::String& boundary,
                               const std::shared_ptr<Listener>& listener,
                               const std::shared_ptr<AsyncListener>& asyncListener)
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
  , m_asyncListener(asyncListener)
{}

void StatefulParser::parseHeaders(Headers& headers) {

  m_currPartIndex ++;

  auto headersText = m_headersBuffer.toString();
  m_headersBuffer.setCurrentPosition(0);

  protocol::http::Status status;
  parser::Caret caret(headersText);

  protocol::http::Parser::parseHeaders(headers, headersText.getPtr(), caret, status);

}

StatefulParser::ListenerCall StatefulParser::parseNext_Boundary(data::buffer::InlineWriteData& inlineData) {

  ListenerCall result;
  auto data = inlineData.currBufferPtr;
  auto size = inlineData.bytesLeft;

  auto sampleData = m_nextBoundarySample->data();
  v_io_size sampleSize = m_nextBoundarySample->size();

  if (m_currPartIndex == 0) {
    sampleData = m_firstBoundarySample->data();
    sampleSize = m_firstBoundarySample->size();
  } else {
    sampleData = m_nextBoundarySample->data();
    sampleSize = m_nextBoundarySample->size();
  }

  v_io_size checkSize = sampleSize - m_currBoundaryCharIndex;
  if(checkSize > size) {
    checkSize = size;
  }

  parser::Caret caret((const char*)data, size);

  if(caret.isAtText(&sampleData[m_currBoundaryCharIndex], checkSize, true)) {

    m_currBoundaryCharIndex += caret.getPosition();

    if(m_currBoundaryCharIndex == sampleSize) {
      m_state = STATE_AFTER_BOUNDARY;
      m_currBoundaryCharIndex = 0;
      m_readingBody = false;
      if(m_currPartIndex > 0) {
        result.setOnDataCall(nullptr, 0);
      }
    }

    inlineData.inc(caret.getPosition());
    return result;

  } else if(m_readingBody) {

    if(m_currBoundaryCharIndex > 0) {
      result.setOnDataCall(sampleData, m_currBoundaryCharIndex);
    } else {
      m_checkForBoundary = false;
    }

    m_state = STATE_DATA;
    m_currBoundaryCharIndex = 0;

    return result;

  }

  throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_Boundary()]: Error. Invalid state.");

}

void StatefulParser::parseNext_AfterBoundary(data::buffer::InlineWriteData& inlineData) {

  p_char8 data = (p_char8) inlineData.currBufferPtr;
  auto size = inlineData.bytesLeft;

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
      inlineData.inc(result);
      return;
    } else if (!m_finishingBoundary && data[1 - m_currBoundaryCharIndex] == '\n') {
      auto result = 2 - m_currBoundaryCharIndex;
      m_state = STATE_HEADERS;
      m_currBoundaryCharIndex = 0;
      m_headerSectionEndAccumulator = 0;
      inlineData.inc(result);
      return;
    } else {
      throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_AfterBoundary()]: Error. Invalid trailing char.");
    }

  }

  m_currBoundaryCharIndex = 1;
  inlineData.inc(1);
  return;

}

StatefulParser::ListenerCall StatefulParser::parseNext_Headers(data::buffer::InlineWriteData& inlineData) {

  ListenerCall result;

  p_char8 data = (p_char8) inlineData.currBufferPtr;
  auto size = inlineData.bytesLeft;

  for(v_buff_size i = 0; i < size; i ++) {

    m_headerSectionEndAccumulator <<= 8;
    m_headerSectionEndAccumulator |= data[i];

    if(m_headerSectionEndAccumulator == HEADERS_SECTION_END) {

      if(m_headersBuffer.getCurrentPosition() + i > m_maxPartHeadersSize) {
        throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_Headers()]: Error. Too large heades.");
      }

      m_headersBuffer.writeSimple(data, i);

      result.setOnHeadersCall();

      m_state = STATE_DATA;
      m_checkForBoundary = true;

      inlineData.inc(i + 1);
      return result;

    }

  }

  if(m_headersBuffer.getCurrentPosition() + size > m_maxPartHeadersSize) {
    throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext_Headers()]: Error. Headers section is too large.");
  }

  m_headersBuffer.writeSimple(data, size);

  inlineData.inc(size);

  return result;

}

StatefulParser::ListenerCall StatefulParser::parseNext_Data(data::buffer::InlineWriteData& inlineData) {

  ListenerCall result;

  const char* data = (const char*) inlineData.currBufferPtr;
  auto size = inlineData.bytesLeft;

  parser::Caret caret(data, size);

  bool rFound = caret.findChar('\r');
  if(rFound && !m_checkForBoundary) {
    caret.inc();
    rFound = caret.findChar('\r');
  }

  m_checkForBoundary = true;

  if(rFound) {
    if(caret.getPosition() > 0) {
      result.setOnDataCall(data, caret.getPosition());
    }
    m_state = STATE_BOUNDARY;
    m_readingBody = true;
    inlineData.inc(caret.getPosition());
  } else {
    result.setOnDataCall(data, size);
    inlineData.inc(size);
  }

  return result;

}

void StatefulParser::parseNext(data::buffer::InlineWriteData& inlineData, async::Action& action) {

  while(inlineData.bytesLeft > 0) {

    ListenerCall listenerCall;

    switch (m_state) {
      case STATE_BOUNDARY:
        listenerCall = parseNext_Boundary(inlineData);
        break;
      case STATE_AFTER_BOUNDARY:
        parseNext_AfterBoundary(inlineData);
        break;
      case STATE_HEADERS:
        listenerCall = parseNext_Headers(inlineData);
        break;
      case STATE_DATA:
        listenerCall = parseNext_Data(inlineData);
        break;
      case STATE_DONE:
        return;
      default:
        throw std::runtime_error("[oatpp::web::mime::multipart::StatefulParser::parseNext()]: Error. Invalid state.");
    }

    if(listenerCall) {
      if(m_asyncListener) {
        action = listenerCall.callAsync(this).next(async::Action::createActionByType(async::Action::TYPE_REPEAT));
        break;
      }else {
        listenerCall.call(this);
      }
    }

  }

}

bool StatefulParser::finished() {
  return m_state == STATE_DONE;
}

}}}}