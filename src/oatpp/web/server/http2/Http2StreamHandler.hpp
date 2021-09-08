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

#ifndef oatpp_web_server_http2_Http2StreamHandler_hpp
#define oatpp_web_server_http2_Http2StreamHandler_hpp

#include <utility>

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/data/stream/FIFOStream.hpp"

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/web/protocol/http2/hpack/Hpack.hpp"

#include "oatpp/web/protocol/http/utils/CommunicationUtils.hpp"
#include "oatpp/web/protocol/http2/Http2.hpp"

#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"
#include "oatpp/web/server/http2/PriorityStreamScheduler.hpp"
#include "oatpp/web/server/http2/Http2Settings.hpp"

#include "oatpp/core/concurrency/ThreadPool.hpp"

#include <arpa/inet.h>


namespace oatpp { namespace web { namespace server { namespace http2 {

class Http2StreamHandler : public async::Coroutine<Http2StreamHandler> {
 private:
  static const char* TAG;
 public:
  enum H2StreamState {
    INIT = 0,
    HEADERS = 1,
    CONTINUATION = 2,
    PAYLOAD = 3,
    READY = 4,
    PIPED = 5,
    PROCESSING = 6,
    RESPONDING = 7,
    DONE = 8,
    RESET = 9,
    ABORTED = 10,
    ERROR = 11
  };

  typedef protocol::http2::Frame::Header::Flags::Header H2StreamHeaderFlags;
  typedef protocol::http2::Frame::Header::Flags::Data H2StreamDataFlags;

  class Task {
   public:
    std::atomic<H2StreamState> state;
    std::shared_ptr<protocol::http2::hpack::Hpack> hpack;
    std::shared_ptr<http2::processing::Components> components;
    std::shared_ptr<http2::PriorityStreamSchedulerAsync> output;
    std::shared_ptr<http2::Http2Settings> inSettings;
    std::shared_ptr<http2::Http2Settings> outSettings;
    std::shared_ptr<data::stream::FIFOInputStream> data;
    std::shared_ptr<data::stream::FIFOInputStream> header;
    v_uint32 streamId;
    v_uint32 dependency;
    v_uint8 weight;
    v_uint8 headerFlags;
    std::atomic_int32_t window;

    Task(v_uint32 id, const std::shared_ptr<http2::PriorityStreamSchedulerAsync> &outputStream, const std::shared_ptr<protocol::http2::hpack::Hpack> &hpack, const std::shared_ptr<http2::processing::Components> &components, const std::shared_ptr<http2::Http2Settings> &inSettings, const std::shared_ptr<http2::Http2Settings> &outSettings)
        : state(INIT)
        , streamId(id)
        , output(outputStream)
        , hpack(hpack)
        , components(components)
        , inSettings(inSettings)
        , outSettings(outSettings)
        , dependency(0)
        , weight(0)
        , headerFlags(0)
        , window(static_cast<v_int32>(outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)))
        , header(data::stream::FIFOInputStream::createShared(inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)))
        , data(data::stream::FIFOInputStream::createShared(inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE))){}

    bool setStateWithExpection(H2StreamState expected, H2StreamState next);
    void setState(H2StreamState next);
    void clean();
    void resizeWindow(v_int32 change);
  };

 private:
  class TaskWorker : public oatpp::async::CoroutineWaitList::Listener {
    friend concurrency::ThreadPool<TaskWorker>;
   public:
    class Resources {
     public:
      std::shared_ptr<protocol::http::incoming::Request> request;
      std::shared_ptr<http2::processing::Components> components;
      std::shared_ptr<protocol::http::outgoing::Response> response;

     public:
      Resources() = delete;
      Resources(const std::shared_ptr<protocol::http::incoming::Request> &req, const std::shared_ptr<http2::processing::Components> &comp)
          : request(req)
          , components(comp) {}
    };

    /* oatpp::async::CoroutineWaitList::Listener::onNewItem */
    void onNewItem(oatpp::async::CoroutineWaitList& list) override;

   private:
    Resources *m_resources;
    oatpp::async::CoroutineWaitList m_waitList;
    bool m_done = false;
    std::mutex m_mutex;

   public:
    TaskWorker(Resources *resources);

    oatpp::async::CoroutineWaitList* getWaitList();
    bool isDone();

   private:
    void run();
  };

  Http2StreamHandler() = delete;
  static concurrency::ThreadPool<TaskWorker> s_pool;
  std::shared_ptr<Task> m_task;
  std::shared_ptr<TaskWorker> m_worker;
  std::shared_ptr<TaskWorker::Resources> m_resources;

 public:
  Http2StreamHandler(const std::shared_ptr<Task> &task);

  async::Action act() override;
  async::Action startWorker();
  async::Action processWorkerResult();

  async::Action writeHeaders();
  async::Action writeData();

  async::Action finalize();

  Action handleError(Error *error) override;

  ///////

  template<class H>
  class AbstractHandlerCoroutine : public async::CoroutineWithResult<H, const std::shared_ptr<Task> &> {
   protected:
    const std::shared_ptr<Task> m_task;
    const std::shared_ptr<data::stream::InputStream> m_stream;
    v_io_size m_streamPayloadLength;
    v_uint8 m_flags;
   public:
    AbstractHandlerCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength)
    : m_task(task)
    , m_flags(flags)
    , m_stream(stream)
    , m_streamPayloadLength(streamPayloadLength) {}
  };

  class HandleDataCoroutine : public AbstractHandlerCoroutine<HandleDataCoroutine> {
   private:
    data::buffer::InlineReadData m_reader;
    std::unique_ptr<v_uint8[]> m_padData;
    v_uint8 m_pad;

   public:
    HandleDataCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength)
    : AbstractHandlerCoroutine<HandleDataCoroutine>(task, flags, stream, streamPayloadLength)
    , m_pad(0) {
      task->setState(H2StreamState::PAYLOAD);
    }

    async::Action act() override {
      if (m_flags & H2StreamDataFlags::DATA_PADDED) {
        -- m_streamPayloadLength;
        m_reader.set(&m_pad, 1);
        return yieldTo(&HandleDataCoroutine::readPadSize);
      }
      return yieldTo(&HandleDataCoroutine::readData);
    }

    async::Action readPadSize() {
      return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandleDataCoroutine::evalPad));
    }

    async::Action evalPad() {
      if (m_pad > m_streamPayloadLength) {
        return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::HandleDataCoroutine::evalPad()] Error: Padding length longer than remaining data.");
      }
      return yieldTo(&HandleDataCoroutine::readData);
    }

    async::Action readData() {
      if (m_streamPayloadLength - m_pad > 0) {
        async::Action action;
        auto res = m_task->data->readStreamToBuffer(m_stream.get(), m_streamPayloadLength - m_pad, action);
        if (!action.isNone()) {
          return action;
        }
        m_streamPayloadLength -= res;
        if (m_streamPayloadLength - m_pad > 0) {
          return repeat();
        }
      }
      if (m_pad > 0) {
        m_padData = std::unique_ptr<v_uint8[]>(new v_uint8[m_pad]); // IO Buffer Pool?
        m_reader.set(m_padData.get(), m_pad);
        return yieldTo(&HandleDataCoroutine::readPad);
      }
      return finalize(); // do not yield to, take a shortcut
    }

    async::Action readPad() {
      if (m_reader.bytesLeft > 0) {
        return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandleDataCoroutine::finalize));
      }
      return finalize(); // do not yield to, take a shortcut
    }

    async::Action finalize() {
      if (m_flags & H2StreamDataFlags::DATA_END_STREAM) {
        if (m_task->state != H2StreamState::PIPED) {
          m_task->setState(H2StreamState::READY);
        }
      }
      return _return(m_task);
    }
  };

  class HandleHeadersCoroutine : public AbstractHandlerCoroutine<HandleHeadersCoroutine> {
   private:
    data::buffer::InlineReadData m_reader;
    std::unique_ptr<v_uint8[]> m_tmpData; // IO Buffer Pool?
    v_uint8 m_pad;

   public:
    HandleHeadersCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength)
    : AbstractHandlerCoroutine<HandleHeadersCoroutine>(task, flags, stream, streamPayloadLength)
    , m_pad(0) {
      task->setState(H2StreamState::HEADERS);
      task->headerFlags |= flags;
    }

    async::Action act() override {
      v_uint8 predata = 0;
      if (m_flags & H2StreamHeaderFlags::HEADER_PADDED) {
        -- m_streamPayloadLength;
        ++ predata;
      }
      if (m_flags & H2StreamHeaderFlags::HEADER_PRIORITY) {
        m_streamPayloadLength -= 5;
        predata += 5;
      }

      if (m_streamPayloadLength + m_task->header->availableToRead() > m_task->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
        m_task->setState(ERROR);
        return error<protocol::http2::error::connection::FrameSizeError>("[oatpp::web::server::http2::Http2StreamHandler::HandleHeadersCoroutine::act()] Error: Frame exceeds SETTINGS_MAX_FRAME_SIZE.");
      }

      if (predata) {
        m_tmpData = std::unique_ptr<v_uint8[]>(new v_uint8[predata]);
        m_reader.set(m_tmpData.get(), predata);
        return yieldTo(&HandleHeadersCoroutine::readPreData);
      }

      return yieldTo(&HandleHeadersCoroutine::readData);
    }

    async::Action readPreData() {
      return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandleHeadersCoroutine::evalPreData));
    }

    async::Action evalPreData() {
      p_uint8 preDataPtr = m_tmpData.get();
      if (m_flags & H2StreamHeaderFlags::HEADER_PADDED) {
        m_pad = *preDataPtr++;
        if (m_pad > m_streamPayloadLength) {
          return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::HandleHeadersCoroutine::evalPreData()] Error: Padding length longer than remaining data.");
        }
      }
      if (m_flags & H2StreamHeaderFlags::HEADER_PRIORITY) {
        m_task->dependency = ((*(preDataPtr)) << 24) | ((*(preDataPtr + 1)) << 16) | ((*(preDataPtr + 2)) << 8) | (*(preDataPtr + 3));
        m_task->weight = *(preDataPtr + 4);
        if (m_task->dependency == m_task->streamId) {
          return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::HandleHeadersCoroutine::evalPreData()] Error: Received header for stream that depends on itself.");
        }
      }
      m_tmpData.release();
      return yieldTo(&HandleHeadersCoroutine::readData);
    }

    async::Action readData() {
      if (m_streamPayloadLength - m_pad > 0) {
        async::Action action;
        auto res = m_task->header->readStreamToBuffer(m_stream.get(), m_streamPayloadLength - m_pad, action);
        if (!action.isNone()) {
          return action;
        }
        m_streamPayloadLength -= res;
        if (m_streamPayloadLength - m_pad > 0) {
          return repeat();
        }
      }
      if (m_pad > 0) {
        m_tmpData = std::unique_ptr<v_uint8[]>(new v_uint8[m_pad]); // IO Buffer Pool?
        m_reader.set(m_tmpData.get(), m_pad);
        return yieldTo(&HandleHeadersCoroutine::readPad);
      }
      return finalize(); // do not yield to, take a shortcut
    }

    async::Action readPad() {
      if (m_reader.bytesLeft > 0) {
        return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandleHeadersCoroutine::finalize));
      }
      return finalize(); // do not yield to, take a shortcut
    }

    async::Action finalize() {
      if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) { // end stream, go to processing
        OATPP_LOGD(TAG, "Stream and headers finished, start processing.");
        if (m_task->state != H2StreamState::PIPED) {
          m_task->setState(H2StreamState::READY);
        }
      } else if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_END_STREAM) { // continuation
        OATPP_LOGD(TAG, "Stream finished, headers not, awaiting continuation.");
        m_task->setState(H2StreamState::CONTINUATION);
      } else if (m_task->headerFlags & H2StreamHeaderFlags::HEADER_END_HEADERS) { // payload
        m_task->setState(H2StreamState::PAYLOAD);
        OATPP_LOGD(TAG, "Headers finished, stream not, awaiting data.");
      }
      return _return(m_task);
    }
  };

  class HandlePriorityCoroutine : public AbstractHandlerCoroutine<HandlePriorityCoroutine> {
   private:
    data::buffer::InlineReadData m_reader;
    v_uint8 m_tmp[5];

   public:
    HandlePriorityCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size stream_payload_length)
    : AbstractHandlerCoroutine(task, flags, stream, stream_payload_length)
    , m_reader(m_tmp, 5) {}

    async::Action act() override {
      return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandlePriorityCoroutine::finalize));
    }

    async::Action finalize() {
      m_task->dependency = (m_tmp[0] << 24) | (m_tmp[1] << 16) | (m_tmp[2]<< 8) | m_tmp[3];
      m_task->weight = m_tmp[4];
      if (m_task->dependency == m_task->streamId) {
        return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::HandlePriorityCoroutine::finalize()] Error: Received header for stream that depends on itself.");
      }
      return _return(m_task);
    }
  };

  class HandleResetStreamCoroutine : public AbstractHandlerCoroutine<HandleResetStreamCoroutine> {
   private:
    data::buffer::InlineReadData m_reader;
    v_uint32 code;

   public:
    HandleResetStreamCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size stream_payload_length)
    : AbstractHandlerCoroutine(task, flags, stream, stream_payload_length)
    , m_reader(&code, 4) {}

    async::Action act() override {
      return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandleResetStreamCoroutine::finalize));
    }

    async::Action finalize() {
      OATPP_LOGD(TAG, "Resetting stream, code %u", ntohl(code))
      if (m_task->state == PROCESSING) {
        m_task->setState(RESET);
        return _return(m_task);
      }
      m_task->setState(ABORTED);
      return _return(m_task);
    }
  };

  class HandlePushPromiseCoroutine : public AbstractHandlerCoroutine<HandlePushPromiseCoroutine> {
   public:
    HandlePushPromiseCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size stream_payload_length)
    : AbstractHandlerCoroutine(task, flags, stream, stream_payload_length) {}

    async::Action act() override {
      return error<protocol::http2::error::connection::InternalError>("[oatpp::web::server::http2::Http2StreamHandler::HandlePriorityCoroutine::act()] Error: Server-side PushPromise not implemented.");
    }
  };

  class HandleWindowUpdateCoroutine : public AbstractHandlerCoroutine<HandleWindowUpdateCoroutine> {
   private:
    data::buffer::InlineReadData m_reader;
    v_uint32 m_increment;

   public:
    HandleWindowUpdateCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size stream_payload_length)
    : AbstractHandlerCoroutine(task, flags, stream, stream_payload_length)
    , m_reader(&m_increment, 4) {}

    async::Action act() override {
      return m_stream->readExactSizeDataAsyncInline(m_reader, yieldTo(&HandleWindowUpdateCoroutine::finalize));
    }

    async::Action finalize() {
      m_increment = ntohl(m_increment);
      if (m_increment == 0) {
        return error<protocol::http2::error::connection::ProtocolError>("[oatpp::web::server::http2::Http2StreamHandler::HandleWindowUpdateCoroutine::finalize()] Error: Increment of 0");
      }
      v_int32 window = m_task->window;
      if (window + m_increment > Http2Settings::getSettingMax(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)) {
        return error<protocol::http2::error::stream::FlowControlError>("[oatpp::web::server::http2::Http2StreamHandler::HandleWindowUpdateCoroutine::finalize()] Error: Increment above 2^31-1");
      }
      OATPP_LOGD(TAG, "Incrementing window by %u", m_increment);
      m_task->window += m_increment;
      return _return(m_task);
    }
  };

  class HandleContinuationCoroutine : public AbstractHandlerCoroutine<HandleContinuationCoroutine> {
   public:
    HandleContinuationCoroutine(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size stream_payload_length)
    : AbstractHandlerCoroutine(task, flags, stream, stream_payload_length) {
      m_task->headerFlags |= flags;
    }

    async::Action act() override {
      if (m_streamPayloadLength > 0) {
        async::Action action;
        auto res = m_task->header->readStreamToBuffer(m_stream.get(), m_streamPayloadLength, action);
        if (!action.isNone()) {
          return action;
        }
        m_streamPayloadLength -= res;
        if (m_streamPayloadLength > 0) {
          return repeat();
        }
      }
      return finalize(); // do not yield to, take a shortcut
    }

    async::Action finalize() {
      if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) == H2StreamHeaderFlags::HEADER_END_HEADERS) {
        m_task->setState(H2StreamState::PAYLOAD);
      } else if ((m_task->headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) {
        if (m_task->state != H2StreamState::PIPED) {
          m_task->setState(H2StreamState::READY);
        }
      }
      return _return(m_task);
    }
  };

  static const char* stateStringRepresentation(H2StreamState state);
};

}}}}

#endif //oatpp_web_server_http2_Http2StreamHandler_hpp
