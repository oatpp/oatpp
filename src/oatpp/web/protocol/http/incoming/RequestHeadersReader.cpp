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

data::v_io_size RequestHeadersReader::readHeadersSection(data::stream::InputStreamBufferedProxy* stream, Result& result) {

  v_word32 accumulator = 0;
  m_bufferStream->setCurrentPosition(0);
  data::v_io_size res;
  while (true) {
    
    v_buff_size desiredToRead = m_readChunkSize;
    if(m_bufferStream->getCurrentPosition() + desiredToRead > m_maxHeadersSize) {
      desiredToRead = m_maxHeadersSize - m_bufferStream->getCurrentPosition();
      if(desiredToRead <= 0) {
        return -1;
      }
    }

    m_bufferStream->reserveBytesUpfront(desiredToRead);
    auto bufferData = m_bufferStream->getData() + m_bufferStream->getCurrentPosition();
    res = stream->peek(bufferData, desiredToRead);
    if(res > 0) {

      m_bufferStream->setCurrentPosition(m_bufferStream->getCurrentPosition() + res);

      for(v_buff_size i = 0; i < res; i ++) {
        accumulator <<= 8;
        accumulator |= bufferData[i];
        if(accumulator == SECTION_END) {
          stream->commitReadOffset(i + 1);
          return res;
        }
      }

      stream->commitReadOffset(res);
      
    } else if(res == data::IOError::WAIT_RETRY || res == data::IOError::RETRY) {
      continue;
    } else {
      break;
    }
    
  }
  
  return res;
  
}
  
RequestHeadersReader::Result RequestHeadersReader::readHeaders(data::stream::InputStreamBufferedProxy* stream,
                                                               http::HttpError::Info& error) {
  
  RequestHeadersReader::Result result;

  error.ioStatus = readHeadersSection(stream, result);
  
  if(error.ioStatus > 0) {
    oatpp::parser::Caret caret (m_bufferStream->getData(), m_bufferStream->getCurrentPosition());
    http::Status status;
    http::Parser::parseRequestStartingLine(result.startingLine, nullptr, caret, status);
    if(status.code == 0) {
      http::Parser::parseHeaders(result.headers, nullptr, caret, status);
    }
  }
  
  return result;
  
}
  
  
oatpp::async::CoroutineStarterForResult<const RequestHeadersReader::Result&>
RequestHeadersReader::readHeadersAsync(const std::shared_ptr<data::stream::InputStreamBufferedProxy>& stream)
{
  
  class ReaderCoroutine : public oatpp::async::CoroutineWithResult<ReaderCoroutine, const Result&> {
  private:
    std::shared_ptr<data::stream::InputStreamBufferedProxy> m_stream;
    RequestHeadersReader* m_this;
    v_word32 m_accumulator;
    RequestHeadersReader::Result m_result;
  public:
    
    ReaderCoroutine(RequestHeadersReader* _this,
                    const std::shared_ptr<data::stream::InputStreamBufferedProxy>& stream)
      : m_stream(stream)
      , m_this(_this)
      , m_accumulator(0)
    {
      m_this->m_bufferStream->setCurrentPosition(0);
    }
    
    Action act() override {
      
      v_buff_size desiredToRead = m_this->m_readChunkSize;
      if(m_this->m_bufferStream->getCurrentPosition() + desiredToRead > m_this->m_maxHeadersSize) {
        desiredToRead = m_this->m_maxHeadersSize - m_this->m_bufferStream->getCurrentPosition();
        if(desiredToRead <= 0) {
          return error<Error>("[oatpp::web::protocol::http::incoming::RequestHeadersReader::readHeadersAsync()]: Error. Headers section is too large.");
        }
      }

      m_this->m_bufferStream->reserveBytesUpfront(desiredToRead);
      auto bufferData = m_this->m_bufferStream->getData() + m_this->m_bufferStream->getCurrentPosition();
      auto res = m_stream->peek(bufferData, desiredToRead);
      if(res > 0) {

        m_this->m_bufferStream->setCurrentPosition(m_this->m_bufferStream->getCurrentPosition() + res);
        
        for(v_buff_size i = 0; i < res; i ++) {
          m_accumulator <<= 8;
          m_accumulator |= bufferData[i];
          if(m_accumulator == SECTION_END) {
            m_stream->commitReadOffset(i + 1);
            return yieldTo(&ReaderCoroutine::parseHeaders);
          }
        }

        m_stream->commitReadOffset(res);
        
        return m_stream->suggestInputStreamAction(res);
        
      } else if(res == data::IOError::WAIT_RETRY || res == data::IOError::RETRY) {
        return m_stream->suggestInputStreamAction(res);
      } else if(res == data::IOError::BROKEN_PIPE){
        return error<oatpp::data::AsyncIOError>(data::IOError::BROKEN_PIPE);
      } else if(res == data::IOError::ZERO_VALUE){
        return error<oatpp::data::AsyncIOError>(data::IOError::BROKEN_PIPE);
      } else {
        return error<Error>("[oatpp::web::protocol::http::incoming::RequestHeadersReader::readHeadersAsync()]: Error. Error reading connection stream.");
      }
      
    }
    
    Action parseHeaders() {

      oatpp::parser::Caret caret (m_this->m_bufferStream->getData(), m_this->m_bufferStream->getCurrentPosition());
      http::Status status;
      http::Parser::parseRequestStartingLine(m_result.startingLine, nullptr, caret, status);
      if(status.code == 0) {
        http::Parser::parseHeaders(m_result.headers, nullptr, caret, status);
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
  
  return ReaderCoroutine::startForResult(this, stream);
  
}

}}}}}
