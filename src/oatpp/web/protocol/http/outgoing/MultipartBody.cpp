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

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MultipartReadCallback

MultipartBody::MultipartReadCallback::MultipartReadCallback(const std::shared_ptr<Multipart>& multipart)
  : m_multipart(multipart)
  , m_iterator(multipart->getAllParts().begin())
  , m_state(STATE_BOUNDARY)
  , m_readStream(nullptr, nullptr, 0)
{}

data::v_io_size MultipartBody::MultipartReadCallback::readBoundary(void *buffer, data::v_io_size count) {

  if (!m_readStream.getDataMemoryHandle()) {

    oatpp::String boundary;

    if (m_iterator == m_multipart->getAllParts().end()) {
      boundary = "\r\n--" + m_multipart->getBoundary() + "--\r\n";
    } else if (m_iterator == m_multipart->getAllParts().begin()) {
      boundary = "--" + m_multipart->getBoundary() + "\r\n";
    } else {
      boundary = "\r\n--" + m_multipart->getBoundary() + "\r\n";
    }

    m_readStream.reset(boundary.getPtr(), boundary->getData(), boundary->getSize());

  }

  auto res = m_readStream.read(buffer, count);
  if(res == 0) {
    m_readStream.reset();
  }
  return res;

}

data::v_io_size MultipartBody::MultipartReadCallback::readHeaders(void *buffer, data::v_io_size count) {

  if (!m_readStream.getDataMemoryHandle()) {

    oatpp::data::stream::ChunkedBuffer stream;
    auto& part = *m_iterator;
    http::Utils::writeHeaders(part->getHeaders(), &stream);
    stream.write("\r\n", 2);
    auto buffer = stream.toString();
    m_readStream.reset(buffer.getPtr(), buffer->getData(), buffer->getSize());

  }

  auto res = m_readStream.read(buffer, count);
  if(res == 0) {
    m_readStream.reset();
  }
  return res;

}

data::v_io_size MultipartBody::MultipartReadCallback::readBody(void *buffer, data::v_io_size count) {
  auto& part = *m_iterator;
  const auto& stream = part->getInputStream();
  if(!stream) {
    OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::MultipartReadCallback::readBody()]", "Error. Part has no input stream", m_state);
    m_iterator ++;
    return 0;
  }
  auto res = stream->read(buffer, count);
  if(res == 0) {
    m_iterator ++;
  }
  return res;
}

data::v_io_size MultipartBody::MultipartReadCallback::read(void *buffer, data::v_io_size count) {

  if(m_state == STATE_FINISHED) {
    return 0;
  }

  p_char8 currBufferPtr = (p_char8) buffer;
  data::v_io_size bytesLeft = count;

  data::v_io_size res = 0;

  while(bytesLeft > 0) {

    switch (m_state) {

      case STATE_BOUNDARY:
        res = readBoundary(currBufferPtr, bytesLeft);
        break;

      case STATE_HEADERS:
        res = readHeaders(currBufferPtr, bytesLeft);
        break;

      case STATE_BODY:
        res = readBody(currBufferPtr, bytesLeft);
        break;

      default:
        OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::MultipartReadCallback::read()]", "Error. Invalid state %d", m_state);
        return 0;

    }

    if(res > 0) {
      currBufferPtr = &currBufferPtr[res];
      bytesLeft -= res;
    } else if(res == 0) {

      if(m_state == STATE_BOUNDARY && m_iterator == m_multipart->getAllParts().end()) {
        m_state = STATE_FINISHED;
        break;
      }

      m_state += 1;
      if(m_state == STATE_ROUND) {
        m_state = 0;
      }

    } else {
      OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::MultipartReadCallback::read()]", "Error. Invalid read result %d. State=%d", res, m_state);
      return 0;
    }

  }

  return count - bytesLeft;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncMultipartReadCallback

MultipartBody::AsyncMultipartReadCallback::AsyncMultipartReadCallback(const std::shared_ptr<Multipart>& multipart)
  : m_multipart(multipart)
{}

oatpp::async::Action MultipartBody::AsyncMultipartReadCallback::readAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                                                void*& currBufferPtr,
                                                                                data::v_io_size& bytesLeftToRead,
                                                                                oatpp::async::Action&& nextAction)
{
  return std::forward<oatpp::async::Action>(nextAction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MultipartBody

MultipartBody::MultipartBody(const std::shared_ptr<Multipart>& multipart)
  : ChunkedBody(std::make_shared<MultipartReadCallback>(multipart),
                std::make_shared<AsyncMultipartReadCallback>(multipart),
                4096)
  , m_multipart(multipart)
{}

void MultipartBody::declareHeaders(Headers& headers) noexcept {
  if(m_multipart->getAllParts().empty()) {
    headers[oatpp::web::protocol::http::Header::CONTENT_LENGTH] = "0";
    return;
  }
  ChunkedBody::declareHeaders(headers);
  headers[oatpp::web::protocol::http::Header::CONTENT_TYPE] = "multipart/form-data; boundary=" + m_multipart->getBoundary();
}

void MultipartBody::writeToStream(OutputStream* stream) noexcept {
  if(m_multipart->getAllParts().empty()) {
    return;
  }
  ChunkedBody::writeToStream(stream);
}

oatpp::async::CoroutineStarter MultipartBody::writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) {
  if(m_multipart->getAllParts().empty()) {
    return nullptr;
  }
  return ChunkedBody::writeToStreamAsync(stream);
}



}}}}}
