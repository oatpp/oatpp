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

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/protocol/http2/Http2.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

class Http2StreamHandler : public oatpp::base::Countable {
 private:
  v_uint32 m_streamId;

 public:
  Http2StreamHandler(v_uint32 id)
    : m_streamId(id) {}

  void handleData(v_uint8 flags, const protocol::http2::Payload &payload);
  void handleHeaders(v_uint8 flags, const protocol::http2::Payload &payload);
  void handlePriority(v_uint8 flags, const protocol::http2::Payload &payload);
  void handleResetStream(v_uint8 flags, const protocol::http2::Payload &payload);
  void handleSettings(v_uint8 flags, const protocol::http2::Payload &payload);
  void handlePushPromise(v_uint8 flags, const protocol::http2::Payload &payload);
  void handlePing(v_uint8 flags, const protocol::http2::Payload &payload);
  void handleGoAway(v_uint8 flags, const protocol::http2::Payload &payload);
  void handleWindowUpdate(v_uint8 flags, const protocol::http2::Payload &payload);
  void handleContinuation(v_uint8 flags, const protocol::http2::Payload &payload);
};

}}}}

#endif //oatpp_web_server_http2_Http2StreamHandler_hpp
