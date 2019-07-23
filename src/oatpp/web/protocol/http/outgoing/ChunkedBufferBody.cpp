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

#include "ChunkedBufferBody.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

const char* ChunkedBufferBody::ERROR_FAILED_TO_WRITE_DATA = "ERROR_FAILED_TO_WRITE_DATA";

ChunkedBufferBody::ChunkedBufferBody(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer, bool chunked)
  : m_buffer(buffer)
  , m_chunked(chunked)
{}

std::shared_ptr<ChunkedBufferBody> ChunkedBufferBody::createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer, bool chunked) {
  return Shared_Http_Outgoing_ChunkedBufferBody_Pool::allocateShared(buffer, chunked);
}

void ChunkedBufferBody::declareHeaders(Headers& headers) noexcept {
  if(m_chunked){
    headers[oatpp::web::protocol::http::Header::TRANSFER_ENCODING] = oatpp::web::protocol::http::Header::Value::TRANSFER_ENCODING_CHUNKED;
  } else {
    headers[oatpp::web::protocol::http::Header::CONTENT_LENGTH] = oatpp::utils::conversion::int64ToStr(m_buffer->getSize());
  }
}

void ChunkedBufferBody::writeToStream(OutputStream* stream) noexcept {
  if(m_chunked) {
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

ChunkedBufferBody::WriteToStreamCoroutine::WriteToStreamCoroutine(const std::shared_ptr<ChunkedBufferBody>& body,
                                                                  const std::shared_ptr<OutputStream>& stream)
  : m_body(body)
  , m_stream(stream)
  , m_chunks(m_body->m_buffer->getChunks())
  , m_currChunk(m_chunks->getFirstNode())
  , m_nextAction(Action::createActionByType(Action::TYPE_FINISH))
{}

async::Action ChunkedBufferBody::WriteToStreamCoroutine::act() {
    if(m_currChunk == nullptr) {
    return yieldTo(&WriteToStreamCoroutine::writeEndOfChunks);
  }
  return yieldTo(&WriteToStreamCoroutine::writeChunkSize);
}

async::Action ChunkedBufferBody::WriteToStreamCoroutine::writeChunkSize() {
  m_inlineWriteData.set(m_buffer, oatpp::utils::conversion::primitiveToCharSequence(m_currChunk->getData()->size, m_buffer, "%X\r\n"));
  m_nextAction = yieldTo(&WriteToStreamCoroutine::writeChunkData);
  return yieldTo(&WriteToStreamCoroutine::writeCurrData);
}

async::Action ChunkedBufferBody::WriteToStreamCoroutine::writeChunkData() {
  m_inlineWriteData.set(m_currChunk->getData()->data, m_currChunk->getData()->size);
  m_nextAction = yieldTo(&WriteToStreamCoroutine::writeChunkSeparator);
  return yieldTo(&WriteToStreamCoroutine::writeCurrData);
}

async::Action ChunkedBufferBody::WriteToStreamCoroutine::writeChunkSeparator() {
  m_inlineWriteData.set("\r\n", 2);
  m_currChunk = m_currChunk->getNext();
  m_nextAction = yieldTo(&WriteToStreamCoroutine::act);
  return yieldTo(&WriteToStreamCoroutine::writeCurrData);
}

async::Action ChunkedBufferBody::WriteToStreamCoroutine::writeEndOfChunks() {
  m_inlineWriteData.set("0\r\n\r\n", 5);
  m_nextAction = finish();
  return yieldTo(&WriteToStreamCoroutine::writeCurrData);
}

async::Action ChunkedBufferBody::WriteToStreamCoroutine::writeCurrData() {
  return oatpp::data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_inlineWriteData, Action::clone(m_nextAction));
}

oatpp::async::CoroutineStarter ChunkedBufferBody::writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) {
  if(m_chunked) {
    return WriteToStreamCoroutine::start(shared_from_this(), stream);
  } else {
    return m_buffer->flushToStreamAsync(stream);
  }
}

}}}}}
