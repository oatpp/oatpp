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
    PROCESSING = 5,
    RESPONDING = 6,
    DONE = 7,
    RESET = 8,
    ABORTED = 9,
    ERROR = 10
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

    void start();
    oatpp::async::CoroutineWaitList* getWaitList();
    bool isDone();

   private:
    void run();
  };

  Http2StreamHandler() = delete;
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

  ///////

  static H2StreamState handleData(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);
  static H2StreamState handleHeaders(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);
  static H2StreamState handlePriority(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);
  static H2StreamState handleResetStream(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);
  static H2StreamState handlePushPromise(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);
  static H2StreamState handleWindowUpdate(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);
  static H2StreamState handleContinuation(const std::shared_ptr<Task> &task, v_uint8 flags, const std::shared_ptr<data::stream::InputStream> &stream, v_io_size streamPayloadLength);

  static const char* stateStringRepresentation(H2StreamState state);
};

}}}}

#endif //oatpp_web_server_http2_Http2StreamHandler_hpp
