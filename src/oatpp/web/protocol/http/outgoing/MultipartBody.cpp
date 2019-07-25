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
        res = readBoundary(m_multipart, m_iterator, m_readStream, currBufferPtr, bytesLeft);
        break;

      case STATE_HEADERS:
        res = readHeaders(m_multipart, m_iterator, m_readStream, currBufferPtr, bytesLeft);
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
  , m_iterator(multipart->getAllParts().begin())
  , m_state(STATE_BOUNDARY)
  , m_readStream(nullptr, nullptr, 0)
{}

oatpp::async::Action MultipartBody::AsyncMultipartReadCallback::readAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                                                oatpp::data::stream::AsyncInlineReadData& inlineData,
                                                                                oatpp::async::Action&& nextAction)
{

  class ReadCoroutine : public oatpp::async::Coroutine<ReadCoroutine> {
  private:
    AsyncMultipartReadCallback* m_this;
    oatpp::data::stream::AsyncInlineReadData* m_inlineData;
    data::v_io_size m_readResult;
    data::v_io_size m_bodyReadPosition0;
  public:

    ReadCoroutine(AsyncMultipartReadCallback* _this, oatpp::data::stream::AsyncInlineReadData* inlineData)
      : m_this(_this)
      , m_inlineData(inlineData)
    {}

    Action act() override {

      if(m_inlineData->bytesLeft == 0 || m_this->m_state == STATE_FINISHED) {
        return finish();
      }

      m_readResult = 0;

      switch (m_this->m_state) {

        case STATE_BOUNDARY:
          m_readResult = readBoundary(m_this->m_multipart, m_this->m_iterator, m_this->m_readStream, m_inlineData->currBufferPtr, m_inlineData->bytesLeft);
          return yieldTo(&ReadCoroutine::processReadResult);

        case STATE_HEADERS:
          m_readResult = readHeaders(m_this->m_multipart, m_this->m_iterator, m_this->m_readStream, m_inlineData->currBufferPtr, m_inlineData->bytesLeft);
          return yieldTo(&ReadCoroutine::processReadResult);

        case STATE_BODY:
          return yieldTo(&ReadCoroutine::processBody);

      }

      OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::AsyncMultipartReadCallback::readAsyncInline(){}]", "Error. Invalid state %d", m_this->m_state);
      throw std::runtime_error("[oatpp::web::protocol::http::outgoing::MultipartBody::AsyncMultipartReadCallback::readAsyncInline(){}]: Error. Invalid state.");

    }

    Action processBody() {

      auto& part = *m_this->m_iterator;
      const auto& stream = part->getInputStream();
      if(!stream) {
        OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::AsyncMultipartReadCallback::readAsyncInline(){}]", "Error. Part has no input stream", m_this->m_state);
        m_this->m_iterator ++;
        m_readResult = 0;
        return yieldTo(&ReadCoroutine::processReadResult);
      }

      m_bodyReadPosition0 = m_inlineData->bytesLeft;

      return yieldTo(&ReadCoroutine::readBody);

    }

    Action readBody() {
      auto& part = *m_this->m_iterator;
      return oatpp::data::stream::readSomeDataAsyncInline(this, part->getInputStream().get(), *m_inlineData, yieldTo(&ReadCoroutine::afterBodyRead), true);
    }

    Action afterBodyRead() {

      m_readResult = m_bodyReadPosition0 - m_inlineData->bytesLeft;

      if(m_readResult > 0) {
        return yieldTo(&ReadCoroutine::act);
      } else if(m_readResult == 0) {
        m_this->m_iterator ++;
      }

      return yieldTo(&ReadCoroutine::processReadResult);

    }

    Action processReadResult() {

      if(m_readResult > 0) {
        m_inlineData->inc(m_readResult);
      } else if(m_readResult == 0) {

        if(m_this->m_state == STATE_BOUNDARY && m_this->m_iterator == m_this->m_multipart->getAllParts().end()) {
          m_this->m_state = STATE_FINISHED;
          return finish();
        }

        m_this->m_state += 1;
        if(m_this->m_state == STATE_ROUND) {
          m_this->m_state = 0;
        }

      } else {
        OATPP_LOGE("[oatpp::web::protocol::http::outgoing::MultipartBody::AsyncMultipartReadCallback::readAsyncInline(){}]", "Error. Invalid read result %d. State=%d", m_readResult, m_this->m_state);
        throw std::runtime_error("[oatpp::web::protocol::http::outgoing::MultipartBody::AsyncMultipartReadCallback::readAsyncInline(){}]: Error. Invalid read result.");
      }

      return yieldTo(&ReadCoroutine::act);

    }

  };

  if(m_state == STATE_FINISHED) {
    return std::forward<oatpp::async::Action>(nextAction);
  }

  return ReadCoroutine::start(this, &inlineData).next(std::forward<oatpp::async::Action>(nextAction));

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MultipartBody

data::v_io_size MultipartBody::readBoundary(const std::shared_ptr<Multipart>& multipart,
                                            std::list<std::shared_ptr<Part>>::const_iterator& iterator,
                                            data::stream::BufferInputStream& readStream,
                                            void *buffer,
                                            data::v_io_size count)
{
  if (!readStream.getDataMemoryHandle()) {

    oatpp::String boundary;

    if (iterator == multipart->getAllParts().end()) {
      boundary = "\r\n--" + multipart->getBoundary() + "--\r\n";
    } else if (iterator == multipart->getAllParts().begin()) {
      boundary = "--" + multipart->getBoundary() + "\r\n";
    } else {
      boundary = "\r\n--" + multipart->getBoundary() + "\r\n";
    }

    readStream.reset(boundary.getPtr(), boundary->getData(), boundary->getSize());

  }

  auto res = readStream.read(buffer, count);
  if(res == 0) {
    readStream.reset();
  }

  return res;
}

data::v_io_size MultipartBody::readHeaders(const std::shared_ptr<Multipart>& multipart,
                                           std::list<std::shared_ptr<Part>>::const_iterator& iterator,
                                           data::stream::BufferInputStream& readStream,
                                           void *buffer,
                                           data::v_io_size count)
{
  if (!readStream.getDataMemoryHandle()) {

    oatpp::data::stream::ChunkedBuffer stream;
    auto& part = *iterator;
    http::Utils::writeHeaders(part->getHeaders(), &stream);
    stream.write("\r\n", 2);
    auto str = stream.toString();
    readStream.reset(str.getPtr(), str->getData(), str->getSize());

  }

  auto res = readStream.read(buffer, count);
  if(res == 0) {
    readStream.reset();
  }

  return res;
}

MultipartBody::MultipartBody(const std::shared_ptr<Multipart>& multipart, data::v_io_size chunkBufferSize)
  : ChunkedBody(std::make_shared<MultipartReadCallback>(multipart),
                std::make_shared<AsyncMultipartReadCallback>(multipart),
                chunkBufferSize)
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
