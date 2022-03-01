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
#include "oatpp/web/server/http2/Http2SendFrameCoroutines.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/concurrency/Thread.hpp"


namespace oatpp { namespace web { namespace server { namespace http2 {

const char* Http2StreamHandler::TAG = "oatpp::web::server::http2::Http2StreamHandler";

const char *Http2StreamHandler::stateStringRepresentation(Http2StreamHandler::H2StreamState state) {
#define ENUM2STR(x) case x: return #x
  switch (state) {
    ENUM2STR(INIT);
    ENUM2STR(HEADERS);
    ENUM2STR(CONTINUATION);
    ENUM2STR(PAYLOAD);
    ENUM2STR(READY);
    ENUM2STR(PIPED);
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
    OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::Task::setStateWithExpection", "Stream %d, State: %s(%d) -> %s(%d)", streamId, stateStringRepresentation(expected), expected, stateStringRepresentation(next), next);
    return true;
  }
  return false;
}

void Http2StreamHandler::Task::setState(Http2StreamHandler::H2StreamState next) {
  OATPP_LOGD("oatpp::web::server::http2::Http2StreamHandler::Task::setState", "Stream %d, State: %s(%d) -> %s(%d)", streamId, stateStringRepresentation(state.load()), state.load(), stateStringRepresentation(next), next);
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
  : m_resources(resources)
  , m_done(false) {
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
  m_waitList.notifyAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Http2StreamHandler Coroutine

concurrency::ThreadPool<Http2StreamHandler::TaskWorker> Http2StreamHandler::s_pool;

Http2StreamHandler::Http2StreamHandler(const std::shared_ptr<Task> &task)
 : m_task(task) {}

async::Action Http2StreamHandler::act() {
  m_task->setState(H2StreamState::PROCESSING);

  protocol::http2::Headers requestHeaders;
  try {
    requestHeaders = m_task->hpack->inflate(m_task->header, m_task->header->availableToRead());
  } catch (std::runtime_error &e) {
    return error<protocol::http2::error::connection::CompressionError>(e.what());
  }

  // ToDo:
  //  1 - check that all header-keys are lower-case
  //  2 - check that there are no unknown pseudo-header fields (keys that start with ":")
  //  3 - check that no response-related pseudo-header fields contained in request
  //  4 - check that no pseudo-header fields are send after regular header fields

  protocol::http::RequestStartingLine startingLine;
  auto path = requestHeaders.get(protocol::http2::Header::PATH);
  if (path == nullptr) {
    return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Headers missing ':path'.");
  }
  if (path->empty()) {
    return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Empty ':path' header.");
  }
  auto method = requestHeaders.get(protocol::http2::Header::METHOD);
  if (method == nullptr) {
    return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Headers missing ':method'.");
  }
  if (method->empty()) {
    return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Empty ':method' header.");
  }
  auto scheme = requestHeaders.get(protocol::http2::Header::SCHEME);
  if (scheme == nullptr) {
    return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Headers missing ':scheme'.");
  }
  if (scheme->empty()) {
    return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Empty ':scheme' header.");
  }
  auto contentLength = requestHeaders.get(protocol::http2::Header::CONTENT_LENGTH);
  if (contentLength) {
    bool success;
    v_uint32 contentLengthValue = oatpp::utils::conversion::strToUInt32(contentLength, success);
    if (contentLengthValue != m_task->data->availableToRead()) {
      return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::prepareAndStartWorker] Error: Data length and content-length do not match.");
    }
  }
  startingLine.protocol = "HTTP/2.0";
  startingLine.path = path;
  startingLine.method = method;

  m_resources = std::make_shared<TaskWorker::Resources>(protocol::http::incoming::Request::createShared(nullptr, startingLine, requestHeaders, m_task->data, m_task->components->bodyDecoder), m_task->components);
  m_worker = std::make_shared<TaskWorker>(m_resources.get());


  s_pool.enqueue(m_worker);
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
    data::buffer::InlineWriteData m_writer;
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
         return m_task->output->lock(m_task->weight, yieldTo(&WriteHeadersCoroutine::sendFrameHeaderPrepare));
       }
       return finish();
    }

    Action sendFrameHeaderPrepare() {
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
      m_writer.set(m_frameHeaderBuffer, protocol::http2::Frame::Header::HeaderSize);
      return yieldTo(&WriteHeadersCoroutine::sendFrameHeader);
    }

    Action sendFrameHeader() {
      return m_task->output->writeExactSizeDataAsyncInline(m_writer, yieldTo(&WriteHeadersCoroutine::sendFrameBody));
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
    data::buffer::InlineWriteData m_headerWriter;
    data::buffer::InlineWriteData m_bodyWriter;
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
      return m_task->output->lock(m_task->weight, yieldTo(&WriteDataCoroutine::sendFrameHeaderPrepare));
    }

    Action waitForWindow() {
      if (m_task->window < 1) {
        return repeat();
      }
      return yieldTo(&WriteDataCoroutine::act);
    };

    Action sendFrameHeaderPrepare() {
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
      m_headerWriter.set(m_frameHeaderBuffer, protocol::http2::Frame::Header::HeaderSize);
      m_bodyWriter.set(m_body->getKnownData() + m_pos, m_chunk);
      return yieldTo(&WriteDataCoroutine::sendFrameHeader);
    }

    Action sendFrameHeader() {
      return m_task->output->writeExactSizeDataAsyncInline(m_headerWriter, yieldTo(&WriteDataCoroutine::sendFrameBody));
    };

    Action sendFrameBody() {
      return m_task->output->writeExactSizeDataAsyncInline(m_bodyWriter, yieldTo(&WriteDataCoroutine::finalizeFrame));
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

async::Action Http2StreamHandler::handleError(async::Error *error) {
  OATPP_LOGE(TAG, error->what());
  if (error->is<protocol::http2::error::connection::ProtocolError>()) {
    return framecoroutines::SendGoaway::start(m_task->output, m_task->streamId, protocol::http2::error::PROTOCOL_ERROR).next(finish());
  } else if (error->is<protocol::http2::error::connection::CompressionError>()) {
    return framecoroutines::SendGoaway::start(m_task->output, m_task->streamId, protocol::http2::error::COMPRESSION_ERROR).next(finish());
  } else if (error->is<protocol::http2::error::connection::FrameSizeError>()) {
    return framecoroutines::SendGoaway::start(m_task->output, m_task->streamId, protocol::http2::error::FRAME_SIZE_ERROR).next(finish());
  }
  return AbstractCoroutine::handleError(error);
}

//void Http2StreamHandler::TaskWorker::start() {
//  // ToDo: Pool threads. Either with `Bench` or a similar approach as in `Executor`
//  std::thread t([this]{
//    run();
//  });
//
//  /* Get hardware concurrency -1 in order to have 1cpu free of workers. */
//  v_int32 concurrency = oatpp::concurrency::getHardwareConcurrency();
//  if (concurrency > 1) {
//    concurrency -= 1;
//  }
//
//  /* Set thread affinity group CPUs [0..cpu_count - 1]. Leave one cpu free of workers */
//  oatpp::concurrency::setThreadAffinityToCpuRange(t.native_handle(),
//                                                  0,
//                                                  concurrency - 1 /* -1 because 0-based index */);
//
//  t.detach();
//}

}}}}