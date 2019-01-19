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

#include "./Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

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
  
  void declareHeaders(Headers& headers) noexcept override {
    if(m_chunked){
      headers[oatpp::web::protocol::http::Header::TRANSFER_ENCODING] = oatpp::web::protocol::http::Header::Value::TRANSFER_ENCODING_CHUNKED;
    } else {
      headers[oatpp::web::protocol::http::Header::CONTENT_LENGTH] = oatpp::utils::conversion::int64ToStr(m_buffer->getSize());
    }
  }
  
  void writeToStream(const std::shared_ptr<OutputStream>& stream) noexcept override {
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
  
public:
  
  class WriteToStreamCoroutine : public oatpp::async::Coroutine<WriteToStreamCoroutine> {
  private:
    std::shared_ptr<ChunkedBufferBody> m_body;
    std::shared_ptr<OutputStream> m_stream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer::Chunks> m_chunks;
    oatpp::data::stream::ChunkedBuffer::Chunks::LinkedListNode* m_currChunk;
    const void* m_currData;
    oatpp::os::io::Library::v_size m_currDataSize;
    Action m_nextAction;
    v_char8 m_buffer[16];
  public:
    
    WriteToStreamCoroutine(const std::shared_ptr<ChunkedBufferBody>& body,
                           const std::shared_ptr<OutputStream>& stream)
      : m_body(body)
      , m_stream(stream)
      , m_chunks(m_body->m_buffer->getChunks())
      , m_currChunk(m_chunks->getFirstNode())
      , m_currData(nullptr)
      , m_currDataSize(0)
      , m_nextAction(Action(Action::TYPE_FINISH, nullptr, nullptr))
    {}
    
    Action act() override {
      if(m_currChunk == nullptr) {
        return yieldTo(&WriteToStreamCoroutine::writeEndOfChunks);
      }
      return yieldTo(&WriteToStreamCoroutine::writeChunkSize);
    }
    
    Action writeChunkSize() {
      m_currDataSize = oatpp::utils::conversion::primitiveToCharSequence(m_currChunk->getData()->size, m_buffer, "%X\r\n");
      m_currData = m_buffer;
      m_nextAction = yieldTo(&WriteToStreamCoroutine::writeChunkData);
      return yieldTo(&WriteToStreamCoroutine::writeCurrData);
    }
    
    Action writeChunkData() {
      m_currData = m_currChunk->getData()->data;
      m_currDataSize = (v_int32) m_currChunk->getData()->size;
      m_nextAction = yieldTo(&WriteToStreamCoroutine::writeChunkSeparator);
      return yieldTo(&WriteToStreamCoroutine::writeCurrData);
    }
    
    Action writeChunkSeparator() {
      m_currData = (void*) "\r\n";
      m_currDataSize = 2;
      m_currChunk = m_currChunk->getNext();
      m_nextAction = yieldTo(&WriteToStreamCoroutine::act);
      return yieldTo(&WriteToStreamCoroutine::writeCurrData);
    }
    
    Action writeEndOfChunks() {
      m_currData = (void*) "0\r\n\r\n";
      m_currDataSize = 5;
      m_nextAction = finish();
      return yieldTo(&WriteToStreamCoroutine::writeCurrData);
    }
    
    Action writeCurrData() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(m_stream.get(), m_currData, m_currDataSize, m_nextAction);
    }
    
  };
  
  Action writeToStreamAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                            const Action& actionOnFinish,
                            const std::shared_ptr<OutputStream>& stream) override {
    if(m_chunked) {
      return parentCoroutine->startCoroutine<WriteToStreamCoroutine>(actionOnFinish, getSharedPtr<ChunkedBufferBody>(), stream);
    } else {
      return m_buffer->flushToStreamAsync(parentCoroutine, actionOnFinish, stream);
    }
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp */
