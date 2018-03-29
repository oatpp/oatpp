/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "BodyDecoder.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

os::io::Library::v_size BodyDecoder::readLine(const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                              p_char8 buffer,
                                              os::io::Library::v_size maxLineSize) {
  
  v_char8 a;
  os::io::Library::v_size count = 0;
  while (fromStream->read(&a, 1) > 0) {
    if(a != '\r') {
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
  
void BodyDecoder::doChunkedDecoding(const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                    const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) {
  
  auto buffer = oatpp::data::buffer::IOBuffer::createShared();
  
  v_int32 maxLineSize = 8; // 0xFFFFFFFF 4Gb for chunk
  v_char8 lineBuffer[maxLineSize];
  os::io::Library::v_size countToRead;
  
  do {
    
    auto lineSize = readLine(fromStream, lineBuffer, maxLineSize);
    if(lineSize == 0 || lineSize >= maxLineSize) {
      return; // error reading stream
    }
    lineBuffer[lineSize] = 0;
    countToRead = std::strtol((const char*)lineBuffer, nullptr, 16);
    
    if(countToRead > 0) {
      oatpp::data::stream::transfer(fromStream, toStream, countToRead, buffer->getData(), buffer->getSize());
    }
    
    fromStream->read(lineBuffer, 2); // just skip "\r\n"
    
  } while (countToRead > 0);
  
}
  
void BodyDecoder::decode(const std::shared_ptr<Protocol::Headers>& headers,
                         const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                         const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) {
  
  auto transferEncoding = headers->get(Header::TRANSFER_ENCODING, nullptr);
  if(!transferEncoding.isNull() && transferEncoding->equals(Header::Value::TRANSFER_ENCODING_CHUNKED)) {
    doChunkedDecoding(bodyStream, toStream);
  } else {
    oatpp::os::io::Library::v_size contentLength = 0;
    auto contentLengthStr = headers->get(Header::CONTENT_LENGTH, nullptr);
    if(contentLengthStr.isNull()) {
      return; // DO NOTHING // it is an empty or invalid body
    } else {
      bool success;
      contentLength = oatpp::utils::conversion::strToInt64(contentLengthStr, success);
      if(!success){
        return; // it is an invalid request/response
      }
      auto buffer = oatpp::data::buffer::IOBuffer::createShared();
      oatpp::data::stream::transfer(bodyStream, toStream, contentLength, buffer->getData(), buffer->getSize());
    }
  }
  
}

}}}}}
