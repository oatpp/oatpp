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
  ChunkedBufferBody(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer, bool chunked);
public:
  
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer);
  
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer, bool chunked);
  
  void declareHeaders(Headers& headers) noexcept override;
  
  void writeToStream(const std::shared_ptr<OutputStream>& stream) noexcept override;
  
public:
  
  class WriteToStreamCoroutine : public oatpp::async::Coroutine<WriteToStreamCoroutine> {
  private:
    std::shared_ptr<ChunkedBufferBody> m_body;
    std::shared_ptr<OutputStream> m_stream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer::Chunks> m_chunks;
    oatpp::data::stream::ChunkedBuffer::Chunks::LinkedListNode* m_currChunk;
    const void* m_currData;
    oatpp::data::v_io_size m_currDataSize;
    Action m_nextAction;
    v_char8 m_buffer[16];
  public:
    
    WriteToStreamCoroutine(const std::shared_ptr<ChunkedBufferBody>& body,
                           const std::shared_ptr<OutputStream>& stream);
    
    Action act() override;
    Action writeChunkSize();
    Action writeChunkData();
    Action writeChunkSeparator();
    Action writeEndOfChunks();
    Action writeCurrData();
    
  };
  
  Action writeToStreamAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                            const Action& actionOnFinish,
                            const std::shared_ptr<OutputStream>& stream) override;
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp */
