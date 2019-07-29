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

#include "StatefulParserTest.hpp"

#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"

#include "oatpp/core/data/stream/BufferInputStream.hpp"

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
    "--12345--\r\n"
    ;


  void parseStepByStep(const oatpp::String& text,
                       const oatpp::String& boundary,
                       const std::shared_ptr<oatpp::web::mime::multipart::StatefulParser::Listener>& listener,
                       v_int32 step)
  {

    oatpp::web::mime::multipart::StatefulParser parser(boundary, listener, nullptr);

    oatpp::data::stream::BufferInputStream stream(text.getPtr(), text->getData(), text->getSize());
    v_char8 buffer[step];
    v_int32 size;
    while((size = stream.read(buffer, step)) != 0) {
      parser.parseNext(buffer, size);
    }

    OATPP_ASSERT(parser.finished());

  }

  void assertPartData(const std::shared_ptr<Part>& part, const oatpp::String& value) {

    OATPP_ASSERT(part->getInMemoryData());
    OATPP_ASSERT(part->getInMemoryData() == value);

    v_int32 bufferSize = 16;
    v_char8 buffer[bufferSize];

    oatpp::data::stream::ChunkedBuffer stream;
    oatpp::data::stream::DefaultWriteCallback writeCallback(&stream);
    oatpp::data::stream::transfer(part->getInputStream().get(), &writeCallback, 0, buffer, bufferSize);

    oatpp::String readData = stream.toString();

    OATPP_ASSERT(readData == part->getInMemoryData());

  }

}

void StatefulParserTest::onRun() {

  oatpp::String text = TEST_DATA_1;

  for(v_int32 i = 1; i < text->getSize(); i++) {

    oatpp::web::mime::multipart::Multipart multipart("12345");

    auto listener = std::make_shared<oatpp::web::mime::multipart::PartsParser>(&multipart);
    listener->setDefaultPartReader(std::make_shared<oatpp::web::mime::multipart::InMemoryPartReader>(128));

    parseStepByStep(text, "12345", listener, i);

    if(multipart.count() != 3) {
      OATPP_LOGD(TAG, "TEST_DATA_1 itearation %d", i);
    }

    OATPP_ASSERT(multipart.count() == 3);

    auto part1 = multipart.getNamedPart("part1");
    auto part2 = multipart.getNamedPart("part2");
    auto part3 = multipart.getNamedPart("part3");

    OATPP_ASSERT(part1);
    OATPP_ASSERT(part2);
    OATPP_ASSERT(part3);

    OATPP_ASSERT(part1->getFilename().get() == nullptr);
    OATPP_ASSERT(part2->getFilename() == "filename.txt");
    OATPP_ASSERT(part3->getFilename() == "filename.jpg");

    assertPartData(part1, "part1-value");
    assertPartData(part2, "--part2-file-content-line1\r\n--1234part2-file-content-line2");
    assertPartData(part3, "part3-file-binary-data");

  }

}

}}}}}
