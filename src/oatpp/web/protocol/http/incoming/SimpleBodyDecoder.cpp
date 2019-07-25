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

#include "SimpleBodyDecoder.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {
  
data::v_io_size SimpleBodyDecoder::readLine(oatpp::data::stream::InputStream* fromStream,
                                            p_char8 buffer,
                                            data::v_io_size maxLineSize)
{
  
  v_char8 a;
  data::v_io_size count = 0;
  while (fromStream->read(&a, 1) > 0) {
    if(a != '\r') {
      if(count + 1 > maxLineSize) {
        OATPP_LOGE("BodyDecoder", "Error - too long line");
        return 0;
      }
      buffer[count++] = a;
    } else {
      fromStream->read(&a, 1);
      if(a != '\n'){
        OATPP_LOGE("BodyDecoder", "Warning - invalid line breaker");
      }
      return count; // size of line
    }
  }
  
  return count;
  
}

void SimpleBodyDecoder::doChunkedDecoding(oatpp::data::stream::InputStream* fromStream,
                                          oatpp::data::stream::WriteCallback* writeCallback) {
  
  auto buffer = oatpp::data::buffer::IOBuffer::createShared();
  
  v_int32 maxLineSize = 8; // 0xFFFFFFFF 4Gb for chunk
  v_char8 lineBuffer[maxLineSize + 1];
  data::v_io_size countToRead;
  
  do {
    
    auto lineSize = readLine(fromStream, lineBuffer, maxLineSize);
    if(lineSize == 0 || lineSize >= maxLineSize) {
      return; // error reading stream
    }
    lineBuffer[lineSize] = 0;
    countToRead = strtol((const char*)lineBuffer, nullptr, 16);
    
    if(countToRead > 0) {
      oatpp::data::stream::transfer(fromStream, writeCallback, countToRead, buffer->getData(), buffer->getSize());
    }
    
    fromStream->read(lineBuffer, 2); // just skip "\r\n"
    
  } while (countToRead > 0);
  
}

void SimpleBodyDecoder::decode(const Headers& headers,
                               oatpp::data::stream::InputStream* bodyStream,
                               oatpp::data::stream::WriteCallback* writeCallback) const {
  
  auto transferEncodingIt = headers.find(Header::TRANSFER_ENCODING);
  if(transferEncodingIt != headers.end() && transferEncodingIt->second == Header::Value::TRANSFER_ENCODING_CHUNKED) {
    doChunkedDecoding(bodyStream, writeCallback);
  } else {
    oatpp::data::v_io_size contentLength = 0;
    auto contentLengthStrIt = headers.find(Header::CONTENT_LENGTH);
    if(contentLengthStrIt == headers.end()) {
      return; // DO NOTHING // it is an empty or invalid body
    } else {
      bool success;
      contentLength = oatpp::utils::conversion::strToInt64(contentLengthStrIt->second.toString(), success);
      if(!success){
        return; // it is an invalid request/response
      }
      auto buffer = oatpp::data::buffer::IOBuffer::createShared();
      if(contentLength > 0) {
        oatpp::data::stream::transfer(bodyStream, writeCallback, contentLength, buffer->getData(), buffer->getSize());
      }
    }
  }
  
}

oatpp::async::CoroutineStarter SimpleBodyDecoder::doChunkedDecodingAsync(const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                                                         const std::shared_ptr<oatpp::data::stream::AsyncWriteCallback>& writeCallback) {
  
  class ChunkedDecoder : public oatpp::async::Coroutine<ChunkedDecoder> {
  private:
    const v_int32 MAX_LINE_SIZE = 8;
  private:
    std::shared_ptr<oatpp::data::stream::InputStream> m_fromStream;
    std::shared_ptr<oatpp::data::stream::AsyncWriteCallback> m_writeCallback;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> m_buffer = oatpp::data::buffer::IOBuffer::createShared();
    v_int32 m_currLineLength;
    v_char8 m_lineChar;
    bool m_lineEnding;
    v_char8 m_lineBuffer [16]; // used max 8
    data::stream::AsyncInlineReadData m_skipData;
    bool m_done = false;
  private:
    void prepareSkipRN() {
      m_skipData.set(&m_lineBuffer[0], 2);
      m_currLineLength = 0;
      m_lineEnding = false;
    }
  public:
    
    ChunkedDecoder(const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                   const std::shared_ptr<oatpp::data::stream::AsyncWriteCallback>& writeCallback)
      : m_fromStream(fromStream)
      , m_writeCallback(writeCallback)
    {}
    
    Action act() override {
      m_currLineLength = 0;
      m_lineEnding = false;
      return yieldTo(&ChunkedDecoder::readLineChar);
    }
    
    Action readLineChar() {
      auto res = m_fromStream->read(&m_lineChar, 1);
      if(res == data::IOError::WAIT_RETRY) {
        return m_fromStream->suggestInputStreamAction(res);
      } else if(res == data::IOError::RETRY) {
        return m_fromStream->suggestInputStreamAction(res);
      } else if( res < 0) {
        return error<Error>("[BodyDecoder::ChunkedDecoder] Can't read line char");
      }
      return yieldTo(&ChunkedDecoder::onLineCharRead);
    }
    
    Action onLineCharRead() {
      if(!m_lineEnding) {
        if(m_lineChar != '\r') {
          if(m_currLineLength + 1 > MAX_LINE_SIZE){
            return error<Error>("[BodyDecoder::ChunkedDecoder] too long line");
          }
          m_lineBuffer[m_currLineLength ++] = m_lineChar;
          return yieldTo(&ChunkedDecoder::readLineChar);
        } else {
          m_lineEnding = true;
          return yieldTo(&ChunkedDecoder::readLineChar);
        }
      } else {
        if(m_lineChar != '\n') {
          OATPP_LOGD("[BodyDecoder::ChunkedDecoder]", "Warning - invalid line breaker")
        }
      }
      if(m_currLineLength == 0) {
        return error<Error>("Error reading stream. 0-length line");
      }
      m_lineBuffer[m_currLineLength] = 0;
      return yieldTo(&ChunkedDecoder::onLineRead);
    }
    
    Action onLineRead() {
      data::v_io_size countToRead = strtol((const char*) m_lineBuffer, nullptr, 16);
      if(countToRead > 0) {
        prepareSkipRN();
        return oatpp::data::stream::transferAsync(m_fromStream, m_writeCallback, countToRead, m_buffer).next(yieldTo(&ChunkedDecoder::skipRN));
      }
      m_done = true;
      prepareSkipRN();
      return yieldTo(&ChunkedDecoder::skipRN);
    }
    
    Action skipRN() {
      if(m_done) {
        return oatpp::data::stream::readExactSizeDataAsyncInline(this, m_fromStream.get(), m_skipData, finish());
      } else {
        return oatpp::data::stream::readExactSizeDataAsyncInline(this, m_fromStream.get(), m_skipData, yieldTo(&ChunkedDecoder::readLineChar));
      }
    }
    
  };
  
  return ChunkedDecoder::start(fromStream, writeCallback);
  
}

oatpp::async::CoroutineStarter SimpleBodyDecoder::decodeAsync(const Headers& headers,
                                                              const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                                              const std::shared_ptr<oatpp::data::stream::AsyncWriteCallback>& writeCallback) const {
  auto transferEncodingIt = headers.find(Header::TRANSFER_ENCODING);
  if(transferEncodingIt != headers.end() && transferEncodingIt->second == Header::Value::TRANSFER_ENCODING_CHUNKED) {
    return doChunkedDecodingAsync(bodyStream, writeCallback);
  } else {
    oatpp::data::v_io_size contentLength = 0;
    auto contentLengthStrIt = headers.find(Header::CONTENT_LENGTH);
    if(contentLengthStrIt == headers.end()) {
      return nullptr;
    } else {
      bool success;
      contentLength = oatpp::utils::conversion::strToInt64(contentLengthStrIt->second.toString(), success);
      if(!success){
        throw HttpError(http::Status::CODE_400, "Invalid 'Content-Length' Header");
      }
      if(contentLength > 0) {
        return oatpp::data::stream::transferAsync(bodyStream, writeCallback, contentLength, oatpp::data::buffer::IOBuffer::createShared());
      } else {
        return nullptr;
      }
    }
  }
}
  
}}}}}
