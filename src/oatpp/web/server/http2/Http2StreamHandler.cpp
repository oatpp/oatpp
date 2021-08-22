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
#include "oatpp/core/concurrency/Thread.hpp"


namespace oatpp { namespace web { namespace server { namespace http2 {

async::Action Http2StreamHandler::handleData(v_uint8 flags,
                                             const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                             v_io_size streamPayloadLength,
                                             async::Action &&nextAction) {
  m_task->setState(H2StreamState::PAYLOAD);
  v_uint8 pad = 0;
  if (flags & H2StreamDataFlags::DATA_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength--;
    if (pad > streamPayloadLength) {
      m_task->setState(ABORTED);
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Padding length longer than remaining data.");
    }
  }

  v_io_size read = streamPayloadLength - pad;

  if (streamPayloadLength + m_task->data->availableToRead() > m_task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    m_task->setState(ABORTED);
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE.");
  }

  // ToDo: Async
  while (read > 0) {
    async::Action action;
    v_io_size res = m_task->data->readStreamToBuffer(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      throw protocol::http2::error::connection::InternalError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Could not read data (Broken Pipe).");
    } else {
      std::this_thread::yield();
    }
  }


  if (pad > 0) {
    v_uint8 paddata[pad];
    stream->readExactSizeDataSimple((void*)paddata, pad);
  }

  if (flags & H2StreamDataFlags::DATA_END_STREAM) {
    m_task->setState(H2StreamState::PROCESSING);
    m_processor = std::thread(Http2StreamHandler::process, m_task);
  }

  return std::forward<async::Action>(nextAction);
}

async::Action Http2StreamHandler::handleHeaders(v_uint8 flags,
                                                const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                v_io_size streamPayloadLength,
                                                async::Action &&nextAction) {

  if (m_task->state == H2StreamState::PAYLOAD && (flags & H2StreamHeaderFlags::HEADER_END_STREAM) == 0) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Received HEADERS frame without the HEADER_END_STREAM flag set after DATA frames");
  }

  m_task->setState(H2StreamState::HEADERS);
  m_task->headerFlags |= flags;
  v_uint8 pad = 0;

  if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength -= 1;
    if (pad > streamPayloadLength) {
      m_task->setState(ABORTED);
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Padding length longer than remaining header-data.");
    }
  }

  if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_PRIORITY) {
    stream->readExactSizeDataSimple(&m_task->dependency, 4);
    m_task->dependency = ntohl(m_task->dependency);
    if (m_task->dependency == m_task->streamId) {
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Received header for stream that depends on itself.");
    }
    stream->readExactSizeDataSimple(&m_task->weight, 1);
    streamPayloadLength -= 5;
  }

  v_io_size read = streamPayloadLength - pad;
  if (streamPayloadLength + m_task->header->availableToRead() > m_task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    m_task->setState(ABORTED);
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE.");
  }

  // ToDo: Async
  while (read > 0) {
    async::Action action;
    v_io_size res = m_task->header->readStreamToBuffer(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      OATPP_LOGD(TAG, "Could not read header: Broken Pipe");
      throw protocol::http2::error::connection::InternalError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Could not read header (Broken Pipe)");
    } else {
      std::this_thread::yield();
    }
  }

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

  return std::forward<async::Action>(nextAction);
}

async::Action Http2StreamHandler::handlePriority(v_uint8 flags,
                                                 const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                 v_io_size streamPayloadLength,
                                                 async::Action &&nextAction) {
  if (streamPayloadLength != 5) {
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handlePriority] Error: Frame size other than 5.");

  }
  stream->readExactSizeDataSimple(&m_task->dependency, 4);
  m_task->dependency = ntohl(m_task->dependency);
  if (m_task->dependency == m_task->streamId) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handlePriority] Error: Received PRIORITY frame with dependency on itself.");
  }
  stream->readExactSizeDataSimple(&m_task->weight, 1);
  streamPayloadLength -= 5;
  return std::forward<async::Action>(nextAction);
}

async::Action Http2StreamHandler::handleResetStream(v_uint8 flags,
                                                    const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                    v_io_size streamPayloadLength,
                                                    async::Action &&nextAction) {
  if (streamPayloadLength != 4) {
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleResetStream] Error: Frame size other than 4.");
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


async::Action Http2StreamHandler::handlePushPromise(v_uint8 flags,
                                                    const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                    v_io_size streamPayloadLength,
                                                    async::Action &&nextAction) {
  return std::forward<async::Action>(nextAction);
}

async::Action Http2StreamHandler::handleWindowUpdate(v_uint8 flags,
                                                     const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                     v_io_size streamPayloadLength,
                                                     async::Action &&nextAction) {
  if (streamPayloadLength != 4) {
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleWindowUpdate] Error: Frame size other than 4.");
  }
  // https://datatracker.ietf.org/doc/html/rfc7540#section-6.9
  // 1 to 2^31-1 (2,147,483,647)
  v_uint32 increment;
  stream->readExactSizeDataSimple(&increment, 4);
  increment = ntohl(increment);
  if (increment == 0) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleWindowUpdate] Error: Increment of 0");
  }
  v_uint32 unsignedWindow = m_task->window;
  if (unsignedWindow + increment > Http2Settings::getSettingMax(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)) {
    throw protocol::http2::error::stream::FlowControlError("[oatpp::web::server::http2::Http2StreamHandler::handleWindowUpdate] Error: Increment above 2^31-1");
  }
  OATPP_LOGD(TAG, "Incrementing window by %u", increment);
  m_task->window += increment;

  return std::forward<async::Action>(nextAction);
}

async::Action Http2StreamHandler::handleContinuation(v_uint8 flags,
                                                     const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                     v_io_size streamPayloadLength,
                                                     async::Action &&nextAction) {

  m_task->headerFlags |= flags;

  // ToDo: Async
  v_io_size read = streamPayloadLength;
  while (read > 0) {
    async::Action action;
    v_io_size res = m_task->header->readStreamToBuffer(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      throw protocol::http2::error::connection::InternalError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Could not read header continuation (Broken Pipe)");
    } else {
      std::this_thread::yield();
    }
  }

  if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == H2StreamHeaderFlags::HEADER_END_HEADERS) {
    m_task->setState(H2StreamState::PAYLOAD);
  } else if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) {
    m_task->setState(H2StreamState::PROCESSING);
    m_processor = std::thread(Http2StreamHandler::process, m_task);
  }

  return std::forward<async::Action>(nextAction);
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

void Http2StreamHandler::Task::resizeWindow(v_int32 change) {
  window += change;
  OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::Task::resizeWindow", "Resized window by %d to %d", change, window);
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


void Http2StreamHandler::processError(std::shared_ptr<Task> task, protocol::http2::error::ErrorCode code) {
  // ToDo Signal abortion of process to Htt2Processor and close connection
  char TAG[80];
  snprintf(TAG, 80, "oatpp::web::server::http2::Http2StreamHandler(%d)::processError", task->streamId);
  protocol::http2::Frame::Header header(8, 0, protocol::http2::Frame::Header::FrameType::GOAWAY, 0);
  OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId());
  v_uint32 lastStream = htonl(task->streamId);
  v_uint32 errorCode = htonl(code);
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
}

void Http2StreamHandler::process(std::shared_ptr<Task> task) {
  
}

void Http2StreamHandler::finalizeProcessAbortion(const std::shared_ptr<Task> &task) {
  task->setState(H2StreamState::ABORTED);
  task->clean();
}

void Http2StreamHandler::abort() {
  if (m_task->state.load() == PROCESSING || m_task->state.load() == RESPONDING) {
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

void Http2StreamHandler::resizeWindow(v_int32 change) {
  m_task->resizeWindow(change);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Worker

Http2StreamHandler::TaskWorker::TaskWorker(const std::shared_ptr<WorkerResources> &resources)
  : m_resources(resources) {
  m_waitList.setListener(this);
}

void Http2StreamHandler::TaskWorker::onNewItem(async::CoroutineWaitList &list)  {
  std::lock_guard<std::mutex> lock(m_mutex);
  if(m_done) {
    m_waitList.notifyAll();
  }
}

oatpp::async::CoroutineWaitList *Http2StreamHandler::TaskWorker::getWaitList() {
  return &m_waitList;
}

bool Http2StreamHandler::TaskWorker::isDone() {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_done;
}

void Http2StreamHandler::TaskWorker::run()  {
  char TAG[75];
  snprintf(TAG, 68, "oatpp::web::server::http2::Http2StreamHandler(%d)::TaskWorker::run", m_task->streamId);

  // ToDo: Move to prepare action
  protocol::http2::Headers requestHeaders;
  try {
    requestHeaders = m_task->hpack->inflate(m_task->header, m_task->header->availableToRead());
  } catch (std::runtime_error &e) {
    m_task->error = std::make_exception_ptr(protocol::http2::error::connection::CompressionError(e.what()));
    m_task->setState(ERROR);

    processError(m_task, protocol::http2::error::ErrorCode::COMPRESSION_ERROR);
    return;
  }

  if (m_task->state.load() == H2StreamState::RESET) {
    finalizeProcessAbortion(m_task);
    return;
  }

  // ToDo:
  //  1 - check that all header-keys are lower-case
  //  2 - check that there are no unknown pseudo-header fields (keys that start with ":")
  //  3 - check that no response-related pseudo-header fields contained in request
  //  4 - check that no pseudo-header fields are send after regular header fields

  protocol::http::RequestStartingLine startingLine;
  auto path = requestHeaders.get(protocol::http2::Header::PATH);
  if (path == nullptr) {
    processError(m_task, protocol::http2::error::ErrorCode::PROTOCOL_ERROR);
    return;
  }
  auto method = requestHeaders.get(protocol::http2::Header::METHOD);
  if (method == nullptr) {
    processError(m_task, protocol::http2::error::ErrorCode::PROTOCOL_ERROR);
    return;
  }
  auto scheme = requestHeaders.get(protocol::http2::Header::SCHEME);
  if (scheme == nullptr) {
    processError(m_task, protocol::http2::error::ErrorCode::PROTOCOL_ERROR);
    return;
  }
  auto contentLength = requestHeaders.get(protocol::http2::Header::CONTENT_LENGTH);
  if (contentLength) {
    bool success;
    v_uint32 contentLengthValue = oatpp::utils::conversion::strToUInt32(contentLength, success);
    if (contentLengthValue != m_task->data->availableToRead()) {
      processError(m_task, protocol::http2::error::ErrorCode::PROTOCOL_ERROR);
      return;
    }
  }
  startingLine.protocol = "HTTP/2.0";
  startingLine.path = path;
  startingLine.method = method;

  auto request = protocol::http::incoming::Request::createShared(nullptr, startingLine, requestHeaders, m_task->data, m_task->components->bodyDecoder);


  // ToDo: Worker Thread run() starts here
  ConnectionState connectionState;


  try {

    // ToDo: Interceptors

    auto route = m_task->components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);

    if(!route) {

      data::stream::BufferOutputStream ss;
      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
         << "', URL: '" << request->getStartingLine().path.toString() << "'";

      connectionState = ConnectionState::CLOSING;
      m_response = m_task->components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
    } else {
      request->setPathVariables(route.getMatchMap());
      m_response = route.getEndpoint()->handle(request);
    }

  } catch (oatpp::web::protocol::http::HttpError& error) {
    m_response = m_task->components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
    connectionState = ConnectionState::CLOSING;
  } catch (std::exception& error) {
    m_response = m_task->components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    connectionState = ConnectionState::CLOSING;
  } catch (...) {
    m_response = m_task->components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
    connectionState = ConnectionState::CLOSING;
  }

  if (m_task->state.load() == H2StreamState::RESET) {
    finalizeProcessAbortion(m_task);
    return;
  }
  
  // ToDo: Move to process response

  auto responseHeaders = response->getHeaders();
  responseHeaders.putIfNotExists(protocol::http2::Header::STATUS, utils::conversion::uint32ToStr(response->getStatus().code));

  auto headerStream = m_task->hpack->deflate(responseHeaders);

  if (!m_task->setStateWithExpection(H2StreamState::PROCESSING, H2StreamState::RESPONDING)) {
    finalizeProcessAbortion(m_task);
    return;
  }

  while(headerStream->availableToRead() > 0) {
    m_task->output->lock(m_task->weight);
    if (m_task->state.load() == H2StreamState::RESET) {
      m_task->output->unlock();
      finalizeProcessAbortion(m_task);
      return;
    }
    v_buff_size chunk = std::min(m_task->outSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE), (v_uint32)headerStream->availableToRead());
    v_uint8 flags = (headerStream->availableToRead() - chunk == 0) ? protocol::http2::Frame::Header::Flags::HEADER_END_HEADERS : 0;
    flags |= (response->getBody()->getKnownSize() == 0 || response->getBody()->getKnownData() == nullptr) ? protocol::http2::Frame::Header::Flags::HEADER_END_HEADERS : 0;

    protocol::http2::Frame::Header hdr(chunk, flags, protocol::http2::Frame::Header::HEADERS, m_task->streamId);
    OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
    hdr.writeToStream(m_task->output.get());
    headerStream->writeBufferToStream(m_task->output.get(), chunk);
//    v_uint8 transferBuffer[chunk];
//    data::stream::transfer(headerStream, m_task->output, chunk, transferBuffer, chunk);
//    m_task->output->writeSimple(body->getKnownData() + send, chunk);
    m_task->output->unlock();
  }

  if (response->getBody()->getKnownSize() > 0 && response->getBody()->getKnownData() != nullptr) {
    const v_buff_size toSend = response->getBody()->getKnownSize();
    auto body = response->getBody();
    for (v_buff_size send = 0; send < toSend;) {
      while (m_task->window <= 0) {
        std::this_thread::yield();
        if (m_task->state.load() == H2StreamState::RESET) {
          finalizeProcessAbortion(m_task);
          return;
        }
      }
      m_task->output->lock(m_task->weight);
      if (m_task->state.load() == H2StreamState::RESET) {
        m_task->output->unlock();
        finalizeProcessAbortion(m_task);
        return;
      }
      v_buff_size chunk = std::min(m_task->outSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE), (v_uint32)(toSend - send));
      if (chunk > m_task->window) {
        chunk = m_task->window;
      }
      protocol::http2::Frame::Header hdr(chunk, (toSend - send - chunk == 0) ? protocol::http2::Frame::Header::Flags::Data::DATA_END_STREAM : 0, protocol::http2::Frame::Header::DATA, m_task->streamId);
      OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      hdr.writeToStream(m_task->output.get());
      m_task->output->writeSimple(body->getKnownData() + send, chunk);
      m_task->output->unlock();
      send += chunk;
      m_task->window -= chunk;
    }
  }
  m_task->setState(H2StreamState::DONE);
  m_task->clean();
}

void Http2StreamHandler::TaskWorker::start() {
  // ToDo: Pool threads. Either with `Bench` or a similar approach as in `Executor`
  std::thread t([this]{
    run();
  });
  
  /* Get hardware concurrency -1 in order to have 1cpu free of workers. */
  v_int32 concurrency = oatpp::concurrency::getHardwareConcurrency();
  if (concurrency > 1) {
    concurrency -= 1;
  }

  /* Set thread affinity group CPUs [0..cpu_count - 1]. Leave one cpu free of workers */
  oatpp::concurrency::setThreadAffinityToCpuRange(t.native_handle(),
                                                  0,
                                                  concurrency - 1 /* -1 because 0-based index */);

  t.detach();
}

}}}}