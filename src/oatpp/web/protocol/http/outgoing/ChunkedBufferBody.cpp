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

ChunkedBufferBody::ChunkedBufferBody(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer)
  : m_buffer(buffer)
{}

std::shared_ptr<ChunkedBufferBody> ChunkedBufferBody::createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer) {
  return Shared_Http_Outgoing_ChunkedBufferBody_Pool::allocateShared(buffer);
}

void ChunkedBufferBody::declareHeaders(Headers& headers) noexcept {
  headers.put(oatpp::web::protocol::http::Header::CONTENT_LENGTH, oatpp::utils::conversion::int64ToStr(m_buffer->getSize()));
}

void ChunkedBufferBody::writeToStream(OutputStream* stream) noexcept {
  m_buffer->flushToStream(stream);
}

oatpp::async::CoroutineStarter ChunkedBufferBody::writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) {
  return m_buffer->flushToStreamAsync(stream);
}

v_buff_size ChunkedBufferBody::getKnownSize() {
  return m_buffer->getSize();
}

}}}}}
