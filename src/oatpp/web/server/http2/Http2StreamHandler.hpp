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

class Http2StreamHandler : public oatpp::base::Countable {
 private:
  char TAG[64];

 public:

  typedef protocol::http::utils::CommunicationUtils::ConnectionState ConnectionState;

  enum H2StreamState {
    INIT = 0,
    HEADERS = 1,
    CONTINUATION = 2,
    PAYLOAD = 3,
    PROCESSING = 4,
    RESPONDING = 5,
    DONE = 6,
    RESET = 7,
    ABORTED = 8,
    ERROR = 9
  };

  typedef protocol::http2::Frame::Header::Flags::Header H2StreamHeaderFlags;
  typedef protocol::http2::Frame::Header::Flags::Data H2StreamDataFlags;

 private:
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
    std::exception_ptr error;
    v_uint32 streamId;
    v_uint32 dependency;
    v_uint8 weight;
    v_uint8 headerFlags;
    v_int32 window;
    v_uint32 windowIncrement;

    Task(v_uint32 id, const std::shared_ptr<http2::PriorityStreamSchedulerAsync> &outputStream, const std::shared_ptr<protocol::http2::hpack::Hpack> &hpack, const std::shared_ptr<http2::processing::Components> &components, const std::shared_ptr<http2::Http2Settings> &inSettings, const std::shared_ptr<http2::Http2Settings> &outSettings)
        : state(INIT)
        , streamId(id)
        , output(outputStream)
        , hpack(hpack)
        , components(components)
        , dependency(0)
        , weight(0)
        , headerFlags(0)
        , error(nullptr)
        , windowIncrement(0)
        , inSettings(inSettings)
        , outSettings(outSettings)
        , window(static_cast<v_int32>(outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)))
        , header(data::stream::FIFOInputStream::createShared(inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)))
        , data(data::stream::FIFOInputStream::createShared(inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE))){}

    bool setStateWithExpection(H2StreamState expected, H2StreamState next);
    void setState(H2StreamState next);
    void clean();
    void resizeWindow(v_int32 change);
  };

  class WorkerResources {
   public:
    std::shared_ptr<protocol::http::incoming::Request> request;
    std::shared_ptr<http2::processing::Components> components;
    std::shared_ptr<protocol::http::outgoing::Response> response;

   public:
    WorkerResources(const std::shared_ptr<protocol::http::incoming::Request> &req, const std::shared_ptr<http2::processing::Components> &comp)
      : request(req)
      , components(comp) {}
  };

  class TaskWorker : public oatpp::async::CoroutineWaitList::Listener {
   public:
    /* oatpp::async::CoroutineWaitList::Listener::onNewItem */
    void onNewItem(oatpp::async::CoroutineWaitList& list) override;

   private:
    std::shared_ptr<WorkerResources> m_resources;
    oatpp::async::CoroutineWaitList m_waitList;
    bool m_done = false;
    std::mutex m_mutex;

   public:
    TaskWorker(const std::shared_ptr<WorkerResources> &resources);

    void run();
    void start();
    oatpp::async::CoroutineWaitList* getWaitList();
    bool isDone();

  };

  std::thread m_processor;
  std::shared_ptr<Task> m_task;

 public:
  Http2StreamHandler(v_uint32 id, const std::shared_ptr<http2::PriorityStreamSchedulerAsync> &outputStream, const std::shared_ptr<protocol::http2::hpack::Hpack> &hpack, const std::shared_ptr<http2::processing::Components> &components, const std::shared_ptr<http2::Http2Settings> &inSettings, const std::shared_ptr<http2::Http2Settings> &outSettings)
    : m_task(std::make_shared<Task>(id, outputStream, hpack, components, inSettings, outSettings)) {
    sprintf(TAG, "oatpp::web::server::http2::Http2StreamHandler(%u)", id);
  }
  ~Http2StreamHandler() override {
    abort();
    waitForFinished();
  }

  async::Action prepareWorker();
  async::Action processWorkerResult(const std::shared_ptr<protocol::http::outgoing::Response> &response);

  ///////

  async::Action handleData(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);
  async::Action handleHeaders(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);
  async::Action handlePriority(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);
  async::Action handleResetStream(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);
  async::Action handlePushPromise(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);
  async::Action handleWindowUpdate(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);
  async::Action handleContinuation(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength, async::Action &&nextAction);

  H2StreamState getState() {
    return m_task->state;
  }

  v_uint32 getStreamId() {
    return m_task->streamId;
  }

  void abort();
  void waitForFinished();
  void clean();
  void resizeWindow(v_int32 change);

  static const char* stateStringRepresentation(H2StreamState state);

 private:
  static void process(std::shared_ptr<Task> task);
  static void finalizeProcessAbortion(const std::shared_ptr<Task> &task);

  static void processError(std::shared_ptr<Task> task, protocol::http2::error::ErrorCode code);
};

}}}}

#endif //oatpp_web_server_http2_Http2StreamHandler_hpp
