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

#include "oatpp/web/server/http2/Http2Settings.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

const v_uint32 Http2Settings::PARAMETER_MINMAX[6][2] = {
    {0, UINT32_MAX},
    {0, 1},
    {0, UINT32_MAX},
    {1, 2147483647},
    {16384, 16777215},
    {0, UINT32_MAX}
};

v_uint32 Http2Settings::getSetting(Http2Settings::Identifier ident) const {
  switch (ident) {
    case SETTINGS_HEADER_TABLE_SIZE:
    case SETTINGS_ENABLE_PUSH:
    case SETTINGS_MAX_CONCURRENT_STREAMS:
    case SETTINGS_INITIAL_WINDOW_SIZE:
    case SETTINGS_MAX_FRAME_SIZE:
    case SETTINGS_MAX_HEADER_LIST_SIZE:
      return m_parameters[ident-1];
    default:
      throw std::runtime_error("[oatpp::web::server::http2::Http2Settings::getSetting] Error: Unknown identifier requested");
  }
}

void Http2Settings::setSetting(Http2Settings::Identifier ident, v_uint32 value) {
  switch (ident) {
    case SETTINGS_HEADER_TABLE_SIZE:
    case SETTINGS_ENABLE_PUSH:
    case SETTINGS_MAX_CONCURRENT_STREAMS:
    case SETTINGS_INITIAL_WINDOW_SIZE:
    case SETTINGS_MAX_FRAME_SIZE:
    case SETTINGS_MAX_HEADER_LIST_SIZE:
      if (value < PARAMETER_MINMAX[ident-1][0] || value > PARAMETER_MINMAX[ident-1][1]) {
        throw std::runtime_error("[oatpp::web::server::http2::Http2Settings::getSetting] Error: Tried to set an out-of-range value for parameter");
      }
      m_parameters[ident-1] = value;
      break;
    default:
      throw std::runtime_error("[oatpp::web::server::http2::Http2Settings::getSetting] Error: Unknown identifier requested");
  }
}

}}}}
