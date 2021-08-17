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
    INIT,
    HEADERS,
    CONTINUATION,
    PAYLOAD,
    PROCESSING,
    RESPONSE,
    GOAWAY,
    RESET,
    ABORTED,
    ERROR
  };

  typedef protocol::http2::Frame::Header::Flags::Header H2StreamHeaderFlags;
  typedef protocol::http2::Frame::Header::Flags::Data H2StreamDataFlags;

 private:
  class Task {
   public:
    std::atomic<H2StreamState> state;
    std::shared_ptr<protocol::http2::hpack::Hpack> hpack;
    std::shared_ptr<http2::processing::Components> components;
    std::shared_ptr<http2::PriorityStreamScheduler> output;
    std::shared_ptr<http2::Http2Settings> inSettings;
    std::shared_ptr<http2::Http2Settings> outSettings;
    std::shared_ptr<data::stream::FIFOInputStream> data;
    std::shared_ptr<data::stream::FIFOInputStream> header;
    std::exception_ptr error;
    v_uint32 streamId;
    v_uint32 dependency;
    v_uint8 weight;
    v_uint8 headerFlags;
    v_uint32 flow;

    Task(v_uint32 id, const std::shared_ptr<http2::PriorityStreamScheduler> &outputStream, const std::shared_ptr<protocol::http2::hpack::Hpack> &hpack, const std::shared_ptr<http2::processing::Components> &components, const std::shared_ptr<http2::Http2Settings> &inSettings, const std::shared_ptr<http2::Http2Settings> &outSettings)
        : state(INIT)
        , streamId(id)
        , output(outputStream)
        , hpack(hpack)
        , components(components)
        , dependency(0)
        , weight(0)
        , headerFlags(0)
        , error(nullptr)
        , flow(inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE))
        , inSettings(inSettings)
        , outSettings(outSettings)
        , header(data::stream::FIFOInputStream::createShared(inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)))
        , data(data::stream::FIFOInputStream::createShared(inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE))){}

    void setState(H2StreamState next);
  };

  std::thread m_processor;
  std::shared_ptr<Task> m_task;

 public:
  Http2StreamHandler(v_uint32 id, const std::shared_ptr<http2::PriorityStreamScheduler> &outputStream, const std::shared_ptr<protocol::http2::hpack::Hpack> &hpack, const std::shared_ptr<http2::processing::Components> &components, const std::shared_ptr<http2::Http2Settings> &inSettings, const std::shared_ptr<http2::Http2Settings> &outSettings)
    : m_task(std::make_shared<Task>(id, outputStream, hpack, components, inSettings, outSettings)) {
    sprintf(TAG, "oatpp::web::server::http2::Http2StreamHandler(%u)", id);
  }
  ~Http2StreamHandler() override {
    abort();
    waitForFinished();
  }

  ConnectionState handleData(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleHeaders(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handlePriority(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleResetStream(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handlePushPromise(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleWindowUpdate(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleContinuation(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);

  H2StreamState getState() {
    return m_task->state;
  }

  void abort();
  void waitForFinished();

  static const char* stateStringRepresentation(H2StreamState state);

 private:
  static void process(std::shared_ptr<Task> task);
};

}}}}

#endif //oatpp_web_server_http2_Http2StreamHandler_hpp
