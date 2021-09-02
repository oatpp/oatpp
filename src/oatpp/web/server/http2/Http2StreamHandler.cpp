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

const char* Http2StreamHandler::TAG = "oatpp::web::server::http2::Http2StreamHandler";

Http2StreamHandler::H2StreamState Http2StreamHandler::handleData(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {
  task->setState(H2StreamState::PAYLOAD);
  v_uint8 pad = 0;
  if (flags & H2StreamDataFlags::DATA_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength--;
    if (pad > streamPayloadLength) {
      task->setState(ABORTED);
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Padding length longer than remaining data.");
    }
  }

  v_io_size read = streamPayloadLength - pad;

  if (streamPayloadLength + task->data->availableToRead() > task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    task->setState(ABORTED);
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleData] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE.");
  }

  // ToDo: Async
  while (read > 0) {
    async::Action action;
    v_io_size res = task->data->readStreamToBuffer(stream.get(), read, action);
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
    task->setState(H2StreamState::READY);
  }

  return task->state;
}

Http2StreamHandler::H2StreamState Http2StreamHandler::handleHeaders(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {

  if (task->state == H2StreamState::PAYLOAD && (flags & H2StreamHeaderFlags::HEADER_END_STREAM) == 0) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Received HEADERS frame without the HEADER_END_STREAM flag set after DATA frames");
  }

  task->setState(H2StreamState::HEADERS);
  task->headerFlags |= flags;
  v_uint8 pad = 0;

  if (task->headerFlags & H2StreamHeaderFlags::HEADER_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength -= 1;
    if (pad > streamPayloadLength) {
      task->setState(ABORTED);
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Padding length longer than remaining header-data.");
    }
  }

  if (task->headerFlags & H2StreamHeaderFlags::HEADER_PRIORITY) {
    stream->readExactSizeDataSimple(&task->dependency, 4);
    task->dependency = ntohl(task->dependency);
    if (task->dependency == task->streamId) {
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Received header for stream that depends on itself.");
    }
    stream->readExactSizeDataSimple(&task->weight, 1);
    streamPayloadLength -= 5;
  }

  v_io_size read = streamPayloadLength - pad;
  if (streamPayloadLength + task->header->availableToRead() > task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    task->setState(ABORTED);
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE.");
  }

  // ToDo: Async
  while (read > 0) {
    async::Action action;
    v_io_size res = task->header->readStreamToBuffer(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      OATPP_LOGD(TAG, "Could not read header: Broken Pipe");
      throw protocol::http2::error::connection::InternalError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Could not read header (Broken Pipe)");
    } else {
      std::this_thread::yield();
    }
  }

  if ((task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) { // end stream, go to processing
    OATPP_LOGD(TAG, "Stream and headers finished, start processing.");
    task->setState(H2StreamState::READY);
  } else if (task->headerFlags & H2StreamHeaderFlags::HEADER_END_STREAM) { // continuation
    OATPP_LOGD(TAG, "Stream finished, headers not, awaiting continuation.");
    task->setState(H2StreamState::CONTINUATION);
  } else if (task->headerFlags & H2StreamHeaderFlags::HEADER_END_HEADERS) { // payload
    task->setState(H2StreamState::PAYLOAD);
    OATPP_LOGD(TAG, "Headers finished, stream not, awaiting data.");
  } else {
    OATPP_LOGE(TAG, "Something is wrong");
  }

  if (pad > 0) {
    oatpp::String paddata((v_buff_size)pad);
    stream->readExactSizeDataSimple((void*)paddata->data(), paddata->size());
  }

  return task->state;
}

Http2StreamHandler::H2StreamState Http2StreamHandler::handlePriority(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {
  if (streamPayloadLength != 5) {
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handlePriority] Error: Frame size other than 5.");

  }
  stream->readExactSizeDataSimple(&task->dependency, 4);
  task->dependency = ntohl(task->dependency);
  if (task->dependency == task->streamId) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::handlePriority] Error: Received PRIORITY frame with dependency on itself.");
  }
  stream->readExactSizeDataSimple(&task->weight, 1);
  streamPayloadLength -= 5;
  return task->state;
}

Http2StreamHandler::H2StreamState Http2StreamHandler::handleResetStream(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {
  if (streamPayloadLength != 4) {
    throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2StreamHandler::handleResetStream] Error: Frame size other than 4.");
  }
  v_uint32 code;
  stream->readExactSizeDataSimple(&code, 4);
  OATPP_LOGD(TAG, "Resetting stream, code %u", ntohl(code))
  if (task->state == PROCESSING) {
    task->setState(RESET);
    return task->state;
  }
  task->setState(ABORTED);
  return task->state;
}


Http2StreamHandler::H2StreamState Http2StreamHandler::handlePushPromise(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {
  return task->state;
}

Http2StreamHandler::H2StreamState Http2StreamHandler::handleWindowUpdate(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {
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
  v_uint32 unsignedWindow = task->window;
  if (unsignedWindow + increment > Http2Settings::getSettingMax(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)) {
    throw protocol::http2::error::stream::FlowControlError("[oatpp::web::server::http2::Http2StreamHandler::handleWindowUpdate] Error: Increment above 2^31-1");
  }
  OATPP_LOGD(TAG, "Incrementing window by %u", increment);
  task->window += increment;

  return task->state;
}

Http2StreamHandler::H2StreamState Http2StreamHandler::handleContinuation(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength) {

  task->headerFlags |= flags;

  // ToDo: Async
  v_io_size read = streamPayloadLength;
  while (read > 0) {
    async::Action action;
    v_io_size res = task->header->readStreamToBuffer(stream.get(), read, action);
    if (res > 0) {
      read -= res;
    } else if (res == IOError::BROKEN_PIPE) {
      throw protocol::http2::error::connection::InternalError("[oatpp::web::server::http2::Http2StreamHandler::handleHeaders] Error: Could not read header continuation (Broken Pipe)");
    } else {
      std::this_thread::yield();
    }
  }

  if ((task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == H2StreamHeaderFlags::HEADER_END_HEADERS) {
    task->setState(H2StreamState::PAYLOAD);
  } else if ((task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) {
    task->setState(H2StreamState::READY);
  }

  return task->state;
}

const char *Http2StreamHandler::stateStringRepresentation(Http2StreamHandler::H2StreamState state) {
#define ENUM2STR(x) case x: return #x
  switch (state) {
    ENUM2STR(INIT);
    ENUM2STR(HEADERS);
    ENUM2STR(CONTINUATION);
    ENUM2STR(PAYLOAD);
    ENUM2STR(READY);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Http2StreamHandler::Task

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
  OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::Task::resizeWindow", "Resized window by %d to %d", change, window.load());
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


//void Http2StreamHandler::processError(std::shared_ptr<Task> task, protocol::http2::error::ErrorCode code) {
//  // ToDo Signal abortion of process to Htt2Processor and close connection
//  char TAG[80];
//  snprintf(TAG, 80, "oatpp::web::server::http2::Http2StreamHandler(%d)::processError", task->streamId);
//  protocol::http2::Frame::Header header(8, 0, protocol::http2::Frame::Header::FrameType::GOAWAY, 0);
//  OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId());
//  v_uint32 lastStream = htonl(task->streamId);
//  v_uint32 errorCode = htonl(code);
//  task->output->lock(PriorityStreamScheduler::PRIORITY_MAX);
//  if (task->state.load() == H2StreamState::RESET) {
//    task->output->unlock();
//    finalizeProcessAbortion(task);
//    return;
//  }
//  header.writeToStream(task->output.get());
//  task->output->writeExactSizeDataSimple(&lastStream, 4);
//  task->output->writeExactSizeDataSimple(&errorCode, 4);
//  task->output->unlock();
//
//  finalizeProcessAbortion(task);
//  task->setState(ERROR);
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TaskWorker

Http2StreamHandler::TaskWorker::TaskWorker(TaskWorker::Resources *resources)
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
  try {

    // ToDo: Interceptors

    auto route = m_resources->components->router->getRoute(m_resources->request->getStartingLine().method, m_resources->request->getStartingLine().path);

    if(!route) {

      data::stream::BufferOutputStream ss;
      ss << "No mapping for HTTP-method: '" << m_resources->request->getStartingLine().method.toString()
         << "', URL: '" << m_resources->request->getStartingLine().path.toString() << "'";

      m_resources->response = m_resources->components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
    } else {
      m_resources->request->setPathVariables(route.getMatchMap());
      m_resources->response = route.getEndpoint()->handle(m_resources->request);
    }

  } catch (oatpp::web::protocol::http::HttpError& error) {
    m_resources->response = m_resources->components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
  } catch (std::exception& error) {
    m_resources->response = m_resources->components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
  } catch (...) {
    m_resources->response = m_resources->components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
  }

  m_mutex.lock();
  m_done = true;
  m_mutex.unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Http2StreamHandler Coroutine

Http2StreamHandler::Http2StreamHandler(const std::shared_ptr<Task> &task)
 : m_task(task) {}

async::Action Http2StreamHandler::act() {
  m_task->setState(H2StreamState::PROCESSING);

  protocol::http2::Headers requestHeaders;
  try {
    requestHeaders = m_task->hpack->inflate(m_task->header, m_task->header->availableToRead());
  } catch (std::runtime_error &e) {
    throw protocol::http2::error::connection::CompressionError(e.what());
  }

  // ToDo:
  //  1 - check that all header-keys are lower-case
  //  2 - check that there are no unknown pseudo-header fields (keys that start with ":")
  //  3 - check that no response-related pseudo-header fields contained in request
  //  4 - check that no pseudo-header fields are send after regular header fields

  protocol::http::RequestStartingLine startingLine;
  auto path = requestHeaders.get(protocol::http2::Header::PATH);
  if (path == nullptr) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Headers missing ':path'.");
  }
  auto method = requestHeaders.get(protocol::http2::Header::METHOD);
  if (method == nullptr) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Headers missing ':method'.");
  }
  auto scheme = requestHeaders.get(protocol::http2::Header::SCHEME);
  if (scheme == nullptr) {
    throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Headers missing ':scheme'.");
  }
  auto contentLength = requestHeaders.get(protocol::http2::Header::CONTENT_LENGTH);
  if (contentLength) {
    bool success;
    v_uint32 contentLengthValue = oatpp::utils::conversion::strToUInt32(contentLength, success);
    if (contentLengthValue != m_task->data->availableToRead()) {
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Data length and content-length do not match.");
    }
  }
  startingLine.protocol = "HTTP/2.0";
  startingLine.path = path;
  startingLine.method = method;

  m_resources = std::make_shared<TaskWorker::Resources>(protocol::http::incoming::Request::createShared(nullptr, startingLine, requestHeaders, m_task->data, m_task->components->bodyDecoder), m_task->components);
  m_worker = std::make_shared<TaskWorker>(m_resources.get());

  m_worker->start();
  return yieldTo(&Http2StreamHandler::startWorker);
}

async::Action Http2StreamHandler::startWorker() {
  if (!m_worker->isDone()) {
    return async::Action::createWaitListAction(m_worker->getWaitList());
  }
  return yieldTo(&Http2StreamHandler::processWorkerResult);
}

async::Action Http2StreamHandler::processWorkerResult() {

  auto responseHeaders = m_resources->response->getHeaders();
  responseHeaders.putIfNotExists(protocol::http2::Header::STATUS, utils::conversion::uint32ToStr(m_resources->response->getStatus().code));

  auto headerStream = m_task->hpack->deflate(responseHeaders);

  if (!m_task->setStateWithExpection(H2StreamState::PROCESSING, H2StreamState::RESPONDING)) {
    return finish();
  }

  class WriteHeadersCoroutine : public async::Coroutine<WriteHeadersCoroutine> {
   private:
    std::shared_ptr<data::stream::BufferedInputStream> m_headerStream;
    std::shared_ptr<Task> m_task;
    v_buff_size m_chunk;
    v_uint8 m_frameHeaderBuffer[protocol::http2::Frame::Header::HeaderSize];
    protocol::http2::Frame::Header::FrameType m_frameType;
    bool m_endsStream;

   public:
    WriteHeadersCoroutine(const std::shared_ptr<Task> &task, const std::shared_ptr<data::stream::BufferedInputStream> &stream, bool endsStream)
     : m_task(task)
     , m_headerStream(stream)
     , m_frameType(protocol::http2::Frame::Header::HEADERS)
     , m_endsStream(endsStream) {}

     Action act() override {
       if(m_headerStream->availableToRead() > 0) {
         m_task->output->lock(m_task->weight, yieldTo(&WriteHeadersCoroutine::sendFrameHeader));
       }
       return finish();
    }

    Action sendFrameHeader() {
      if (m_task->state.load() == H2StreamState::RESET) {
        m_task->output->unlock();
        return finish();
      }
      m_chunk = std::min(m_task->outSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE), (v_uint32)m_headerStream->availableToRead());
      v_uint8 flags = (m_headerStream->availableToRead() - m_chunk == 0) ? protocol::http2::Frame::Header::Flags::HEADER_END_HEADERS : 0;
      flags |= m_endsStream ? protocol::http2::Frame::Header::Flags::HEADER_END_STREAM : 0;

      protocol::http2::Frame::Header hdr(m_chunk, flags, m_frameType, m_task->streamId);
      OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::processWorkerResult::WriteHeadersCoroutine", "Sending %s (length:%lu, flags:0x%02x, StreamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(hdr.getType()), hdr.getLength(), hdr.getFlags(), hdr.getStreamId());
      hdr.writeToBuffer(m_frameHeaderBuffer);
      data::buffer::InlineWriteData iwd(m_frameHeaderBuffer, protocol::http2::Frame::Header::HeaderSize);
      return m_task->output->writeExactSizeDataAsyncInline(iwd, yieldTo(&WriteHeadersCoroutine::sendFrameBody));
    }

    Action sendFrameBody() {
      return m_headerStream->writeBufferToStreamAsync(m_task->output, m_chunk).next(yieldTo(&WriteHeadersCoroutine::finalizeFrame));
    }

    Action finalizeFrame() {
      m_task->output->unlock();
      if (m_task->state.load() == H2StreamState::RESET) {
        return finish();
      }
      if (m_headerStream->availableToRead() > 0) {
        m_endsStream = false;
        m_frameType = protocol::http2::Frame::Header::CONTINUATION;
        return yieldTo(&WriteHeadersCoroutine::act);
      }
      return finish();
    }
  };

  if (m_resources->response->getBody()->getKnownSize() > 0 && m_resources->response->getBody()->getKnownData() != nullptr) {
    return WriteHeadersCoroutine::start(m_task, headerStream, false).next(yieldTo(&Http2StreamHandler::writeData));
  }
  return WriteHeadersCoroutine::start(m_task, headerStream, false).next(yieldTo(&Http2StreamHandler::finalize));
}

async::Action Http2StreamHandler::writeData() {

  class WriteDataCoroutine : public async::Coroutine<WriteDataCoroutine> {
   private:
    std::shared_ptr<Task> m_task;
    std::shared_ptr<protocol::http::outgoing::Body> m_body;
    const v_buff_size m_toSend;
    v_buff_size m_chunk;
    v_buff_size m_pos;
    v_uint8 m_frameHeaderBuffer[protocol::http2::Frame::Header::HeaderSize];

   public:
    WriteDataCoroutine(const std::shared_ptr<Task> &task, const std::shared_ptr<protocol::http::outgoing::Body> &body)
     : m_task(task)
     , m_body(body)
     , m_toSend(body->getKnownSize())
     , m_pos(0) {}

    Action act() override {
      if (m_task->window < 1) {
        return yieldTo(&WriteDataCoroutine::waitForWindow);
      }
      return m_task->output->lock(m_task->weight, yieldTo(&WriteDataCoroutine::sendFrameHeader));
    }

    Action waitForWindow() {
      if (m_task->window < 1) {
        return repeat();
      }
      return yieldTo(&WriteDataCoroutine::act);
    };

    Action sendFrameHeader() {
      if (m_task->state.load() == H2StreamState::RESET) {
        m_task->output->unlock();
        return finish();
      }
      m_chunk = std::min(m_task->outSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE), (v_uint32)(m_toSend - m_pos));
      if (m_chunk > m_task->window) {
        m_chunk = m_task->window;
      }
      protocol::http2::Frame::Header hdr(m_chunk, (m_toSend - m_pos - m_chunk == 0) ? protocol::http2::Frame::Header::Flags::Data::DATA_END_STREAM : 0, protocol::http2::Frame::Header::DATA, m_task->streamId);
      hdr.writeToBuffer(m_frameHeaderBuffer);
      data::buffer::InlineWriteData iwd(m_frameHeaderBuffer, protocol::http2::Frame::Header::HeaderSize);
      return m_task->output->writeExactSizeDataAsyncInline(iwd, yieldTo(&WriteDataCoroutine::sendFrameBody));
    };

    Action sendFrameBody() {
      data::buffer::InlineWriteData iwd(m_body->getKnownData() + m_pos, m_chunk);
      return m_task->output->writeExactSizeDataAsyncInline(iwd, yieldTo(&WriteDataCoroutine::finalizeFrame));
    };

    Action finalizeFrame() {
      m_task->output->unlock();
      m_pos += m_chunk;
      m_task->window -= m_chunk;
      if (m_task->state.load() == H2StreamState::RESET) {
        return finish();
      }
      if (m_body->getKnownSize() - m_pos > 0) {
        return yieldTo(&WriteDataCoroutine::act);
      }
      return finish();
    }
  };

  return WriteDataCoroutine::start(m_task, m_resources->response->getBody()).next(yieldTo(&Http2StreamHandler::finalize));
}

async::Action Http2StreamHandler::finalize() {
  return finish();
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