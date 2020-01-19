/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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

#include "ChunkedBufferTest.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace stream {

void ChunkedBufferTest::onRun() {

  typedef oatpp::data::stream::ChunkedBuffer ChunkedBuffer;

  {
    ChunkedBuffer stream;

    stream  << "int=" << 1 << ", float=" << 1.1 << ", "
            << "bool=" << true << " or " << false;

    OATPP_LOGV(TAG, "str='%s'", stream.toString()->c_str());

    stream.clear();
    stream << 101;
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(101));

    stream.clear();
    stream << (v_float32)101.1;
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float32ToStr(101.1f));

    stream.clear();
    stream << (v_float64)101.1;
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float64ToStr(101.1));

    stream.clear();
    stream << true;
    OATPP_ASSERT(stream.toString() == "true");

    stream.clear();
    stream << false;
    OATPP_ASSERT(stream.toString() == "false");

    stream.clear();
    stream << oatpp::String("oat++");
    OATPP_ASSERT(stream.toString() == "oat++");

    stream.clear();
    stream << oatpp::Int8(8);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(8));

    stream.clear();
    stream << oatpp::Int16(16);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(16));

    stream.clear();
    stream << oatpp::Int32(32);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(32));

    stream.clear();
    stream << oatpp::Int64(64);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(64));

    stream.clear();
    stream << oatpp::Float32(0.32f);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float32ToStr(0.32f));

    stream.clear();
    stream << oatpp::Float64(0.64);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float64ToStr(0.64));

    stream.clear();
    stream << oatpp::Boolean(true);
    OATPP_ASSERT(stream.toString() == "true");

    stream.clear();
    stream << oatpp::Boolean(false);
    OATPP_ASSERT(stream.toString() == "false");

  }

  {

    ChunkedBuffer stream;

    for(v_int32 i = 0; i < ChunkedBuffer::CHUNK_ENTRY_SIZE * 10; i++) {
      stream.writeSimple("0123456789", 10);
    }

    auto wholeText = stream.toString();

    OATPP_ASSERT(wholeText->getSize() == ChunkedBuffer::CHUNK_ENTRY_SIZE * 10 * 10);

    v_int32 substringSize = 10;
    for(v_int32 i = 0; i < wholeText->getSize() - substringSize; i ++) {
      OATPP_ASSERT(oatpp::String((const char*)&wholeText->getData()[i], substringSize, false) == stream.getSubstring(i, substringSize));
    }

    substringSize = (v_int32) ChunkedBuffer::CHUNK_ENTRY_SIZE * 2;
    for(v_int32 i = 0; i < wholeText->getSize() - substringSize; i ++) {
      OATPP_ASSERT(oatpp::String((const char*)&wholeText->getData()[i], substringSize, false) == stream.getSubstring(i, substringSize));
    }

  }


}

}}}}}
