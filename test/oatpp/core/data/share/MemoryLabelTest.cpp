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

#include "MemoryLabelTest.hpp"

#include "oatpp/core/data/share/MemoryLabel.hpp"

#include <unordered_map>

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp-test/Checker.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace share {

namespace {
  typedef oatpp::data::share::MemoryLabel MemoryLabel;
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
  typedef oatpp::data::share::StringKeyLabelCI StringKeyLabelCI;
}
  
void MemoryLabelTest::onRun() {

  {
    OATPP_LOGI(TAG, "StringKeyLabel default constructor...");
    StringKeyLabel s;
    StringKeyLabel s0;
    OATPP_ASSERT(!s);
    OATPP_ASSERT(s == nullptr);
    OATPP_ASSERT(s == s0);
    OATPP_ASSERT(s != "text");
    OATPP_ASSERT(s != oatpp::String("text"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabel nullptr constructor...");
    StringKeyLabel s(nullptr);
    OATPP_ASSERT(!s);
    OATPP_ASSERT(s == nullptr);
    OATPP_ASSERT(s != "text");
    OATPP_ASSERT(s != oatpp::String("text"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabel const char* constructor...");
    StringKeyLabel s("hello");
    StringKeyLabel s0;
    OATPP_ASSERT(s);
    OATPP_ASSERT(s != nullptr);
    OATPP_ASSERT(s != s0);
    OATPP_ASSERT(s0 != s);
    OATPP_ASSERT(s == "hello");
    OATPP_ASSERT(s == oatpp::String("hello"));
    OATPP_ASSERT(s != "text");
    OATPP_ASSERT(s != oatpp::String("text"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabel oatpp::String constructor...");
    StringKeyLabel s(oatpp::String("hello"));
    OATPP_ASSERT(s);
    OATPP_ASSERT(s != nullptr);
    OATPP_ASSERT(s == "hello");
    OATPP_ASSERT(s == oatpp::String("hello"));
    OATPP_ASSERT(s != "text");
    OATPP_ASSERT(s != oatpp::String("text"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabelCI default constructor...");
    StringKeyLabelCI s;
    StringKeyLabelCI s0;
    OATPP_ASSERT(!s);
    OATPP_ASSERT(s == nullptr);
    OATPP_ASSERT(s == s0);
    OATPP_ASSERT(s != "teXt");
    OATPP_ASSERT(s != oatpp::String("teXt"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabelCI nullptr constructor...");
    StringKeyLabelCI s(nullptr);
    OATPP_ASSERT(!s);
    OATPP_ASSERT(s == nullptr);
    OATPP_ASSERT(s != "teXt");
    OATPP_ASSERT(s != oatpp::String("teXt"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabelCI const char* constructor...");
    StringKeyLabelCI s("hello");
    StringKeyLabelCI s0;
    OATPP_ASSERT(s);
    OATPP_ASSERT(s != nullptr);
    OATPP_ASSERT(s != s0);
    OATPP_ASSERT(s0 != s);
    OATPP_ASSERT(s == "helLO");
    OATPP_ASSERT(s == oatpp::String("helLO"));
    OATPP_ASSERT(s != "text");
    OATPP_ASSERT(s != oatpp::String("teXt"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "StringKeyLabelCI oatpp::String constructor...");
    StringKeyLabelCI s(oatpp::String("hello"));
    OATPP_ASSERT(s);
    OATPP_ASSERT(s != nullptr);
    OATPP_ASSERT(s == "helLO");
    OATPP_ASSERT(s == oatpp::String("helLO"));
    OATPP_ASSERT(s != "text");
    OATPP_ASSERT(s != oatpp::String("teXt"));
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "general test...");

    oatpp::String sharedData = "big text goes here";
    oatpp::String key1 = "key1";
    oatpp::String key2 = "key2";
    oatpp::String key3 = "key3";
    oatpp::String key4 = "key4";

    std::unordered_map<StringKeyLabel, MemoryLabel> stringMap;
    std::unordered_map<StringKeyLabelCI, MemoryLabel> stringMapCI;

    // Case-Sensitive

    stringMap[key1] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[0], 3);
    stringMap[key2] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[4], 4);
    stringMap[key3] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[9], 4);
    stringMap[key4] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[14], 4);

    OATPP_ASSERT(stringMap["key1"].equals("big"));
    OATPP_ASSERT(stringMap["key2"].equals("text"));
    OATPP_ASSERT(stringMap["key3"].equals("goes"));
    OATPP_ASSERT(stringMap["key4"].equals("here"));

    OATPP_ASSERT(stringMap.find("Key1") == stringMap.end());
    OATPP_ASSERT(stringMap.find("Key2") == stringMap.end());
    OATPP_ASSERT(stringMap.find("Key3") == stringMap.end());
    OATPP_ASSERT(stringMap.find("Key4") == stringMap.end());


    // CI

    stringMapCI[key1] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[0], 3);
    stringMapCI[key2] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[4], 4);
    stringMapCI[key3] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[9], 4);
    stringMapCI[key4] = MemoryLabel(sharedData.getPtr(), &sharedData->data()[14], 4);

    OATPP_ASSERT(stringMapCI["key1"].equals("big"));
    OATPP_ASSERT(stringMapCI["key2"].equals("text"));
    OATPP_ASSERT(stringMapCI["key3"].equals("goes"));
    OATPP_ASSERT(stringMapCI["key4"].equals("here"));

    OATPP_ASSERT(stringMapCI["KEY1"].equals("big"));
    OATPP_ASSERT(stringMapCI["KEY2"].equals("text"));
    OATPP_ASSERT(stringMapCI["KEY3"].equals("goes"));
    OATPP_ASSERT(stringMapCI["KEY4"].equals("here"));

    {

      v_int32 iterationsCount = 100;

      oatpp::String headersText =
        "header0: value0\r\n"
        "header1: value1\r\n"
        "header2: value2\r\n"
        "header3: value3\r\n"
        "header4: value4\r\n"
        "header5: value5\r\n"
        "header6: value6\r\n"
        "header7: value7\r\n"
        "header8: value8\r\n"
        "header9: value9\r\n"
        "\r\n";

      {

        oatpp::test::PerformanceChecker timer("timer");

        for (v_int32 i = 0; i < iterationsCount; i++) {

          oatpp::parser::Caret caret(headersText);
          oatpp::web::protocol::http::Status status;
          oatpp::web::protocol::http::Headers headers;
          oatpp::web::protocol::http::Parser::parseHeaders(headers, headersText.getPtr(), caret, status);

          OATPP_ASSERT(status.code == 0);
          OATPP_ASSERT(headers.getSize() == 10);


          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header0").equals("value0", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header1").equals("value1", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header2").equals("value2", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header3").equals("value3", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header4").equals("value4", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header5").equals("value5", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header6").equals("value6", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header7").equals("value7", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header8").equals("value8", 6));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header9").equals("value9", 6));

          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header0").equals("value0"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header1").equals("value1"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header2").equals("value2"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header3").equals("value3"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header4").equals("value4"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header5").equals("value5"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header6").equals("value6"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header7").equals("value7"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header8").equals("value8"));
          OATPP_ASSERT(headers.getAsMemoryLabel<StringKeyLabel>("header9").equals("value9"));

        }

      }

    }

    OATPP_LOGI(TAG, "OK");

  }

  {

    v_int32 iterationsCount = 100;

    oatpp::String headersText =
      "header0: value0\r\n"
      "header0: value1\r\n"
      "header1: value2\r\n"
      "header1: value3\r\n"
      "header2: value4\r\n"
      "header2: value5\r\n"
      "header3: value6\r\n"
      "header3: value7\r\n"
      "header4: value8\r\n"
      "header4: value9\r\n"
      "\r\n";

    oatpp::parser::Caret caret(headersText);
    oatpp::web::protocol::http::Status status;
    oatpp::web::protocol::http::Headers headers;
    oatpp::web::protocol::http::Parser::parseHeaders(headers, headersText.getPtr(), caret, status);

    OATPP_ASSERT(status.code == 0);
    OATPP_ASSERT(headers.getSize() == 10);

    for(auto& h : headers.getAll()) {
      auto key = h.first.toString();
      auto val = h.second.toString();
      OATPP_LOGD(TAG, "'%s': '%s'", key->c_str(), val->c_str());
    }

  }

}
  
}}}}}
