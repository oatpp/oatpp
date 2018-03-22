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

#ifndef oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp
#define oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp

#include "../Http.hpp"
#include "./Body.hpp"

#include "../../../../../../oatpp-lib/core/src/data/stream/ChunkedBuffer.hpp"
#include "../../../../../../oatpp-lib/core/src/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class ChunkedBufferBody : public oatpp::base::Controllable, public Body {
public:
  static const char* ERROR_FAILED_TO_WRITE_DATA;
public:
  OBJECT_POOL(Http_Outgoing_ChunkedBufferBody_Pool, ChunkedBufferBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_ChunkedBufferBody_Pool, ChunkedBufferBody, 32)
protected:
  std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_buffer;
  bool m_chunked;
public:
  ChunkedBufferBody(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer,
                    bool chunked)
    : m_buffer(buffer)
    , m_chunked(chunked)
  {}
public:
  
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer) {
    return Shared_Http_Outgoing_ChunkedBufferBody_Pool::allocateShared(buffer, false);
  }
  
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer,
                                                   bool chunked) {
    return Shared_Http_Outgoing_ChunkedBufferBody_Pool::allocateShared(buffer, chunked);
  }
  
  void declareHeaders(const std::shared_ptr<Headers>& headers) override {
    if(m_chunked){
      headers->put(oatpp::web::protocol::http::Header::TRANSFER_ENCODING,
                   oatpp::web::protocol::http::Header::Value::TRANSFER_ENCODING_CHUNKED);
    } else {
      headers->put(oatpp::web::protocol::http::Header::CONTENT_LENGTH,
                   oatpp::utils::conversion::int64ToStr(m_buffer->getSize()));
    }
  }
  
  void writeToStream(const std::shared_ptr<OutputStream>& stream) override {
    if(m_chunked){
      auto chunks = m_buffer->getChunks();
      auto curr = chunks->getFirstNode();
      while (curr != nullptr) {
        stream->write(oatpp::utils::conversion::primitiveToStr(curr->getData()->size, "%X"));
        stream->write("\r\n", 2);
        stream->write(curr->getData()->data, curr->getData()->size);
        stream->write("\r\n", 2);
        curr = curr->getNext();
      }
      stream->write("0\r\n\r\n", 5);
    } else {
      m_buffer->flushToStream(stream);
    }
  }
  
  Action writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) override {
    if(m_chunked){

      struct LocalState {
        LocalState(const std::shared_ptr<OutputStream>& pStream,
                   const std::shared_ptr<oatpp::data::stream::ChunkedBuffer::Chunks>& pChunks,
                   oatpp::data::stream::ChunkedBuffer::Chunks::LinkedListNode* pCurr)
        : stream(pStream)
        , chunks(pChunks)
        , curr(pCurr)
        , whileState(0)
        {}
        std::shared_ptr<OutputStream> stream;
        std::shared_ptr<oatpp::data::stream::ChunkedBuffer::Chunks> chunks;
        oatpp::data::stream::ChunkedBuffer::Chunks::LinkedListNode* curr;
        v_int32 whileState; ///< introduced for optimization only
      };
      
      auto chunks = m_buffer->getChunks();
      auto state = std::make_shared<LocalState>(stream, chunks, chunks->getFirstNode());
      
      return oatpp::async::Routine::_do({
        [state] {
          
          auto curr = state->curr;
          
          if(curr == nullptr) {
            state->whileState = 3;
          }
          
          if(state->whileState == 0) {
            auto res = state->stream->write(oatpp::utils::conversion::primitiveToStr(curr->getData()->size, "%X\r\n"));
            if(res == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN) {
              return oatpp::async::Action::_wait_retry();
            } else if(res < 1) {
              return oatpp::async::Action(oatpp::async::Error(ERROR_FAILED_TO_WRITE_DATA));
            }
            state->whileState = 1;
          } else if(state->whileState == 1) {
            auto res = state->stream->write(curr->getData()->data, curr->getData()->size);
            if(res == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN) {
              return oatpp::async::Action::_wait_retry();
            } else if(res < curr->getData()->size) {
              return oatpp::async::Action(oatpp::async::Error(ERROR_FAILED_TO_WRITE_DATA));
            }
            state->whileState = 2;
          } else if(state->whileState == 2) {
            auto res = state->stream->write("\r\n", 2);
            if(res == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN) {
              return oatpp::async::Action::_wait_retry();
            } else if(res < 2) {
              return oatpp::async::Action(oatpp::async::Error(ERROR_FAILED_TO_WRITE_DATA));
            }
            state->whileState = 3;
          } else if(state->whileState == 3) {
            auto res = state->stream->write("0\r\n\r\n", 5);
            if(res == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN) {
              return oatpp::async::Action::_wait_retry();
            } else if(res < 5) {
              return oatpp::async::Action(oatpp::async::Error(ERROR_FAILED_TO_WRITE_DATA));
            }
            return oatpp::async::Action::_continue();
          }
          
          state->curr = curr->getNext();
          return oatpp::async::Action::_repeat();
          
        }, nullptr
      });
      
    } else {
      return m_buffer->flushToStreamAsync(stream);
    }
    
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp */
