/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include <arpa/inet.h>
#include <thread>

#include "Http2StreamHandler.hpp"

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

Http2StreamHandler::ConnectionState Http2StreamHandler::handleData(v_uint8 flags,
                                                                   const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                   v_io_size streamPayloadLength) {
  m_task->setState(H2StreamState::PAYLOAD);
  v_uint8 pad = 0;
  if (flags & H2StreamDataFlags::DATA_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength--;
  }

  v_io_size read = streamPayloadLength - pad;

  if (streamPayloadLength + m_task->data->availableToRead() > m_task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    m_task->setState(ABORTED);
    throw protocol::http2::error::Http2FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE");
  }

  async::Action action;
  do {
    v_io_size res = m_task->data->readFromStreamAndWrite(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      OATPP_LOGD(TAG, "Could not read data: Broken Pipe");
      return Http2StreamHandler::ConnectionState::DEAD;
    } else {
      std::this_thread::yield();
    }
  } while (read > 0);


  if (pad > 0) {
    v_uint8 paddata[pad];
    stream->readExactSizeDataSimple((void*)paddata, pad);
  }

  if (flags & H2StreamDataFlags::DATA_END_STREAM) {
    m_task->setState(H2StreamState::PROCESSING);
    m_processor = std::thread(Http2StreamHandler::process, m_task);
  }

  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleHeaders(v_uint8 flags,
                                                                      const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                      v_io_size streamPayloadLength) {
  m_task->setState(H2StreamState::HEADERS);
  m_task->headerFlags |= flags;
  v_uint8 pad = 0;

  if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength -= 1;
  }

  if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_PRIORITY) {
    stream->readExactSizeDataSimple(&m_task->dependency, 4);
    stream->readExactSizeDataSimple(&m_task->weight, 1);
    streamPayloadLength -= 5;
  }

  v_io_size read = streamPayloadLength - pad;
  async::Action action;
  do {
    v_io_size res = m_task->header->readFromStreamAndWrite(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      OATPP_LOGD(TAG, "Could not read header: Broken Pipe");
      return Http2StreamHandler::ConnectionState::DEAD;
    } else {
      std::this_thread::yield();
    }
  } while (read > 0);

  if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) { // end stream, go to processing
    OATPP_LOGD(TAG, "Stream and headers finished, start processing.");
    m_task->setState(H2StreamState::PROCESSING);
    m_processor = std::thread(Http2StreamHandler::process, m_task);
  } else if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_END_STREAM) { // continuation
    OATPP_LOGD(TAG, "Stream finished, headers not, awaiting continuation.");
    m_task->setState(H2StreamState::CONTINUATION);
  } else if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_END_HEADERS) { // payload
    m_task->setState(H2StreamState::PAYLOAD);
    OATPP_LOGD(TAG, "Headers finished, stream not, awaiting data.");
  } else {
    OATPP_LOGE(TAG, "Something is wrong");
  }

  if (pad > 0) {
    oatpp::String paddata((v_buff_size)pad);
    stream->readExactSizeDataSimple((void*)paddata->data(), paddata->size());
  }

  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handlePriority(v_uint8 flags,
                                                                       const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                       v_io_size streamPayloadLength) {
  if (streamPayloadLength != 5) {
    // ToDo: stream error (Section 5.4.2) of type FRAME_SIZE_ERROR.
  }
  stream->readExactSizeDataSimple(&m_task->dependency, 4);
  stream->readExactSizeDataSimple(&m_task->weight, 1);
  streamPayloadLength -= 5;
  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleResetStream(v_uint8 flags,
                                                                          const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                          v_io_size streamPayloadLength) {
  if (streamPayloadLength != 4) {
    // ToDo: A RST_STREAM frame with a length other than 4 octets MUST be treated
    //   as a connection error (Section 5.4.1) of type FRAME_SIZE_ERROR.
  }
  v_uint32 code;
  stream->readExactSizeDataSimple(&code, 4);
  OATPP_LOGD(TAG, "Resetting stream, code %u", ntohl(code))
  if (m_task->state == PROCESSING) {
    m_task->setState(RESET);
    return Http2StreamHandler::ConnectionState::CLOSING;
  }
  m_task->setState(RESET);
  return Http2StreamHandler::ConnectionState::DEAD;
}


Http2StreamHandler::ConnectionState Http2StreamHandler::handlePushPromise(v_uint8 flags,
                                                                          const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                          v_io_size streamPayloadLength) {
  return Http2StreamHandler::ConnectionState::CLOSING;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleWindowUpdate(v_uint8 flags,
                                                                           const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                           v_io_size streamPayloadLength) {
  if (streamPayloadLength != 4) {
    // ToDo: A WINDOW_UPDATE frame with a length other than 4 octets MUST be
    //   treated as a connection error (Section 5.4.1) of type
    //   FRAME_SIZE_ERROR.
  }
  // https://datatracker.ietf.org/doc/html/rfc7540#section-6.9
  // 1 to 2^31-1 (2,147,483,647)
  v_uint32 increment;
  stream->readExactSizeDataSimple(&increment, 4);
  OATPP_LOGD(TAG, "Incrementing window by %u", ntohl(increment));
  m_task->data->reserveBytesUpfront(ntohl(increment));

  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleContinuation(v_uint8 flags,
                                                                           const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                           v_io_size streamPayloadLength) {

  m_task->headerFlags |= flags;

  v_io_size read = streamPayloadLength;
  async::Action action;
  do {
    v_io_size res = m_task->header->readFromStreamAndWrite(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      OATPP_LOGD(TAG, "Could not read header: Broken Pipe");
      return Http2StreamHandler::ConnectionState::DEAD;
    } else {
      std::this_thread::yield();
    }
  } while (read > 0);

  if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == H2StreamHeaderFlags::HEADER_END_HEADERS) {
    m_task->setState(H2StreamState::PAYLOAD);
  } else if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) {
    m_task->setState(H2StreamState::PROCESSING);
    m_processor = std::thread(Http2StreamHandler::process, m_task);
  }

  return Http2StreamHandler::ConnectionState::ALIVE;
}

const char *Http2StreamHandler::stateStringRepresentation(Http2StreamHandler::H2StreamState state) {
#define ENUM2STR(x) case x: return #x
  switch (state) {
    ENUM2STR(INIT);
    ENUM2STR(HEADERS);
    ENUM2STR(CONTINUATION);
    ENUM2STR(PAYLOAD);
    ENUM2STR(PROCESSING);
    ENUM2STR(RESPONSE);
    ENUM2STR(GOAWAY);
    ENUM2STR(RESET);
  }
#undef ENUM2STR
  return nullptr;
}

void Http2StreamHandler::Task::setState(Http2StreamHandler::H2StreamState next) {
//  OATPP_LOGD(TAG, "State: %s -> %s", stateStringRepresentation(m_task->state.load()), stateStringRepresentation(next))
  state.store(next);
}

void Http2StreamHandler::process(std::shared_ptr<Task> task) {
  char TAG[68];
  snprintf(TAG, 64, "oatpp::web::server::http2::Http2StreamHandler(%d)::process", task->streamId);

  auto requestHeaders = task->hpack->inflate(task->header, task->header->availableToRead());

  protocol::http::RequestStartingLine startingLine;
  startingLine.protocol = "HTTP/2.0";
  startingLine.path = requestHeaders.get(protocol::http2::Header::PATH);
  startingLine.method = requestHeaders.get(protocol::http2::Header::METHOD);

  auto request = protocol::http::incoming::Request::createShared(nullptr, startingLine, requestHeaders, task->data, task->components->bodyDecoder);

  ConnectionState connectionState;
  std::shared_ptr<protocol::http::outgoing::Response> response;

  task->setState(H2StreamState::PROCESSING);

  try {

    // ToDo: Interceptors

    auto route = task->components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);

    if(!route) {

      data::stream::BufferOutputStream ss;
      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
         << "', URL: '" << request->getStartingLine().path.toString() << "'";

      connectionState = ConnectionState::CLOSING;
      response = task->components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
    } else {
      request->setPathVariables(route.getMatchMap());
      response = route.getEndpoint()->handle(request);
    }

  } catch (oatpp::web::protocol::http::HttpError& error) {
    response = task->components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
    connectionState = ConnectionState::CLOSING;
  } catch (std::exception& error) {
    response = task->components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    connectionState = ConnectionState::CLOSING;
  } catch (...) {
    response = task->components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
    connectionState = ConnectionState::CLOSING;
  }

  if (task->state.load() == H2StreamState::RESET) {
    task->setState(H2StreamState::ABORTED);
    return;
  }

  auto responseHeaders = response->getHeaders();
  responseHeaders.putIfNotExists(protocol::http2::Header::STATUS, utils::conversion::uint32ToStr(response->getStatus().code));
  auto headerBlocks = task->hpack->deflate(responseHeaders, task->flow); // ToDo: Framesize != FlowSize

  if (headerBlocks.size() == 1) {
    auto &block = headerBlocks.front();
    protocol::http2::Frame::Header hdr(block.size(), protocol::http2::Frame::Header::Flags::Header::HEADER_END_HEADERS | (response->getBody()->getKnownSize() == 0 ? protocol::http2::Frame::Header::Flags::Header::HEADER_END_STREAM : 0), protocol::http2::Frame::Header::HEADERS, task->streamId);
    task->output->lock(task->weight);
    OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
    if (task->state.load() == H2StreamState::RESET) {
      task->output->unlock();
      task->setState(H2StreamState::ABORTED);
      return;
    }
    hdr.writeToStream(task->output.get());
    task->output->writeSimple(block.data(), block.size());
    task->output->unlock();
  } else {
    auto it = headerBlocks.begin();
    auto last = --headerBlocks.end();
    {
      protocol::http2::Frame::Header hdr(it->size(), 0, protocol::http2::Frame::Header::HEADERS, task->streamId);
      task->output->lock(task->weight);
      OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      if (task->state.load() == H2StreamState::RESET) {
        task->output->unlock();
        task->setState(H2StreamState::ABORTED);
        return;
      }
      hdr.writeToStream(task->output.get());
      task->output->writeSimple(it->data(), it->size());
      task->output->unlock();
    }
    while(it != last) {
      protocol::http2::Frame::Header hdr(it->size(), 0, protocol::http2::Frame::Header::CONTINUATION, task->streamId);
      task->output->lock(task->weight);
      OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      if (task->state.load() == H2StreamState::RESET) {
        task->output->unlock();
        task->setState(H2StreamState::ABORTED);
        return;
      }
      hdr.writeToStream(task->output.get());
      task->output->writeSimple(it->data(), it->size());
      task->output->unlock();
      ++it;
    }
    {
      protocol::http2::Frame::Header hdr(it->size(), protocol::http2::Frame::Header::Flags::Header::HEADER_END_HEADERS | (response->getBody()->getKnownSize() == 0 ? protocol::http2::Frame::Header::Flags::Header::HEADER_END_STREAM : 0), protocol::http2::Frame::Header::CONTINUATION, task->streamId);
      task->output->lock(task->weight);
      OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      if (task->state.load() == H2StreamState::RESET) {
        task->output->unlock();
        task->setState(H2StreamState::ABORTED);
        return;
      }
      hdr.writeToStream(task->output.get());
      task->output->writeSimple(it->data(), it->size());
      task->output->unlock();
    }
  }

  // ToDo: Segmentation/Framing
  if (response->getBody()->getKnownSize() > 0 && response->getBody()->getKnownData() != nullptr) {
    protocol::http2::Frame::Header hdr(response->getBody()->getKnownSize(), protocol::http2::Frame::Header::Flags::Data::DATA_END_STREAM, protocol::http2::Frame::Header::DATA, task->streamId);
    task->output->lock(task->weight);
    OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
    if (task->state.load() == H2StreamState::RESET) {
      task->output->unlock();
      task->setState(H2StreamState::ABORTED);
      return;
    }
    hdr.writeToStream(task->output.get());
    task->output->writeSimple(response->getBody()->getKnownData(), response->getBody()->getKnownSize());
    task->output->unlock();
  }
  task->setState(H2StreamState::RESPONSE);
}

void Http2StreamHandler::abort() {
  if (m_task->state.load() == PROCESSING) {
    m_task->setState(RESET);
  } else {
    m_task->setState(ABORTED);
  }
}

void Http2StreamHandler::waitForFinished() {
  if (m_processor.joinable()) {
    m_processor.join();
  }
}

}}}}