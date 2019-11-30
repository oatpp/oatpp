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

#include "Beautifier.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace parser { namespace json {

Beautifier::Beautifier(ConsistentOutputStream* outputStream, const oatpp::String& indent, const oatpp::String& newLine)
  : m_outputStream(outputStream)
  , m_indent(indent)
  , m_newLine(newLine)
  , m_level(0)
  , m_wantIndent(false)
  , m_isCharEscaped(false)
  , m_isInString(false)
{}

void Beautifier::writeIndent(ConsistentOutputStream* outputStream) {
  outputStream->write(m_newLine->getData(), m_newLine->getSize());
  for(v_int32 i = 0; i < m_level; i ++ ) {
    outputStream->write(m_indent->getData(), m_indent->getSize());
  }
}

data::v_io_size Beautifier::write(const void *data, v_buff_size count) {

  oatpp::data::stream::BufferOutputStream buffer;

  for(v_buff_size i = 0; i < count; i ++) {

    v_char8 c = ((p_char8) data) [i];

    if(m_isCharEscaped) {
      m_isCharEscaped = false;
      buffer.writeChar(c);
      continue;
    }

    switch(c) {

      case '\\': {
        m_isCharEscaped = true;
        buffer.writeChar('\\');
        break;
      }

      case '{': {
        if(m_wantIndent) {
          writeIndent(&buffer);
        }
        m_level ++;
        m_wantIndent = true;
        buffer.writeChar('{');
        break;
      }

      case '}': {
        m_level --;
        if(!m_wantIndent) {
          writeIndent(&buffer);
        }
        buffer.writeChar('}');
        m_wantIndent = false;
        break;
      }

      case '[': {
        if(m_wantIndent) {
          writeIndent(&buffer);
        }
        m_level ++;
        m_wantIndent = true;
        buffer.writeChar('[');
        break;
      }

      case ']': {
        m_level --;
        if(!m_wantIndent) {
          writeIndent(&buffer);
        }
        buffer.writeChar(']');
        m_wantIndent = false;
        break;
      }

      case ',': {
        m_wantIndent = true;
        buffer.writeChar(',');
        break;
      }

      case '"': {
        if(m_wantIndent) {
          writeIndent(&buffer);
        }
        buffer.writeChar('"');
        m_wantIndent = false;
        m_isInString = !m_isInString;
        break;
      }

      case ':': {
        if(!m_isInString) {
          buffer.write(": ", 2);
        } else {
          buffer.writeChar(':');
        }
        break;
      }

      default:
        buffer.writeChar(c);

    }

  }

  return m_outputStream->write(buffer.getData(), buffer.getCurrentPosition());

}


void Beautifier::setOutputStreamIOMode(IOMode ioMode) {
  return m_outputStream->setOutputStreamIOMode(ioMode);
}


Beautifier::IOMode Beautifier::getOutputStreamIOMode() {
  return m_outputStream->getOutputStreamIOMode();
}

}}}
