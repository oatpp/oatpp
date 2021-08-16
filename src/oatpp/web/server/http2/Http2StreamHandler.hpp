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
#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/web/protocol/http2/hpack/Hpack.hpp"

#include "oatpp/web/protocol/http/utils/CommunicationUtils.hpp"
#include "oatpp/web/protocol/http2/Http2.hpp"

#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"
#include "oatpp/web/server/http2/PriorityStreamScheduler.hpp"

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
    RESET
  };

  typedef protocol::http2::Frame::Header::Flags::Header H2StreamHeaderFlags;
  typedef protocol::http2::Frame::Header::Flags::Data H2StreamDataFlags;

 private:
  std::atomic<H2StreamState> m_state;
  v_uint32 m_streamId;
  v_uint32 m_dependency;
  v_uint8 m_weight;
  v_uint8 m_headerFlags;
  v_uint32 m_flow;
  std::shared_ptr<protocol::http2::hpack::Hpack> m_hpack;
  std::shared_ptr<http2::processing::Components> m_components;
  std::shared_ptr<http2::PriorityStreamScheduler> m_output;
  oatpp::web::protocol::http2::Headers m_headers;
  data::stream::BufferOutputStream m_data;

 public:
  Http2StreamHandler(v_uint32 id, const std::shared_ptr<http2::PriorityStreamScheduler> &outputStream, const std::shared_ptr<protocol::http2::hpack::Hpack> &hpack, const std::shared_ptr<http2::processing::Components> &components)
    : m_state(INIT)
    , m_streamId(id)
    , m_output(outputStream)
    , m_hpack(hpack)
    , m_components(components)
    , m_dependency(0)
    , m_weight(0)
    , m_flow (65535)
    , m_data(m_flow) {
    sprintf(TAG, "oatpp::web::server::http2::Http2StreamHandler(%u)", m_streamId);
  }

  ConnectionState handleData(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleHeaders(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handlePriority(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleResetStream(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handlePushPromise(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleGoAway(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleWindowUpdate(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);
  ConnectionState handleContinuation(v_uint8 flags, const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);

  H2StreamState getState() {
    return m_state;
  }

  static const char* stateStringRepresentation(H2StreamState state);

 private:
  void process();
  void setState(H2StreamState next);
};

}}}}

#endif //oatpp_web_server_http2_Http2StreamHandler_hpp
