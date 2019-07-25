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

#include "RequestHeadersReader.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

data::v_io_size RequestHeadersReader::readHeadersSection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                                         oatpp::data::stream::OutputStream* bufferStream,
                                                         Result& result) {

  v_word32 accumulator = 0;
  v_int32 progress = 0;
  data::v_io_size res;
  while (true) {
    
    v_int32 desiredToRead = m_bufferSize;
    if(progress + desiredToRead > m_maxHeadersSize) {
      desiredToRead = m_maxHeadersSize - progress;
      if(desiredToRead <= 0) {
        return -1;
      }
    }
    
    res = connection->read(m_buffer, desiredToRead);
    if(res > 0) {
      bufferStream->write(m_buffer, res);
      
      for(v_int32 i = 0; i < res; i ++) {
        accumulator <<= 8;
        accumulator |= m_buffer[i];
        if(accumulator == SECTION_END) {
          result.bufferPosStart = i + 1;
          result.bufferPosEnd = (v_int32) res;
          return res;
        }
      }
      
    } else if(res == data::IOError::WAIT_RETRY || res == data::IOError::RETRY) {
      continue;
    } else {
      break;
    }
    
  }
  
  return res;
  
}
  
RequestHeadersReader::Result RequestHeadersReader::readHeaders(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                                               http::HttpError::Info& error) {
  
  RequestHeadersReader::Result result;
  
  oatpp::data::stream::ChunkedBuffer buffer;
  error.ioStatus = readHeadersSection(connection, &buffer, result);
  
  if(error.ioStatus > 0) {
    auto headersText = buffer.toString();
    oatpp::parser::Caret caret (headersText);
    http::Status status;
    http::Parser::parseRequestStartingLine(result.startingLine, headersText.getPtr(), caret, status);
    if(status.code == 0) {
      http::Parser::parseHeaders(result.headers, headersText.getPtr(), caret, status);
    }
  }
  
  return result;
  
}
  
  
oatpp::async::CoroutineStarterForResult<const RequestHeadersReader::Result&>
RequestHeadersReader::readHeadersAsync(const std::shared_ptr<oatpp::data::stream::IOStream>& connection)
{
  
  class ReaderCoroutine : public oatpp::async::CoroutineWithResult<ReaderCoroutine, const Result&> {
  private:
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    p_char8 m_buffer;
    v_int32 m_bufferSize;
    v_int32 m_maxHeadersSize;
    v_word32 m_accumulator;
    v_int32 m_progress;
    RequestHeadersReader::Result m_result;
    oatpp::data::stream::ChunkedBuffer m_bufferStream;
  public:
    
    ReaderCoroutine(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                    p_char8 buffer, v_int32 bufferSize, v_int32 maxHeadersSize)
      : m_connection(connection)
      , m_buffer(buffer)
      , m_bufferSize(bufferSize)
      , m_maxHeadersSize(maxHeadersSize)
      , m_accumulator(0)
      , m_progress(0)
    {}
    
    Action act() override {
      
      v_int32 desiredToRead = m_bufferSize;
      if(m_progress + desiredToRead > m_maxHeadersSize) {
        desiredToRead = m_maxHeadersSize - m_progress;
        if(desiredToRead <= 0) {
          return error<Error>("[oatpp::web::protocol::http::incoming::RequestHeadersReader::readHeadersAsync()]: Error. Headers section is too large.");
        }
      }
      
      auto res = m_connection->read(m_buffer, desiredToRead);
      if(res > 0) {
        m_bufferStream.write(m_buffer, res);
        m_progress += res;
        
        for(v_int32 i = 0; i < res; i ++) {
          m_accumulator <<= 8;
          m_accumulator |= m_buffer[i];
          if(m_accumulator == SECTION_END) {
            m_result.bufferPosStart = i + 1;
            m_result.bufferPosEnd = (v_int32) res;
            return yieldTo(&ReaderCoroutine::parseHeaders);
          }
        }
        
        return m_connection->suggestInputStreamAction(res);
        
      } else if(res == data::IOError::WAIT_RETRY || res == data::IOError::RETRY) {
        return m_connection->suggestInputStreamAction(res);
      } else if(res == data::IOError::BROKEN_PIPE){
        return error(oatpp::data::AsyncIOError::ERROR_BROKEN_PIPE);
      } else if(res == data::IOError::ZERO_VALUE){
        return error(oatpp::data::AsyncIOError::ERROR_BROKEN_PIPE);
      } else {
        return error<Error>("[oatpp::web::protocol::http::incoming::RequestHeadersReader::readHeadersAsync()]: Error. Error reading connection stream.");
      }
      
    }
    
    Action parseHeaders() {
      
      auto headersText = m_bufferStream.toString();
      oatpp::parser::Caret caret (headersText);
      http::Status status;
      http::Parser::parseRequestStartingLine(m_result.startingLine, headersText.getPtr(), caret, status);
      if(status.code == 0) {
        http::Parser::parseHeaders(m_result.headers, headersText.getPtr(), caret, status);
        if(status.code == 0) {
          return _return(m_result);
        } else {
          return error<Error>("[oatpp::web::protocol::http::incoming::RequestHeadersReader::readHeadersAsync()]: Error. Error occurred while parsing headers.");
        }
      } else {
        return error<Error>("[oatpp::web::protocol::http::incoming::RequestHeadersReader::readHeadersAsync()]: Error. Can't parse starting line.");
      }
      
    }
    
  };
  
  return ReaderCoroutine::startForResult(connection, m_buffer, m_bufferSize, m_maxHeadersSize);
  
}

}}}}}
