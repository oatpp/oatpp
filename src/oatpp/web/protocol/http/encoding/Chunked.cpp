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

#include "Chunked.hpp"

#include "oatpp/utils/Conversion.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace encoding {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EncoderChunked

v_io_size EncoderChunked::suggestInputStreamReadSize() {
  return 32767;
}

v_int32 EncoderChunked::iterate(data::buffer::InlineReadData& dataIn, data::buffer::InlineReadData& dataOut) {

  if(dataOut.bytesLeft > 0) {
    return Error::FLUSH_DATA_OUT;
  }

  if(dataIn.currBufferPtr != nullptr) {

    if(m_lastFlush > 0) {
      dataIn.inc(m_lastFlush);
      m_lastFlush = 0;
    }

    if(m_finished){
      dataOut.set(nullptr, 0);
      return Error::FINISHED;
    }

    if(dataIn.bytesLeft == 0) {
      return Error::PROVIDE_DATA_IN;
    }

    if(m_writeChunkHeader) {

      async::Action action;
      data::stream::BufferOutputStream stream(16);
      if(!m_firstChunk) {
        stream.write("\r\n", 2, action);
      }

      stream << utils::Conversion::primitiveToStr(dataIn.bytesLeft, "%X");
      stream.write("\r\n", 2, action);

      m_chunkHeader = stream.toString();
      dataOut.set(reinterpret_cast<p_char8>(m_chunkHeader->data()), static_cast<v_buff_size>(m_chunkHeader->size()));

      m_firstChunk = false;
      m_writeChunkHeader = false;

      return Error::FLUSH_DATA_OUT;

    }

    dataOut = dataIn;
    m_lastFlush = dataOut.bytesLeft;
    m_writeChunkHeader = true;
    return Error::FLUSH_DATA_OUT;

  }

  if(m_writeChunkHeader){

    async::Action action;
    data::stream::BufferOutputStream stream(16);
    if(!m_firstChunk) {
      stream.write("\r\n", 2, action);
    }

    stream.write("0\r\n\r\n", 5, action);

    m_chunkHeader = stream.toString();
    dataOut.set(reinterpret_cast<p_char8>(m_chunkHeader->data()), static_cast<v_buff_size>(m_chunkHeader->size()));

    m_firstChunk = false;
    m_writeChunkHeader = false;

    return Error::FLUSH_DATA_OUT;

  }

  m_finished = true;
  dataOut.set(nullptr, 0);
  return Error::FINISHED;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DecoderChunked

DecoderChunked::DecoderChunked()
  : m_chunkHeaderBuffer(16)
  , m_currentChunkSize(-1)
  , m_firstChunk(true)
  , m_finished(false)
  , m_lastFlush(0)
{}

v_io_size DecoderChunked::suggestInputStreamReadSize() {
  if(m_currentChunkSize > 0) {
    return m_currentChunkSize;
  }
  return 1;
}

v_int32 DecoderChunked::readHeader(data::buffer::InlineReadData& dataIn) {

  async::Action action;

  while(dataIn.bytesLeft > 0 && m_currentChunkSize < 0) {

    if (m_chunkHeaderBuffer.getCurrentPosition() < 12) {

      m_chunkHeaderBuffer.write(dataIn.currBufferPtr, 1, action);
      dataIn.inc(1);

      auto pos = m_chunkHeaderBuffer.getCurrentPosition();

      if(m_currentChunkSize == -1) {

        if (pos > 2 && m_chunkHeaderBuffer.getData()[pos - 2] == '\r' && m_chunkHeaderBuffer.getData()[pos - 1] == '\n') {

          auto data = m_chunkHeaderBuffer.getData();
          v_buff_size i = 0;
          v_buff_size lineEnd = pos - 2;

          if(!m_firstChunk) {
            // data of the previous chunk must be followed by "\r\n"
            if(data[0] != '\r' || data[1] != '\n') {
              return ERROR_CHUNK_HEADER_INVALID;
            }
            i = 2;
          }

          // chunk-size = 1*HEXDIG
          // header length is capped above so the value can't overflow
          v_buff_size digitsStart = i;
          v_io_size chunkSize = 0;
          for(; i < lineEnd; i++) {
            v_char8 a = data[i];
            if(a >= '0' && a <= '9') {
              chunkSize = (chunkSize << 4) | (a - '0');
            } else if(a >= 'A' && a <= 'F') {
              chunkSize = (chunkSize << 4) | (a - 'A' + 10);
            } else if(a >= 'a' && a <= 'f') {
              chunkSize = (chunkSize << 4) | (a - 'a' + 10);
            } else {
              break;
            }
          }

          if(i == digitsStart) {
            return ERROR_CHUNK_HEADER_INVALID;
          }

          // only chunk-ext (*( BWS ";" ... )) may follow chunk-size
          while(i < lineEnd && (data[i] == ' ' || data[i] == '\t')) {
            i++;
          }
          if(i < lineEnd && data[i] != ';') {
            return ERROR_CHUNK_HEADER_INVALID;
          }

          m_currentChunkSize = chunkSize;
          m_firstChunk = false;

          if (m_currentChunkSize > 0) {
            return Error::OK;
          } else {
            m_currentChunkSize = -2;
          }

        }

      } else if(m_currentChunkSize == -2) {

        if (pos > 4 &&
            m_chunkHeaderBuffer.getData()[pos - 4] == '\r' && m_chunkHeaderBuffer.getData()[pos - 3] == '\n' &&
            m_chunkHeaderBuffer.getData()[pos - 2] == '\r' && m_chunkHeaderBuffer.getData()[pos - 1] == '\n') {
          m_currentChunkSize = 0;
          m_finished = true;
          return Error::OK;
        }

      }

    } else {
      return ERROR_CHUNK_HEADER_TOO_LONG;
    }

  }

  return Error::PROVIDE_DATA_IN;

}

v_int32 DecoderChunked::iterate(data::buffer::InlineReadData& dataIn, data::buffer::InlineReadData& dataOut) {

  if(dataOut.bytesLeft > 0) {
    return Error::FLUSH_DATA_OUT;
  }

  if(dataIn.currBufferPtr != nullptr) {

    if(m_lastFlush > 0) {
      dataIn.inc(m_lastFlush);
      m_currentChunkSize -= m_lastFlush;
      if(m_currentChunkSize == 0) {
        m_currentChunkSize = -1;
      }
      m_lastFlush = 0;
    }

    if (m_finished) {
      dataOut.set(nullptr, 0);
      return Error::FINISHED;
    }

    if (dataIn.bytesLeft == 0) {
      return Error::PROVIDE_DATA_IN;
    }

    if(m_currentChunkSize < 0) {
      return readHeader(dataIn);
    } else if(m_currentChunkSize == 0) {
      m_chunkHeaderBuffer.setCurrentPosition(0);
      dataOut.set(nullptr, 0);
      m_finished = true;
      return Error::FINISHED;
    }

    m_chunkHeaderBuffer.setCurrentPosition(0);

    m_lastFlush = dataIn.bytesLeft;
    if(m_lastFlush > m_currentChunkSize) {
      m_lastFlush = m_currentChunkSize;
    }

    dataOut.set(dataIn.currBufferPtr, m_lastFlush);
    return Error::FLUSH_DATA_OUT;

  }

  m_chunkHeaderBuffer.setCurrentPosition(0);
  dataOut.set(nullptr, 0);
  m_finished = true;
  return Error::FINISHED;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChunkedEncoderProvider

oatpp::String ChunkedEncoderProvider::getEncodingName() {
  return "chunked";
}

std::shared_ptr<data::buffer::Processor> ChunkedEncoderProvider::getProcessor() {
  return std::make_shared<EncoderChunked>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChunkedDecoderProvider

oatpp::String ChunkedDecoderProvider::getEncodingName() {
  return "chunked";
}

std::shared_ptr<data::buffer::Processor> ChunkedDecoderProvider::getProcessor() {
  return std::make_shared<DecoderChunked>();
}

}}}}}
