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

#include "ChunkedBody.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

ChunkedBody::ChunkedBody(const std::shared_ptr<ReadCallback>& readCallback,
                         const std::shared_ptr<AsyncReadCallback>& asyncReadCallback,
                         data::v_io_size chunkBufferSize)
  : m_readCallback(readCallback)
  , m_asyncReadCallback(asyncReadCallback)
  , m_buffer(new v_char8[chunkBufferSize])
  , m_bufferSize(chunkBufferSize)
{}

ChunkedBody::~ChunkedBody() {
  delete [] m_buffer;
}

bool ChunkedBody::writeData(OutputStream* stream, const void* data, data::v_io_size size) {
  return oatpp::data::stream::writeExactSizeData(stream, data, size) == size;
}

void ChunkedBody::declareHeaders(Headers& headers) noexcept {
  headers[oatpp::web::protocol::http::Header::TRANSFER_ENCODING] = oatpp::web::protocol::http::Header::Value::TRANSFER_ENCODING_CHUNKED;
}


void ChunkedBody::writeToStream(OutputStream* stream) noexcept {

  if(stream->getOutputStreamIOMode() != oatpp::data::stream::IOMode::BLOCKING) {
    OATPP_LOGE("[oatpp::web::protocol::http::outgoing::ChunkedBody::writeToStream()]", "Error. Blocking method called for NON_BLOCKING stream.");
  }

  if(!m_readCallback) {
    OATPP_LOGE("[oatpp::web::protocol::http::outgoing::ChunkedBody::writeToStream()]", "Error. ReadCallback not set.");
  }

  data::v_io_size size;

  while((size = m_readCallback->read(m_buffer, m_bufferSize)) > 0) {

    auto chunkSizeHex = oatpp::utils::conversion::primitiveToStr(size, "%X");

    bool res = writeData(stream, chunkSizeHex->getData(), chunkSizeHex->getSize()) &&
               writeData(stream, "\r\n", 2) &&
               writeData(stream, m_buffer, size) &&
               writeData(stream, "\r\n", 2);


    if(!res) {
      OATPP_LOGE("[oatpp::web::protocol::http::outgoing::ChunkedBody::writeToStream()]", "Error. Unknown Error. Can't write data.");
      return;
    }

  }

  if(!writeData(stream, "0\r\n\r\n", 5)) {
    OATPP_LOGE("[oatpp::web::protocol::http::outgoing::ChunkedBody::writeToStream()]", "Error. Unknown Error. Can't write data trailing bytes.");
    return;
  }

}

oatpp::async::CoroutineStarter ChunkedBody::writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) {

  class WriteCoroutine : public oatpp::async::Coroutine<WriteCoroutine> {
  private:
    std::shared_ptr<ChunkedBody> m_body;
    std::shared_ptr<OutputStream> m_stream;
    bool m_firstChunk;
  private:
    oatpp::String m_chunkSizeHex;
  private:
    data::stream::AsyncInlineReadData m_inlineReadData;
    data::stream::AsyncInlineWriteData m_inlineWriteData;
    data::stream::AsyncInlineWriteData m_chunkSizeWriteData;
  public:

    WriteCoroutine(const std::shared_ptr<ChunkedBody>& body,
                   const std::shared_ptr<OutputStream>& stream)
      : m_body(body)
      , m_stream(stream)
      , m_firstChunk(true)
    {}

    Action act() override {
      m_inlineReadData.set(m_body->m_buffer, m_body->m_bufferSize);
      return yieldTo(&WriteCoroutine::readCallback);
    }

    Action readCallback() {
      return m_body->m_asyncReadCallback->readAsyncInline(this, m_inlineReadData, yieldTo(&WriteCoroutine::onChunkRead));
    }

    Action onChunkRead() {

      data::v_io_size bytesRead = m_body->m_bufferSize - m_inlineReadData.bytesLeft;

      if(bytesRead > 0) {

        if(m_firstChunk) {
          m_chunkSizeHex = oatpp::utils::conversion::primitiveToStr(bytesRead, "%X") + "\r\n";
          m_firstChunk = false;
        } else {
          m_chunkSizeHex = "\r\n" + oatpp::utils::conversion::primitiveToStr(bytesRead, "%X") + "\r\n";
        }

        m_chunkSizeWriteData.set(m_chunkSizeHex->getData(), m_chunkSizeHex->getSize());
        m_inlineWriteData.set(m_body->m_buffer, bytesRead);

        return yieldTo(&WriteCoroutine::writeChunkSize);

      }

      m_chunkSizeWriteData.set("\r\n0\r\n\r\n", 7);

      return yieldTo(&WriteCoroutine::writeTrailingBytes);

    }

    Action writeChunkSize() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_chunkSizeWriteData, yieldTo(&WriteCoroutine::writeChunk));
    }

    Action writeChunk() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_inlineWriteData, yieldTo(&WriteCoroutine::act));
    }

    Action writeTrailingBytes() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_chunkSizeWriteData, finish());
    }

  };

  if(stream->getOutputStreamIOMode() != oatpp::data::stream::IOMode::NON_BLOCKING) {
    throw std::runtime_error("[oatpp::web::protocol::http::outgoing::ChunkedBody::writeToStreamAsync()]: Error. Async method called for BLOCKING stream.");
  }

  if(!m_asyncReadCallback) {
    throw std::runtime_error("[oatpp::web::protocol::http::outgoing::ChunkedBody::writeToStreamAsync()]: Error. AsyncReadCallback not set.");
  }

  return WriteCoroutine::start(shared_from_this(), stream);

}


}}}}}