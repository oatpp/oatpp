/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
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

#include "StatefulParserTest.hpp"

#include "oatpp/web/mime/multipart/PartList.hpp"
#include "oatpp/web/mime/multipart/InMemoryDataProvider.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

#include <unordered_map>

namespace oatpp { namespace test { namespace web { namespace mime { namespace multipart {

namespace {

  typedef oatpp::web::mime::multipart::Part Part;

  static const char* TEST_DATA_1 =
    "--12345\r\n"
    "Content-Disposition: form-data; name=\"part1\"\r\n"
    "\r\n"
    "part1-value\r\n"
    "--12345\r\n"
    "Content-Disposition: form-data; name='part2' filename=\"filename.txt\"\r\n"
    "\r\n"
    "--part2-file-content-line1\r\n"
    "--1234part2-file-content-line2\r\n"
    "--12345\r\n"
    "Content-Disposition: form-data; name=part3 filename=\"filename.jpg\"\r\n"
    "\r\n"
    "part3-file-binary-data\r\n"
    "--12345\r\n"
    "Content-Disposition: form-data; name=\"part4\"\r\n"
    "\r\n"
    "part4-first-value\r\n"
    "--12345\r\n"
    "Content-Disposition: form-data; name=\"part4\"\r\n"
    "\r\n"
    "part4-second-value\r\n"
    "--12345--\r\n"
    ;


  void parseStepByStep(const oatpp::String& text,
                       const oatpp::String& boundary,
                       const std::shared_ptr<oatpp::web::mime::multipart::StatefulParser::Listener>& listener,
                       const v_int32 step)
  {

    oatpp::web::mime::multipart::StatefulParser parser(boundary, listener, nullptr);

    oatpp::data::stream::BufferInputStream stream(text.getPtr(), text->data(), text->size());
    std::unique_ptr<v_char8[]> buffer(new v_char8[step]);
    v_io_size size;
    while((size = stream.readSimple(buffer.get(), step)) != 0) {
      oatpp::data::buffer::InlineWriteData inlineData(buffer.get(), size);
      while(inlineData.bytesLeft > 0 && !parser.finished()) {
        oatpp::async::Action action;
        parser.parseNext(inlineData, action);
      }
    }
    OATPP_ASSERT(parser.finished());

  }

  void assertPartData(const std::shared_ptr<Part>& part, const oatpp::String& value) {

    auto payload = part->getPayload();
    OATPP_ASSERT(payload)
    OATPP_ASSERT(payload->getInMemoryData());
    OATPP_ASSERT(payload->getInMemoryData() == value);

    v_int64 bufferSize = 16;
    std::unique_ptr<v_char8[]> buffer(new v_char8[bufferSize]);

    oatpp::data::stream::BufferOutputStream stream;
    oatpp::data::stream::transfer(payload->openInputStream(), &stream, 0, buffer.get(), bufferSize);

    oatpp::String readData = stream.toString();

    OATPP_ASSERT(readData == payload->getInMemoryData());

  }

}

void StatefulParserTest::onRun() {

  oatpp::String text = TEST_DATA_1;

  for(v_int32 i = 1; i < text->size(); i++) {

    oatpp::web::mime::multipart::PartList multipart("12345");

    auto listener = std::make_shared<oatpp::web::mime::multipart::PartsParser>(&multipart);
    listener->setDefaultPartReader(oatpp::web::mime::multipart::createInMemoryPartReader(128));

    parseStepByStep(text, "12345", listener, i);

    if(multipart.count() != 5) {
      OATPP_LOGD(TAG, "TEST_DATA_1 itearation %d", i);
    }

    OATPP_ASSERT(multipart.count() == 5);

    auto part1 = multipart.getNamedPart("part1");
    auto part2 = multipart.getNamedPart("part2");
    auto part3 = multipart.getNamedPart("part3");

    auto part4 = multipart.getNamedPart("part4");
    auto part4List = multipart.getNamedParts("part4");

    OATPP_ASSERT(part1);
    OATPP_ASSERT(part2);
    OATPP_ASSERT(part3);
    OATPP_ASSERT(part4);
    OATPP_ASSERT(part4List.size() == 2);
    OATPP_ASSERT(part4List.front().get() == part4.get());

    OATPP_ASSERT(part1->getFilename().get() == nullptr);
    OATPP_ASSERT(part2->getFilename() == "filename.txt");
    OATPP_ASSERT(part3->getFilename() == "filename.jpg");

    assertPartData(part1, "part1-value");
    assertPartData(part2, "--part2-file-content-line1\r\n--1234part2-file-content-line2");
    assertPartData(part3, "part3-file-binary-data");
    assertPartData(part4List.front(), "part4-first-value");
    assertPartData(part4List.back(), "part4-second-value");

  }

}

}}}}}
