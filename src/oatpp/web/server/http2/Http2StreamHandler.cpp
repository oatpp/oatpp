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
    if (pad > streamPayloadLength) {
      m_task->setState(ABORTED);
      throw protocol::http2::error::Http2ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Padding length longer than remaining data.");
    }
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
    if (pad > streamPayloadLength) {
      m_task->setState(ABORTED);
      throw protocol::http2::error::Http2ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Padding length longer than remaining header-data.");
    }
  }

  if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_PRIORITY) {
    stream->readExactSizeDataSimple(&m_task->dependency, 4);
    m_task->dependency = ntohl(m_task->dependency);
    if (m_task->dependency == m_task->streamId) {
      throw protocol::http2::error::Http2ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Received header for stream that depends on itself.");
    }
    stream->readExactSizeDataSimple(&m_task->weight, 1);
    streamPayloadLength -= 5;
  }

  v_io_size read = streamPayloadLength - pad;
  if (streamPayloadLength + m_task->header->availableToRead() > m_task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    m_task->setState(ABORTED);
    throw protocol::http2::error::Http2FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE");
  }

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
    throw protocol::http2::error::Http2FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handlePriority] Error: Frame size other than 5.");

  }
  stream->readExactSizeDataSimple(&m_task->dependency, 4);
  m_task->dependency = ntohl(m_task->dependency);
  if (m_task->dependency == m_task->streamId) {
    throw protocol::http2::error::Http2ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handlePriority] Error: Received PRIORITY frame with dependency on itself.");
  }
  stream->readExactSizeDataSimple(&m_task->weight, 1);
  streamPayloadLength -= 5;
  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleResetStream(v_uint8 flags,
                                                                          const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                          v_io_size streamPayloadLength) {
  if (streamPayloadLength != 4) {
    throw protocol::http2::error::Http2FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleResetStream] Error: Frame size other than 4.");
  }
  v_uint32 code;
  stream->readExactSizeDataSimple(&code, 4);
  OATPP_LOGD(TAG, "Resetting stream, code %u", ntohl(code))
  if (m_task->state == PROCESSING) {
    m_task->setState(RESET);
    return Http2StreamHandler::ConnectionState::CLOSING;
  }
  m_task->setState(ABORTED);
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
    throw protocol::http2::error::Http2FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleWindowUpdate] Error: Frame size other than 4.");
  }
  // https://datatracker.ietf.org/doc/html/rfc7540#section-6.9
  // 1 to 2^31-1 (2,147,483,647)
  v_uint32 increment;
  stream->readExactSizeDataSimple(&increment, 4);
  increment = ntohl(increment);
  if (increment == 0) {
    throw protocol::http2::error::Http2ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleWindowUpdate] Error: Increment of 0");
  }
  OATPP_LOGD(TAG, "Incrementing window by %u", increment);
  m_task->windowIncrement = increment;

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
    ENUM2STR(RESPONDING);
    ENUM2STR(DONE);
    ENUM2STR(RESET);
    ENUM2STR(ABORTED);
    ENUM2STR(ERROR);
  }
#undef ENUM2STR
  return nullptr;
}

bool Http2StreamHandler::Task::setStateWithExpection(Http2StreamHandler::H2StreamState expected, Http2StreamHandler::H2StreamState next) {
  if (state.compare_exchange_strong(expected, next)) {
    OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::Task::setStateWithExpection", "(%d) State: %s(%d) -> %s(%d)", streamId, stateStringRepresentation(expected), expected, stateStringRepresentation(next), next);
    return true;
  }
  return false;
}

void Http2StreamHandler::Task::setState(Http2StreamHandler::H2StreamState next) {
  OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::Task::setState", "(%d) State: %s(%d) -> %s(%d)", streamId, stateStringRepresentation(state.load()), state.load(), stateStringRepresentation(next), next);
  state.store(next);
}

void Http2StreamHandler::Task::clean() {
  hpack = nullptr;
  components = nullptr;
  output = nullptr;
  inSettings = nullptr;
  outSettings = nullptr;
  data = nullptr;
  header = nullptr;
  headerFlags = 0;
}

void Http2StreamHandler::process(std::shared_ptr<Task> task) {
  char TAG[68];
  snprintf(TAG, 64, "oatpp::web::server::http2::Http2StreamHandler(%d)::process", task->streamId);

  protocol::http2::Headers requestHeaders;
  try {
    requestHeaders = task->hpack->inflate(task->header, task->header->availableToRead());
  } catch (std::runtime_error &e) {
    task->error = std::make_exception_ptr(protocol::http2::error::Http2CompressionError(e.what()));
    task->setState(ERROR);

    // ToDo HAXX
    //   This is the only reason we want to close an connection from an stream handler
    //   I need to find a better way to handle that instead of sending a GOAWAY frame and hoping for the other peer
    //   to disconnect.
    protocol::http2::Frame::Header header(8, 0, protocol::http2::Frame::Header::FrameType::GOAWAY, 0);
    OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId());
    v_uint32 lastStream = htonl(task->streamId);
    v_uint32 errorCode = htonl(protocol::http2::error::ErrorCode::COMPRESSION_ERROR);
    task->output->lock(PriorityStreamScheduler::PRIORITY_MAX);
    if (task->state.load() == H2StreamState::RESET) {
      task->output->unlock();
      finalizeProcessAbortion(task);
      return;
    }
    header.writeToStream(task->output.get());
    task->output->writeExactSizeDataSimple(&lastStream, 4);
    task->output->writeExactSizeDataSimple(&errorCode, 4);
    task->output->unlock();

    finalizeProcessAbortion(task);
    task->setState(ERROR);
    return;
  }

  if (task->state.load() == H2StreamState::RESET) {
    finalizeProcessAbortion(task);
    return;
  }

  protocol::http::RequestStartingLine startingLine;
  startingLine.protocol = "HTTP/2.0";
  startingLine.path = requestHeaders.get(protocol::http2::Header::PATH);
  startingLine.method = requestHeaders.get(protocol::http2::Header::METHOD);

  auto request = protocol::http::incoming::Request::createShared(nullptr, startingLine, requestHeaders, task->data, task->components->bodyDecoder);

  ConnectionState connectionState;
  std::shared_ptr<protocol::http::outgoing::Response> response;

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
    finalizeProcessAbortion(task);
    return;
  }

  auto responseHeaders = response->getHeaders();
  responseHeaders.putIfNotExists(protocol::http2::Header::STATUS, utils::conversion::uint32ToStr(response->getStatus().code));

  while (task->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE) + task->windowIncrement == 0) {
    std::this_thread::yield();
  }

  auto headerBlocks = task->hpack->deflate(responseHeaders, task->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE) + task->windowIncrement);


  if (!task->setStateWithExpection(H2StreamState::PROCESSING, H2StreamState::RESPONDING)) {
    finalizeProcessAbortion(task);
    return;
  }


  if (headerBlocks.size() == 1) {
    auto &block = headerBlocks.front();
    protocol::http2::Frame::Header hdr(block.size(), protocol::http2::Frame::Header::Flags::Header::HEADER_END_HEADERS | (response->getBody()->getKnownSize() == 0 ? protocol::http2::Frame::Header::Flags::Header::HEADER_END_STREAM : 0), protocol::http2::Frame::Header::HEADERS, task->streamId);
    task->output->lock(task->weight);
    OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
    if (task->state.load() == H2StreamState::RESET) {
      task->output->unlock();
      finalizeProcessAbortion(task);
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
        finalizeProcessAbortion(task);
        return;
      }
      hdr.writeToStream(task->output.get());
      task->output->writeSimple(it->data(), it->size());
      task->output->unlock();
    }
    while (it != last) {
      protocol::http2::Frame::Header hdr(it->size(), 0, protocol::http2::Frame::Header::CONTINUATION, task->streamId);
      task->output->lock(task->weight);
      OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      if (task->state.load() == H2StreamState::RESET) {
        task->output->unlock();
        finalizeProcessAbortion(task);
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
        finalizeProcessAbortion(task);
        return;
      }
      hdr.writeToStream(task->output.get());
      task->output->writeSimple(it->data(), it->size());
      task->output->unlock();
    }
  }


  if (response->getBody()->getKnownSize() > 0 && response->getBody()->getKnownData() != nullptr) {
    const v_buff_size toSend = response->getBody()->getKnownSize();
    auto body = response->getBody();
    for (v_buff_size send = 0; send < toSend;) {
      v_buff_size chunk = std::min((v_buff_size)task->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE) + task->windowIncrement, toSend - send);
      protocol::http2::Frame::Header hdr(chunk, (toSend - send - chunk == 0) ? protocol::http2::Frame::Header::Flags::Data::DATA_END_STREAM : 0, protocol::http2::Frame::Header::DATA, task->streamId);
      task->output->lock(task->weight);
      OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      if (task->state.load() == H2StreamState::RESET) {
        task->output->unlock();
        finalizeProcessAbortion(task);
        return;
      }
      hdr.writeToStream(task->output.get());
      task->output->writeSimple(body->getKnownData() + send, chunk);
      send += chunk;
      task->output->unlock();
    }
  }
  task->setState(H2StreamState::DONE);
  task->clean();
}

void Http2StreamHandler::finalizeProcessAbortion(const std::shared_ptr<Task> &task) {
  task->setState(H2StreamState::ABORTED);
  task->clean();
}

void Http2StreamHandler::abort() {
  if (m_task->state.load() == PROCESSING) {
    m_task->setState(RESET);
  }
}

void Http2StreamHandler::waitForFinished() {
  if (m_processor.joinable()) {
    m_processor.join();
  }
}

void Http2StreamHandler::clean() {
  if (m_task->state.load() < PROCESSING || m_task->state.load() > RESET) {
    m_task->clean();
  }
}

}}}}