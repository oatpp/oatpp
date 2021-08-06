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

#include "HpackDeflateTest.hpp"

#include "oatpp/web/protocol/http2/hpack/Hpack.hpp"

namespace oatpp { namespace test { namespace web { namespace protocol { namespace http2 { namespace hpack {

void HpackDeflateTest::onRun() {

  static const uint8_t expected[] = {
      0x41, 0x08, 0x6F, 0x61, 0x74, 0x70, 0x70, 0x2E, 0x69, 0x6F, 0x04, 0x15, 0x2F, 0x73, 0x74, 0x79,
      0x6C, 0x65, 0x73, 0x68, 0x65, 0x65, 0x74, 0x2F, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x2E, 0x63, 0x73,
      0x73, 0x87, 0x90, 0x73, 0x10, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x6F, 0x61, 0x74,
      0x70, 0x70, 0x2E, 0x69, 0x6F, 0x76, 0x05, 0x6F, 0x61, 0x74, 0x2B, 0x2B
  };
  oatpp::web::protocol::http2::Headers hdr;
  /*
   * MAKE_NV(":scheme", "https"),
     MAKE_NV(":authority", "example.org"),
     MAKE_NV(":path", "/stylesheet/style.css"),
     MAKE_NV("user-agent", "libnghttp2"),
     MAKE_NV("accept-encoding", "gzip, deflate"),
     MAKE_NV("referer", "https://example.org")
   */
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::SCHEME, "https");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::AUTHORITY, "oatpp.io");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::PATH, "/stylesheet/style.css");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::SERVER, "oat++");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::ACCEPT_ENCODING, "gzip, deflate");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::REFERER, "https://oatpp.io");

  oatpp::web::protocol::http2::hpack::Hpack hpack;

  auto deflated = hpack.deflate(hdr, 16 * 1024 - 1);
  OATPP_ASSERT(deflated.size() == 1);
  OATPP_ASSERT(deflated.front().size() == sizeof(expected));
  for (int i = 0; i < sizeof(expected); ++i) {
    OATPP_ASSERT(deflated.front()[i] == expected[i]);
  }

}

}}}}}}