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

#include "DeserializerTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

typedef oatpp::parser::Caret ParsingCaret;
typedef oatpp::parser::json::mapping::Serializer Serializer;
typedef oatpp::parser::json::mapping::Deserializer Deserializer;

class EmptyDto : public oatpp::DTO {

  DTO_INIT(EmptyDto, DTO)

};

class Test1 : public oatpp::DTO {
  
  DTO_INIT(Test1, DTO)
  
  DTO_FIELD(String, strF);
  
};
  
class Test2 : public oatpp::DTO {
  
  DTO_INIT(Test2, DTO)
  
  DTO_FIELD(Int32, int32F);
  
};
  
class Test3 : public oatpp::DTO {
  
  DTO_INIT(Test3, DTO)
  
  DTO_FIELD(Float32, float32F);
  
};

class Test4 : public oatpp::DTO {

  DTO_INIT(Test4, DTO)

  DTO_FIELD(Object<EmptyDto>, object);
  DTO_FIELD(List<Object<EmptyDto>>, list);
  DTO_FIELD(Fields<Object<EmptyDto>>, map);

};

class AnyDto : public oatpp::DTO {

  DTO_INIT(AnyDto, DTO)

  DTO_FIELD(Any, any);

};

#include OATPP_CODEGEN_END(DTO)
  
}
  
void DeserializerTest::onRun(){
  
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  auto obj1 = mapper->readFromString<oatpp::Object<Test1>>("{}");
  
  OATPP_ASSERT(obj1);
  OATPP_ASSERT(!obj1->strF);
  
  obj1 = mapper->readFromString<oatpp::Object<Test1>>("{\"strF\":\"value1\"}");
  
  OATPP_ASSERT(obj1);
  OATPP_ASSERT(obj1->strF);
  OATPP_ASSERT(obj1->strF == "value1");
  
  obj1 = mapper->readFromString<oatpp::Object<Test1>>("{\n\r\t\f\"strF\"\n\r\t\f:\n\r\t\f\"value1\"\n\r\t\f}");
  
  OATPP_ASSERT(obj1);
  OATPP_ASSERT(obj1->strF);
  OATPP_ASSERT(obj1->strF == "value1");
  
  auto obj2 = mapper->readFromString<oatpp::Object<Test2>>("{\"int32F\": null}");
  
  OATPP_ASSERT(obj2);
  OATPP_ASSERT(!obj2->int32F);
  
  obj2 = mapper->readFromString<oatpp::Object<Test2>>("{\"int32F\": 32}");
  
  OATPP_ASSERT(obj2);
  OATPP_ASSERT(obj2->int32F == 32);
  
  obj2 = mapper->readFromString<oatpp::Object<Test2>>("{\"int32F\":    -32}");
  
  OATPP_ASSERT(obj2);
  OATPP_ASSERT(obj2->int32F == -32);
  
  auto obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": null}");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(!obj3->float32F);
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 32}");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(obj3->float32F == 32);
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 1.32e1}");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(obj3->float32F);
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 1.32e+1 }");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(obj3->float32F);
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 1.32e-1 }");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(obj3->float32F);
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": -1.32E-1 }");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(obj3->float32F);
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": -1.32E1 }");
  
  OATPP_ASSERT(obj3);
  OATPP_ASSERT(obj3->float32F);
  
  auto list = mapper->readFromString<oatpp::List<oatpp::Int32>>("[1, 2, 3]");
  OATPP_ASSERT(list);
  OATPP_ASSERT(list->size() == 3);
  OATPP_ASSERT(list[0] == 1);
  OATPP_ASSERT(list[1] == 2);
  OATPP_ASSERT(list[2] == 3);

  // Empty test

  auto obj4 = mapper->readFromString<oatpp::Object<Test4>>("{\"object\": {}, \"list\": [], \"map\": {}}");
  OATPP_ASSERT(obj4);
  OATPP_ASSERT(obj4->object);
  OATPP_ASSERT(obj4->list);
  OATPP_ASSERT(obj4->list->size() == 0);
  OATPP_ASSERT(obj4->map->size() == 0);

  obj4 = mapper->readFromString<oatpp::Object<Test4>>("{\"object\": {\n\r\t}, \"list\": [\n\r\t], \"map\": {\n\r\t}}");
  OATPP_ASSERT(obj4);
  OATPP_ASSERT(obj4->object);
  OATPP_ASSERT(obj4->list);
  OATPP_ASSERT(obj4->list->size() == 0);
  OATPP_ASSERT(obj4->map->size() == 0);

  OATPP_LOGD(TAG, "Any: String")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":"my_string"})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == String::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<String>() == "my_string");
  }
  OATPP_LOGD(TAG, "Any: Boolean")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":false})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == Boolean::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<Boolean>() == false);
  }
  OATPP_LOGD(TAG, "Any: Negative Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":-1.23456789,"another":1.1})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<Float64>() == -1.23456789);
  }
  OATPP_LOGD(TAG, "Any: Positive Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":1.23456789,"another":1.1})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<Float64>() == 1.23456789);
  }
  OATPP_LOGD(TAG, "Any: Negative exponential Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":-1.2345e30,"another":1.1})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<Float64>() == -1.2345e30);
  }
  OATPP_LOGD(TAG, "Any: Positive exponential Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":1.2345e30,"another":1.1})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<Float64>() == 1.2345e30);
  }
  OATPP_LOGD(TAG, "Any: Unsigned Integer")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":12345678901234567890,"another":1.1})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == UInt64::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<UInt64>() == 12345678901234567890u);
  }
  OATPP_LOGD(TAG, "Any: Signed Integer")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":-1234567890,"another":1.1})");
    OATPP_ASSERT(dto);
    OATPP_ASSERT(dto->any.getStoredType() == Int64::Class::getType());
    OATPP_ASSERT(dto->any.retrieve<Int64>() == -1234567890);
  }

}
  
}}}}}
