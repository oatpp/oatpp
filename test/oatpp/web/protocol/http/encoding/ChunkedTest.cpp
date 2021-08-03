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

#include "ChunkedTest.hpp"

#include "oatpp/web/protocol/http/encoding/Chunked.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace test { namespace web { namespace protocol { namespace http { namespace encoding {

void ChunkedTest::onRun() {

  oatpp::String data = "Hello World!!!";
  oatpp::String encoded;
  oatpp::String decoded;

  { // Empty string
    oatpp::data::stream::BufferInputStream inStream(oatpp::String(""));
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::EncoderChunked encoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &encoder);
    encoded = outStream.toString();

    OATPP_ASSERT(count == 0);
    OATPP_ASSERT(encoded == "0\r\n\r\n");
  }

  { // Empty string
    oatpp::data::stream::BufferInputStream inStream(encoded);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::DecoderChunked decoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &decoder);
    decoded = outStream.toString();
    OATPP_ASSERT(count == encoded->size());
    OATPP_ASSERT(decoded == "");
  }

  {
    oatpp::data::stream::BufferInputStream inStream(data);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::EncoderChunked encoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &encoder);
    encoded = outStream.toString();

    OATPP_ASSERT(count == data->size());
    OATPP_ASSERT(encoded == "5\r\nHello\r\n5\r\n Worl\r\n4\r\nd!!!\r\n0\r\n\r\n");
  }

  {
    oatpp::data::stream::BufferInputStream inStream(encoded);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::DecoderChunked decoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &decoder);
    decoded = outStream.toString();
    OATPP_ASSERT(count == encoded->size());
    OATPP_LOGD(TAG, "decoded='%s'", decoded->c_str());
    OATPP_ASSERT(decoded == data);
  }

  {
    oatpp::data::stream::BufferInputStream inStream(data);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::EncoderChunked encoder;
    oatpp::web::protocol::http::encoding::DecoderChunked decoder;
    oatpp::data::buffer::ProcessingPipeline pipeline({
      &encoder,
      &decoder
    });

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &pipeline);
    auto result = outStream.toString();
    OATPP_ASSERT(count == data->size());
    OATPP_LOGD(TAG, "result='%s'", result->c_str());
    OATPP_ASSERT(result == data);
  }


}

}}}}}}