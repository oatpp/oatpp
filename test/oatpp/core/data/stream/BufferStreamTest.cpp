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

#include "BufferStreamTest.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/utils/Binary.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace stream {

void BufferStreamTest::onRun() {

  typedef oatpp::data::stream::BufferOutputStream BufferOutputStream;

  {
    BufferOutputStream stream;

    stream  << "int=" << 1 << ", float=" << 1.1 << ", "
            << "bool=" << true << " or " << false;

    OATPP_LOGV(TAG, "str='%s'", stream.toString()->c_str());

    stream.setCurrentPosition(0);
    stream << 101;
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(101));

    stream.setCurrentPosition(0);
    stream << (v_float32)101.1;
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float32ToStr(101.1f));

    stream.setCurrentPosition(0);
    stream << (v_float64)101.1;
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float64ToStr(101.1));

    stream.setCurrentPosition(0);
    stream << true;
    OATPP_ASSERT(stream.toString() == "true");

    stream.setCurrentPosition(0);
    stream << false;
    OATPP_ASSERT(stream.toString() == "false");

    stream.setCurrentPosition(0);
    stream << oatpp::String("oat++");
    OATPP_ASSERT(stream.toString() == "oat++");

    stream.setCurrentPosition(0);
    stream << oatpp::Int8(8);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(8));

    stream.setCurrentPosition(0);
    stream << oatpp::Int16(16);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(16));

    stream.setCurrentPosition(0);
    stream << oatpp::Int32(32);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(32));

    stream.setCurrentPosition(0);
    stream << oatpp::Int64(64);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::int32ToStr(64));

    stream.setCurrentPosition(0);
    stream << oatpp::Float32(0.32f);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float32ToStr(0.32f));

    stream.setCurrentPosition(0);
    stream << oatpp::Float64(0.64);
    OATPP_ASSERT(stream.toString() == oatpp::utils::conversion::float64ToStr(0.64));

    stream.setCurrentPosition(0);
    stream << oatpp::Boolean(true);
    OATPP_ASSERT(stream.toString() == "true");

    stream.setCurrentPosition(0);
    stream << oatpp::Boolean(false);
    OATPP_ASSERT(stream.toString() == "false");

  }

  {

    BufferOutputStream stream;
    v_int32 fragmentsCount = 1024 * 10;

    for(v_int32 i = 0; i < fragmentsCount; i++) {
      stream.writeSimple("0123456789", 10);
    }

    auto wholeText = stream.toString();

    OATPP_ASSERT(wholeText->size() == fragmentsCount * 10);

    v_int32 substringSize = 10;
    for(v_int32 i = 0; i < wholeText->size() - substringSize; i ++) {
      OATPP_ASSERT(oatpp::String(&wholeText->data()[i], substringSize) == stream.getSubstring(i, substringSize));
    }

  }

  {

    oatpp::String sample = "0123456789";
    oatpp::String text = "";

    for(v_int32 i = 0; i < 1024; i++ ) {
      text = text + sample;
    }

    BufferOutputStream stream(0);

    for(v_int32 i = 0; i < 1024; i++ ) {
      stream << sample;

      OATPP_ASSERT(stream.getCapacity() >= stream.getCurrentPosition());

    }

    OATPP_ASSERT(text == stream.toString());

    OATPP_ASSERT(stream.getCapacity() == oatpp::utils::Binary::nextP2(1024 * (10)));

  }

}

}}}}}