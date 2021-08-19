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
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace test { namespace web { namespace protocol { namespace http2 { namespace hpack {

void HpackDeflateTest::onRun() {

  static const uint8_t expected[] = {
      0x41, 0x86, 0x38, 0xD3, 0x5D, 0x6B, 0x98, 0xFF, 0x04, 0x8F, 0x61, 0x09, 0xF5, 0x41, 0x51, 0x39,
      0x4A, 0x96, 0x10, 0x9F, 0x54, 0x15, 0x72, 0x21, 0x1F, 0x87, 0x90, 0x73, 0x8C, 0x9D, 0x29, 0xAD,
      0x17, 0x18, 0x60, 0xE3, 0x4D, 0x75, 0xAE, 0x63, 0xFF, 0x76, 0x05, 0x6F, 0x61, 0x74, 0x2B, 0x2B
  };
  oatpp::web::protocol::http2::Headers hdr;

  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::SCHEME, "https");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::AUTHORITY, "oatpp.io");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::PATH, "/stylesheet/style.css");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::SERVER, "oat++");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::ACCEPT_ENCODING, "gzip, deflate");
  hdr.putIfNotExists(oatpp::web::protocol::http2::Header::REFERER, "https://oatpp.io");

  {
    auto deflateTable = std::make_shared<oatpp::web::protocol::http2::hpack::SimpleTable>(1024);
    auto inflateTable = std::make_shared<oatpp::web::protocol::http2::hpack::SimpleTable>(1024);
    oatpp::web::protocol::http2::hpack::SimpleHpack deflater(deflateTable), inflater(inflateTable);

    v_int64 ticks = base::Environment::getMicroTickCount();
    auto deflated = deflater.deflate(hdr);
    v_int64 deflateTicks = base::Environment::getMicroTickCount() - ticks;
    OATPP_ASSERT(deflated->availableToRead() == sizeof(expected));
    v_uint8 extracted[sizeof(expected)];
    deflated->readSimple((void *) extracted, sizeof(extracted));
    for (int i = 0; i < sizeof(expected); ++i) {
      OATPP_ASSERT(extracted[i] == expected[i]);
    }

  }

  {
    auto deflateTable = std::make_shared<oatpp::web::protocol::http2::hpack::SimpleTable>(1024);
    auto inflateTable = std::make_shared<oatpp::web::protocol::http2::hpack::SimpleTable>(1024);
    oatpp::web::protocol::http2::hpack::SimpleHpack deflater(deflateTable), inflater(inflateTable);

    v_int64 ticks = base::Environment::getMicroTickCount();
    auto deflated = deflater.deflate(hdr);
    v_int64 deflateTicks = base::Environment::getMicroTickCount() - ticks;
    OATPP_ASSERT(deflated->availableToRead() == sizeof(expected));

    ticks = base::Environment::getMicroTickCount();
    auto inflated = inflater.inflate(deflated, deflated->availableToRead());
    v_int64 inflateTicks = base::Environment::getMicroTickCount() - ticks;
    OATPP_ASSERT(inflated.getSize() == hdr.getSize());
    auto originalmap = hdr.getAll();
    auto inflatedmap = inflated.getAll();

    for (auto it = originalmap.begin(); it != originalmap.end(); ++it) {
      auto found = inflatedmap.find(it->first);
      OATPP_ASSERT(found != inflatedmap.end());
      OATPP_ASSERT(found->second = it->second);
    }
    OATPP_LOGI(TAG, "Payload: Deflating \033[33m%d(micro)\033[0m, Inflating \033[33m%d(micro)\033[0m", deflateTicks, inflateTicks);
  }
}

}}}}}}