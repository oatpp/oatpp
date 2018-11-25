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

#include "RequestHeadersReader.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

os::io::Library::v_size RequestHeadersReader::readHeadersSection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                                                 oatpp::data::stream::OutputStream* bufferStream,
                                                                 Result& result) {
  
  v_word32 sectionEnd = ('\r' << 24) | ('\n' << 16) | ('\r' << 8) | ('\n');
  v_word32 accumulator = 0;
  v_int32 progress = 0;
  os::io::Library::v_size res;
  while (true) {
    
    v_int32 desiredToRead = m_bufferSize;
    if(progress + desiredToRead > m_maxHeadersSize) {
      desiredToRead = m_maxHeadersSize - progress;
    }
    
    res = connection->read(m_buffer, desiredToRead);
    if(res > 0) {
      bufferStream->write(m_buffer, res);
      
      for(v_int32 i = 0; i < res; i ++) {
        accumulator <<= 8;
        accumulator |= m_buffer[i];
        if(accumulator == sectionEnd) {
          result.bufferPosStart = i + 1;
          result.bufferPosEnd = (v_int32) res;
          break;
        }
      }
      
    } else if(res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY || res == oatpp::data::stream::Errors::ERROR_IO_RETRY) {
      continue;
    } else {
      break;
    }
    
  }
  
  return res;
  
}
  
RequestHeadersReader::Result RequestHeadersReader::readHeaders(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                                               http::Status& error) {
  
  RequestHeadersReader::Result result;
  
  oatpp::data::stream::ChunkedBuffer buffer;
  auto res = readHeadersSection(connection, &buffer, result);
  
  if(res > 0) {
    auto headersText = buffer.toString();
    oatpp::parser::ParsingCaret caret (headersText);
    http::Status status;
    http::Protocol::parseHeadersLabels(result.headers, headersText.getPtr(), caret, status);
  }
  
  return result;
  
}

}}}}}
