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

#include "DTOMapperTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"

#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {
  
namespace {
  
#include OATPP_CODEGEN_BEGIN(DTO)
  
class TestChild : public oatpp::Object {
  
  DTO_INIT(TestChild, Object)
  
  static ObjectWrapper createShared(const char* name, const char* secondName){
    auto result = createShared();
    result->name = name;
    result->secondName = secondName;
    return result;
  }
  
  DTO_FIELD(String, name) = "Name";
  DTO_FIELD(String, secondName) = "Second Name";
  
};

class Test : public oatpp::Object {
  
  DTO_INIT(Test, Object)
  
  DTO_FIELD(String, field_string, "string-field-name-qualifier");
  DTO_FIELD(Int32, field_int32, "int32-field-name-qualifier");
  DTO_FIELD(Int64, field_int64);
  DTO_FIELD(Float32, field_float32);
  DTO_FIELD(Float64, field_float64);
  DTO_FIELD(Boolean, field_boolean);
  
  DTO_FIELD(List<String>, field_list_string) = List<String>::createShared();
  DTO_FIELD(List<Int32>, field_list_int32) = List<Int32>::createShared();
  DTO_FIELD(List<Int64>, field_list_int64) = List<Int64>::createShared();
  DTO_FIELD(List<Float32>, field_list_float32) = List<Float32>::createShared();
  DTO_FIELD(List<Float64>, field_list_float64) = List<Float64>::createShared();
  DTO_FIELD(List<Boolean>, field_list_boolean) = List<Boolean>::createShared();
  
  DTO_FIELD(List<TestChild>, field_list_object) = List<TestChild>::createShared();
  DTO_FIELD(List<List<TestChild>>, field_list_list_object) = List<List<TestChild>>::createShared();
  
  DTO_FIELD(Test, obj1);
  DTO_FIELD(TestChild, child1);
  
};

class TestAny : public oatpp::Object {

  DTO_INIT(TestAny, Object)

  DTO_FIELD(List<Any>, anyList) = List<Any>::createShared();

};

class TestAnyNested : public oatpp::Object {

  DTO_INIT(TestAnyNested, Object)

  DTO_FIELD(String, f1) = "Field_1";
  DTO_FIELD(String, f2) = "Field_2";

};
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
void DTOMapperTest::onRun(){
  
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  mapper->getSerializer()->getConfig()->useBeautifier = true;

  Test::ObjectWrapper test1 = Test::createShared();
  
  test1->field_string = "string value";
  test1->field_int32 = 32;
  test1->field_int64 = 64;
  test1->field_float32 = 0.32f;
  test1->field_float64 = 0.64;
  test1->field_boolean = true;
  
  test1->obj1 = Test::createShared();
  test1->obj1->field_string = "inner string";
  test1->obj1->field_list_string->pushBack("inner str_item_1");
  test1->obj1->field_list_string->pushBack("inner str_item_2");
  test1->obj1->field_list_string->pushBack("inner str_item_3");
  
  test1->child1 = TestChild::createShared();
  test1->child1->name = "child1_name";
  test1->child1->secondName = "child1_second_name";
  
  test1->field_list_string->pushBack("str_item_1");
  test1->field_list_string->pushBack("str_item_2");
  test1->field_list_string->pushBack("str_item_3");
  
  test1->field_list_int32->pushBack(321);
  test1->field_list_int32->pushBack(322);
  test1->field_list_int32->pushBack(323);
  
  test1->field_list_int64->pushBack(641);
  test1->field_list_int64->pushBack(642);
  test1->field_list_int64->pushBack(643);
  
  test1->field_list_float32->pushBack(0.321f);
  test1->field_list_float32->pushBack(0.322f);
  test1->field_list_float32->pushBack(0.323f);
  
  test1->field_list_float64->pushBack(0.641);
  test1->field_list_float64->pushBack(0.642);
  test1->field_list_float64->pushBack(0.643);
  
  test1->field_list_boolean->pushBack(true);
  test1->field_list_boolean->pushBack(false);
  test1->field_list_boolean->pushBack(true);
  
  test1->field_list_object->pushBack(TestChild::createShared("child", "1"));
  test1->field_list_object->pushBack(TestChild::createShared("child", "2"));
  test1->field_list_object->pushBack(TestChild::createShared("child", "3"));
  
  auto l1 = oatpp::List<TestChild>::createShared();
  auto l2 = oatpp::List<TestChild>::createShared();
  auto l3 = oatpp::List<TestChild>::createShared();
  
  l1->pushBack(TestChild::createShared("list_1", "item_1"));
  l1->pushBack(TestChild::createShared("list_1", "item_2"));
  l1->pushBack(TestChild::createShared("list_1", "item_3"));
  
  l2->pushBack(TestChild::createShared("list_2", "item_1"));
  l2->pushBack(TestChild::createShared("list_2", "item_2"));
  l2->pushBack(TestChild::createShared("list_2", "item_3"));
  
  l3->pushBack(TestChild::createShared("list_3", "item_1"));
  l3->pushBack(TestChild::createShared("list_3", "item_2"));
  l3->pushBack(TestChild::createShared("list_3", "item_3"));
  
  test1->field_list_list_object->pushBack(l1);
  test1->field_list_list_object->pushBack(l2);
  test1->field_list_list_object->pushBack(l3);
  
  auto result = mapper->writeToString(test1);
  
  OATPP_LOGV(TAG, "json='%s'", (const char*) result->getData());
  
  OATPP_LOGV(TAG, "...");
  OATPP_LOGV(TAG, "...");
  OATPP_LOGV(TAG, "...");

  oatpp::parser::Caret caret(result);
  auto obj = mapper->readFromCaret<Test>(caret);
  
  OATPP_ASSERT(obj->field_string);
  OATPP_ASSERT(obj->field_string == test1->field_string);
  
  OATPP_ASSERT(obj->field_int32);
  OATPP_ASSERT(obj->field_int32->getValue() == test1->field_int32->getValue());
  
  OATPP_ASSERT(obj->field_int64);
  OATPP_ASSERT(obj->field_int64->getValue() == test1->field_int64->getValue());
  
  OATPP_ASSERT(obj->field_float32);
  OATPP_ASSERT(obj->field_float32->getValue() == test1->field_float32->getValue());
  
  OATPP_ASSERT(obj->field_float64);
  OATPP_ASSERT(obj->field_float64->getValue() == test1->field_float64->getValue());
  
  OATPP_ASSERT(obj->field_boolean);
  OATPP_ASSERT(obj->field_boolean->getValue() == test1->field_boolean->getValue());
  
  result = mapper->writeToString(obj);
  
  OATPP_LOGV(TAG, "json='%s'", (const char*) result->getData());

  {

    TestAny::ObjectWrapper::__Wrapper objOW1;
    TestAny::__Wrapper objOW2;

    auto obj = TestAny::createShared();
    obj->anyList->pushBack(oatpp::String("Hello Any!!!"));
    obj->anyList->pushBack(oatpp::Int32(32));
    obj->anyList->pushBack(oatpp::Int64(64));
    obj->anyList->pushBack(oatpp::Float64(0.64));
    obj->anyList->pushBack(oatpp::Float64(0.64));
    obj->anyList->pushBack(TestAnyNested::createShared());

    auto map = oatpp::Fields<Any>::createShared();
    map->put("bool-field", oatpp::Boolean(false));

    obj->anyList->pushBack(map);

    auto json = mapper->writeToString(obj);
    OATPP_LOGV(TAG, "any json='%s'", (const char*) json->getData());

    auto deserializedAny = mapper->readFromString<oatpp::Fields<oatpp::Any>>(json);

    auto json2 = mapper->writeToString(deserializedAny);
    OATPP_LOGV(TAG, "any json='%s'", (const char*) json2->getData());

  }

}
  
#include OATPP_CODEGEN_END(DTO)

}}}}}
