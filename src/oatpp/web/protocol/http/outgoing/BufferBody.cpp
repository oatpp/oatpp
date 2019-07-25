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

#include "BufferBody.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

BufferBody::WriteToStreamCoroutine::WriteToStreamCoroutine(const std::shared_ptr<BufferBody>& body,
                                                           const std::shared_ptr<OutputStream>& stream)
  : m_body(body)
  , m_stream(stream)
  , m_inlineData(m_body->m_buffer->getData(), m_body->m_buffer->getSize())
{}

async::Action BufferBody::WriteToStreamCoroutine::act() {
  return oatpp::data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_inlineData, finish());
}

BufferBody::BufferBody(const oatpp::String& buffer)
  : m_buffer(buffer)
{}

std::shared_ptr<BufferBody> BufferBody::createShared(const oatpp::String& buffer) {
  return Shared_Http_Outgoing_BufferBody_Pool::allocateShared(buffer);
}

void BufferBody::declareHeaders(Headers& headers) noexcept {
  headers[oatpp::web::protocol::http::Header::CONTENT_LENGTH] = oatpp::utils::conversion::int32ToStr(m_buffer->getSize());
}

void BufferBody::writeToStream(OutputStream* stream) noexcept {
  oatpp::data::stream::writeExactSizeData(stream, m_buffer->getData(), m_buffer->getSize());
}


oatpp::async::CoroutineStarter BufferBody::writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) {
  return WriteToStreamCoroutine::start(shared_from_this(), stream);
}

}}}}}