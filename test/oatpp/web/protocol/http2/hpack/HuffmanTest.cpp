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

#include "HuffmanTest.hpp"

#include "oatpp/core/data/stream/FIFOStream.hpp"
#include "oatpp/web/protocol/http2/hpack/Huffman.hpp"

namespace oatpp { namespace test { namespace web { namespace protocol { namespace http2 { namespace hpack {

void HuffmanTest::onRun() {

  v_uint8 input[] = "oatpp.io";
  static const uint8_t expected[] = {
    0x38, 0xd3, 0x5d, 0x6b, 0x98, 0xff
  };

  {
    oatpp::data::stream::FIFOInputStream ffip;
    oatpp::web::protocol::http2::hpack::Huffman huf;
    auto ret = huf.encode(&ffip, input, 8);
    OATPP_ASSERT(ret == sizeof(expected));
    OATPP_ASSERT(ffip.availableToRead() == sizeof(expected));

    v_uint8 extracted[sizeof(expected)];
    ffip.readExactSizeDataSimple((void*)extracted, sizeof(expected));
    for (int i = 0; i < sizeof(expected); ++i) {
      OATPP_ASSERT(extracted[i] == expected[i]);
    }
  }


}

}}}}}}