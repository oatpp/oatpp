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
#include "oatpp/data/stream/BufferStream.hpp"

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

    OATPP_ASSERT(count == 0)
    OATPP_ASSERT(encoded == "0\r\n\r\n")
  }

  { // Empty string
    oatpp::data::stream::BufferInputStream inStream(encoded);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::DecoderChunked decoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &decoder);
    decoded = outStream.toString();
    OATPP_ASSERT(count == static_cast<v_io_size>(encoded->size()))
    OATPP_ASSERT(decoded == "")
  }

  {
    oatpp::data::stream::BufferInputStream inStream(data);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::EncoderChunked encoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &encoder);
    encoded = outStream.toString();

    OATPP_ASSERT(count == static_cast<v_io_size>(data->size()))
    OATPP_ASSERT(encoded == "5\r\nHello\r\n5\r\n Worl\r\n4\r\nd!!!\r\n0\r\n\r\n")
  }

  {
    oatpp::data::stream::BufferInputStream inStream(encoded);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::web::protocol::http::encoding::DecoderChunked decoder;

    const v_int32 bufferSize = 5;
    v_char8 buffer[bufferSize];

    auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &decoder);
    decoded = outStream.toString();
    OATPP_ASSERT(count == static_cast<v_io_size>(encoded->size()))
    OATPP_LOGd(TAG, "decoded='{}'", decoded)
    OATPP_ASSERT(decoded == data)
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
    OATPP_ASSERT(count == static_cast<v_io_size>(data->size()))
    OATPP_LOGd(TAG, "result='{}'", result)
    OATPP_ASSERT(result == data)
  }

  { // Malformed chunk headers must not be decoded

    auto decode = [](const oatpp::String& input, oatpp::String& output) {
      oatpp::data::stream::BufferInputStream inStream(input);
      oatpp::data::stream::BufferOutputStream outStream;

      oatpp::web::protocol::http::encoding::DecoderChunked decoder;

      const v_int32 bufferSize = 5;
      v_char8 buffer[bufferSize];

      auto count = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, bufferSize, &decoder);
      output = outStream.toString();
      return count;
    };

    oatpp::String valid[] = {
      "5;a=b\r\nHello\r\n0\r\n\r\n",
      "5 ;a\r\nHello\r\n0\r\n\r\n",
      "5\r\nHello\r\n0;a\r\n\r\n"
    };

    for(auto& input : valid) {
      oatpp::String output;
      auto count = decode(input, output);
      OATPP_ASSERT(count == static_cast<v_io_size>(input->size()))
      OATPP_ASSERT(output == "Hello")
    }

    oatpp::String invalid[] = {
      "0x5\r\nHello\r\n0\r\n\r\n",
      "+5\r\nHello\r\n0\r\n\r\n",
      " 5\r\nHello\r\n0\r\n\r\n",
      "5x\r\nHello\r\n0\r\n\r\n",
      "G\r\n\r\n",
      "-0\r\n\r\n",
      "3\r\nHel4\r\nloWo\r\n0\r\n\r\n",
      "5\r\nHelloXX0\r\n\r\n"
    };

    for(auto& input : invalid) {
      oatpp::String output;
      auto count = decode(input, output);
      OATPP_ASSERT(count < static_cast<v_io_size>(input->size()))
    }

  }


}

}}}}}}
