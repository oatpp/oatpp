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
  
void MemoryLabelTest::onRun() {
  
  oatpp::String sharedData = "big text goes here";
  oatpp::String key1 = "key1";
  oatpp::String key2 = "key2";
  oatpp::String key3 = "key3";
  oatpp::String key4 = "key4";
  
  std::unordered_map<oatpp::data::share::StringKeyLabel, oatpp::data::share::MemoryLabel> stringMap;
  std::unordered_map<oatpp::data::share::StringKeyLabelCI, oatpp::data::share::MemoryLabel> stringMapCI;
  std::unordered_map<oatpp::data::share::StringKeyLabelCI_FAST, oatpp::data::share::MemoryLabel> stringMapCI_FAST;
  
  // Case-Sensitive
  
  stringMap[key1] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[0], 3);
  stringMap[key2] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[4], 4);
  stringMap[key3] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[9], 4);
  stringMap[key4] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[14], 4);
  
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("big", stringMap["key1"].getData(), 3));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("text", stringMap["key2"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("goes", stringMap["key3"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("here", stringMap["key4"].getData(), 4));
  
  OATPP_ASSERT(stringMap.find("Key1") == stringMap.end());
  OATPP_ASSERT(stringMap.find("Key2") == stringMap.end());
  OATPP_ASSERT(stringMap.find("Key3") == stringMap.end());
  OATPP_ASSERT(stringMap.find("Key4") == stringMap.end());

  
  // CI
  
  stringMapCI[key1] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[0], 3);
  stringMapCI[key2] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[4], 4);
  stringMapCI[key3] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[9], 4);
  stringMapCI[key4] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[14], 4);
  
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("big", stringMapCI["key1"].getData(), 3));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("text", stringMapCI["key2"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("goes", stringMapCI["key3"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("here", stringMapCI["key4"].getData(), 4));
  
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("big", stringMapCI["KEY1"].getData(), 3));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("text", stringMapCI["KEY2"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("goes", stringMapCI["KEY3"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("here", stringMapCI["KEY4"].getData(), 4));

  
  // CI_FAST
    
  stringMapCI_FAST[key1] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[0], 3);
  stringMapCI_FAST[key2] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[4], 4);
  stringMapCI_FAST[key3] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[9], 4);
  stringMapCI_FAST[key4] = oatpp::data::share::MemoryLabel(sharedData.getPtr(), &sharedData->getData()[14], 4);
  
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("big", stringMapCI_FAST["key1"].getData(), 3));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("text", stringMapCI_FAST["key2"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("goes", stringMapCI_FAST["key3"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("here", stringMapCI_FAST["key4"].getData(), 4));
  
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("big", stringMapCI_FAST["KEY1"].getData(), 3));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("text", stringMapCI_FAST["KEY2"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("goes", stringMapCI_FAST["KEY3"].getData(), 4));
  OATPP_ASSERT(oatpp::base::StrBuffer::equals("here", stringMapCI_FAST["KEY4"].getData(), 4));
  
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
      
      for(v_int32 i = 0; i < iterationsCount; i ++) {
        
        oatpp::parser::Caret caret(headersText);
        oatpp::web::protocol::http::Status status;
        oatpp::web::protocol::http::Headers headers;
        oatpp::web::protocol::http::Parser::parseHeaders(headers, headersText.getPtr(), caret, status);
        
        OATPP_ASSERT(status.code == 0);
        OATPP_ASSERT(headers.size() == 10);
        
        
        OATPP_ASSERT(headers["header0"].equals("value0", 6));
        OATPP_ASSERT(headers["header1"].equals("value1", 6));
        OATPP_ASSERT(headers["header2"].equals("value2", 6));
        OATPP_ASSERT(headers["header3"].equals("value3", 6));
        OATPP_ASSERT(headers["header4"].equals("value4", 6));
        OATPP_ASSERT(headers["header5"].equals("value5", 6));
        OATPP_ASSERT(headers["header6"].equals("value6", 6));
        OATPP_ASSERT(headers["header7"].equals("value7", 6));
        OATPP_ASSERT(headers["header8"].equals("value8", 6));
        OATPP_ASSERT(headers["header9"].equals("value9", 6));

        OATPP_ASSERT(headers["header0"].equals("value0"));
        OATPP_ASSERT(headers["header1"].equals("value1"));
        OATPP_ASSERT(headers["header2"].equals("value2"));
        OATPP_ASSERT(headers["header3"].equals("value3"));
        OATPP_ASSERT(headers["header4"].equals("value4"));
        OATPP_ASSERT(headers["header5"].equals("value5"));
        OATPP_ASSERT(headers["header6"].equals("value6"));
        OATPP_ASSERT(headers["header7"].equals("value7"));
        OATPP_ASSERT(headers["header8"].equals("value8"));
        OATPP_ASSERT(headers["header9"].equals("value9"));
         
      }
      
    }

  }

}
  
}}}}}
