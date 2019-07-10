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

#include "oatpp/web/mime/multipart/StatefulParser.hpp"

namespace oatpp { namespace test { namespace web { namespace mime { namespace multipart {

namespace {

  static const char* TEST_DATA_1 =
    "--12345\r\n"
    "Content-Disposition: form-data; name=\"part1\"\r\n"
    "\r\n"
    "part1-value\r\n"
    "--12345\r\n"
    "Content-Disposition: form-data; name=\"part2\" filename=\"filename.txt\"\r\n"
    "\r\n"
    "--part2-file-content-line1\r\n"
    "--1234part2-file-content-line2\r\n"
    "--12345\r\n"
    "Content-Disposition: form-data; name=\"part3\" filename=\"filename.jpg\"\r\n"
    "\r\n"
    "part3-file-binary-data\r\n"
    "--12345--\r\n"
    ;

}

void StatefulParserTest::onRun() {

  oatpp::String text = TEST_DATA_1;

  {
    oatpp::web::mime::multipart::StatefulParser parser("12345");

    for (v_int32 i = 0; i < text->getSize(); i++) {
      parser.parseNext(&text->getData()[i], 1);
    }
  }

  OATPP_LOGI(TAG, "Test2.................................................");

  {
    oatpp::web::mime::multipart::StatefulParser parser("12345");
    parser.parseNext(text->getData(), text->getSize());
  }


}

}}}}}
