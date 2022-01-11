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

#include "DTOMapperPerfTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/parser/json/mapping/Serializer.hpp"
#include "oatpp/parser/json/mapping/Deserializer.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include "oatpp-test/Checker.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {
  
namespace {

typedef oatpp::parser::json::mapping::Serializer Serializer;
typedef oatpp::parser::json::mapping::Deserializer Deserializer;

#include OATPP_CODEGEN_BEGIN(DTO)
  
  class Test1 : public oatpp::DTO {
    
    DTO_INIT(Test1, DTO)
    
    DTO_FIELD(String, field_string);
    DTO_FIELD(Int32, field_int32);
    DTO_FIELD(List<Int32>, field_list);
    
    static Wrapper createTestInstance(){
      auto result = Test1::createShared();
      result->field_string = "String Field";
      result->field_int32 = 5;
      result->field_list = List<Int32>::createShared();
      result->field_list->push_back(1);
      result->field_list->push_back(2);
      result->field_list->push_back(3);
      return result;
    }
    
  };
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
void DTOMapperPerfTest::onRun() {
  
  v_int32 numIterations = 1000000;

  auto serializer2 = std::make_shared<oatpp::parser::json::mapping::Serializer>();
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  auto test1 = Test1::createTestInstance();
  auto test1_Text = mapper->writeToString(test1);
  OATPP_LOGV(TAG, "json='%s'", test1_Text->c_str());

  {
    PerformanceChecker checker("Serializer");
    for(v_int32 i = 0; i < numIterations; i ++) {
      mapper->writeToString(test1);
    }
  }
  
  {
    PerformanceChecker checker("Deserializer");
    oatpp::parser::Caret caret(test1_Text);
    for(v_int32 i = 0; i < numIterations; i ++) {
      caret.setPosition(0);
      mapper->readFromCaret<oatpp::Object<Test1>>(caret);
    }
  }

}
  
}}}}}
