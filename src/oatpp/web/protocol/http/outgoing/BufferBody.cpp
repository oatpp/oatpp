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

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

BufferBody::BufferBody(const oatpp::String &buffer, const data::share::StringKeyLabel &contentType)
  : m_buffer(buffer ? buffer : "")
  , m_contentType(contentType)
  , m_inlineData((void*) m_buffer->data(), m_buffer->size())
{}

std::shared_ptr<BufferBody> BufferBody::createShared(const oatpp::String &buffer,
                                                     const data::share::StringKeyLabel &contentType) {
  return std::make_shared<BufferBody>(buffer, contentType);
}

v_io_size BufferBody::read(void *buffer, v_buff_size count, async::Action &action) {

  (void) action;

  v_buff_size desiredToRead = m_inlineData.bytesLeft;

  if (desiredToRead > 0) {

    if (desiredToRead > count) {
      desiredToRead = count;
    }

    std::memcpy(buffer, m_inlineData.currBufferPtr, desiredToRead);
    m_inlineData.inc(desiredToRead);

    return desiredToRead;

  }

  return 0;

}

void BufferBody::declareHeaders(Headers &headers) {
  if (m_contentType) {
    headers.putIfNotExists(Header::CONTENT_TYPE, m_contentType);
  }
}

p_char8 BufferBody::getKnownData() {
  return (p_char8) m_buffer->data();
}

v_int64 BufferBody::getKnownSize() {
  return m_buffer->size();
}

}}}}}
