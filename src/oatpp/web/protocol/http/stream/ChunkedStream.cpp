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

#include "ChunkedStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace stream {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChunkedDecodingStream

ChunkedDecodingStream::ChunkedDecodingStream(const base::ObjectHandle<data::stream::InputStream>& baseStream)
  : m_baseStream(baseStream)
  , m_chunkSize(0)
  , m_lineSize(0)
  , m_done(false)
{}

data::v_io_size ChunkedDecodingStream::readLine(void *buffer, v_buff_size count, async::Action& action) {

  if(m_lineSize < 10) {

    auto res = m_baseStream->read(&m_lineBuffer[m_lineSize], 1, action);

    if(res == 1) {

      ++ m_lineSize;

      if (m_lineSize > 2 && m_lineBuffer[m_lineSize - 2] == '\r' && m_lineBuffer[m_lineSize - 1] == '\n') {

        m_lineBuffer[m_lineSize] = 0;
        m_lineSize = 0;
        m_chunkSize = strtol((const char *) m_lineBuffer, nullptr, 16);

        if(m_chunkSize == 0) {
          m_done = true;
          return data::IOError::ZERO_VALUE;
        }

      }

      return data::IOError::RETRY_READ;

    } else if(res > 1) {
      return data::IOError::BROKEN_PIPE;
    }

    return res;
  }

  throw std::runtime_error("[oatpp::web::protocol::http::stream::ChunkedDecodingStream::readLine()]: Error. Line is too long.");

}

data::v_io_size ChunkedDecodingStream::read(void *buffer, v_buff_size count, async::Action& action) {

  if(m_done) {
    return data::IOError::ZERO_VALUE;
  }

  if(m_chunkSize == 0) {
    return readLine(buffer, count, action);
  } else if(m_chunkSize > 0) {
    data::v_io_size desiredToRead = count;
    if(desiredToRead > m_chunkSize) {
      desiredToRead = m_chunkSize;
    }
    auto res = m_baseStream->read(buffer, desiredToRead, action);
    if(res > 0) {
      m_chunkSize -= res;
    }
    return res;
  }

  throw std::runtime_error("[oatpp::web::protocol::http::stream::ChunkedDecodingStream::read()]: Error. Invalid State.");

}

void ChunkedDecodingStream::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_baseStream->setInputStreamIOMode(ioMode);
}

data::stream::IOMode ChunkedDecodingStream::getInputStreamIOMode() {
  return m_baseStream->getInputStreamIOMode();
}

data::stream::Context& ChunkedDecodingStream::getInputStreamContext() {
  return m_baseStream->getInputStreamContext();
}

}}}}}
