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

#ifndef oatpp_web_server_http2_Http2Settings_hpp
#define oatpp_web_server_http2_Http2Settings_hpp

#include <utility>

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/protocol/http2/Http2.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

class Http2Settings {
 public:
  enum Identifier : v_uint16 {
    SETTINGS_HEADER_TABLE_SIZE = 0x01,
    SETTINGS_ENABLE_PUSH = 0x02,
    SETTINGS_MAX_CONCURRENT_STREAMS = 0x03,
    SETTINGS_INITIAL_WINDOW_SIZE = 0x04,
    SETTINGS_MAX_FRAME_SIZE = 0x05,
    SETTINGS_MAX_HEADER_LIST_SIZE = 0x06
  };

 private:
  v_uint32 m_parameters[6];
  static const v_uint32 PARAMETER_MINMAX[6][2];

 public:
  explicit Http2Settings(
      v_uint32 tableSize = 4096,
      v_uint32 enablePush = 1,
      v_uint32 maxConcurrentStreams = UINT32_MAX,
      v_uint32 initialWindowSize = 65535,
      v_uint32 maxFrameSize = 16384,
      v_uint32 maxHeaderListSize = UINT32_MAX)
      : m_parameters{tableSize, enablePush, maxConcurrentStreams, initialWindowSize, maxFrameSize, maxHeaderListSize} {}

  static std::shared_ptr<Http2Settings> createShared(
      v_uint32 tableSize = 4096,
      v_uint32 enablePush = 1,
      v_uint32 maxConcurrentStreams = UINT32_MAX,
      v_uint32 initialWindowSize = 65535,
      v_uint32 maxFrameSize = 16384,
      v_uint32 maxHeaderListSize = UINT32_MAX) {
    return std::make_shared<Http2Settings>(tableSize, enablePush, maxConcurrentStreams, initialWindowSize, maxFrameSize, maxHeaderListSize);
  }

  v_uint32 getSetting(Identifier ident);
  void setSetting(Identifier ident, v_uint32 value);
};


}}}}

#endif //oatpp_web_server_http2_Http2Settings_hpp
