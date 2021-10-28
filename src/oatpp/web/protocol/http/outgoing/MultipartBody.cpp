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

#include "MultipartBody.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

v_io_size MultipartBody::readBody(void *buffer, v_buff_size count, async::Action& action) {
  const auto& stream = m_iterator.getPartInputStream();
  if(!stream) {
    OATPP_LOGW("[oatpp::web::protocol::http::outgoing::MultipartBody::MultipartReadCallback::readBody()]", "Warning. Part has no input stream.");
    return 0;
  }
  return stream->read(buffer, count, action);
}

v_io_size MultipartBody::incPart(async::Action& action) {
  m_iterator.inc(action);
  if(action.isNone()) {
    return 0;
  }
  return oatpp::IOError::RETRY_READ;
}

v_io_size MultipartBody::read(void *buffer, v_buff_size count, async::Action& action) {

  if(m_state == STATE_FINISHED) {
    return 0;
  }

  m_iterator.init(action);
  if(!action.isNone()) {
    return 0;
  }

  p_char8 currBufferPtr = (p_char8) buffer;
  v_io_size bytesLeft = count;

  v_io_size res = 0;

  while(bytesLeft > 0 && action.isNone()) {

    switch (m_state) {

      case STATE_BOUNDARY:
        res = readBoundary(m_multipart, m_iterator, m_readStream, currBufferPtr, bytesLeft);
        break;

      case STATE_HEADERS:
        res = readHeaders(m_multipart, m_iterator, m_readStream, currBufferPtr, bytesLeft);
        break;

      case STATE_BODY:
        res = readBody(currBufferPtr, bytesLeft, action);
        break;

      case STATE_INC_PART:
        res = incPart(action);
        break;

      default:
        OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::MultipartReadCallback::read()]", "Error. Invalid state %d", m_state);
        return 0;

    }

    if(res > 0) {
      currBufferPtr = &currBufferPtr[res];
      bytesLeft -= res;
    } else if(res == 0) {

      if(m_state == STATE_BOUNDARY && m_iterator.finished()) {
        m_state = STATE_FINISHED;
        break;
      }

      m_state += 1;

      if(m_state == STATE_INC_PART && m_flushParts && bytesLeft < count) {
        break;
      }

      if(m_state == STATE_ROUND) {
        m_state = 0;
      }

    } else if(action.isNone()) {
      OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::MultipartReadCallback::read()]", "Error. Invalid read result %d. State=%d", res, m_state);
      return 0;
    }

  }

  return count - bytesLeft;
}

v_io_size MultipartBody::readBoundary(const std::shared_ptr<Multipart>& multipart,
                                      PartIterator& iterator,
                                      data::stream::BufferInputStream& readStream,
                                      void *buffer,
                                      v_buff_size count)
{
  if (!readStream.getDataMemoryHandle()) {

    oatpp::String boundary;

    if (iterator.finished()) {
      boundary = "\r\n--" + multipart->getBoundary() + "--\r\n";
    } else if (iterator.isFirst()) {
      boundary = "--" + multipart->getBoundary() + "\r\n";
    } else {
      boundary = "\r\n--" + multipart->getBoundary() + "\r\n";
    }

    readStream.reset(boundary.getPtr(), (p_char8) boundary->data(), boundary->size());

  }

  auto res = readStream.readSimple(buffer, count);
  if(res == 0) {
    readStream.reset();
  }

  return res;
}

v_io_size MultipartBody::readHeaders(const std::shared_ptr<Multipart>& multipart,
                                     PartIterator& iterator,
                                     data::stream::BufferInputStream& readStream,
                                     void *buffer,
                                     v_buff_size count)
{
  (void) multipart;

  if (!readStream.getDataMemoryHandle()) {

    oatpp::data::stream::BufferOutputStream stream;
    const auto& part = iterator.get();
    http::Utils::writeHeaders(part->getHeaders(), &stream);
    stream.writeSimple("\r\n", 2);
    auto str = stream.toString();
    readStream.reset(str.getPtr(), (p_char8) str->data(), str->size());

  }

  auto res = readStream.readSimple(buffer, count);
  if(res == 0) {
    readStream.reset();
  }

  return res;
}

MultipartBody::MultipartBody(const std::shared_ptr<Multipart>& multipart, const oatpp::String& contentType, bool flushParts)
  : m_multipart(multipart)
  , m_contentType(contentType)
  , m_iterator(multipart)
  , m_state(STATE_BOUNDARY)
  , m_readStream(nullptr, nullptr, 0)
  , m_flushParts(flushParts)
{}

void MultipartBody::declareHeaders(Headers& headers) {
  headers.put_LockFree(oatpp::web::protocol::http::Header::CONTENT_TYPE, m_contentType + "; boundary=" + m_multipart->getBoundary());
}

p_char8 MultipartBody::getKnownData() {
  return nullptr;
}

v_int64 MultipartBody::getKnownSize() {
 return -1;
}

}}}}}
