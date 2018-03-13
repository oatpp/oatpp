/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "../../../src/json/mapping/ObjectMapper.hpp"

#include "../../../../../oatpp-lib/core/src/data/mapping/type/Object.hpp"
#include "../../../../../oatpp-lib/core/src/data/mapping/type/List.hpp"
#include "../../../../../oatpp-lib/core/src/data/mapping/type/Primitive.hpp"

#include "../../../../../oatpp-lib/core/src/macro/codegen.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {
  
namespace {
  
#include OATPP_CODEGEN_BEGIN(DTO)
  
typedef oatpp::data::mapping::type::Object DTO;
  
class TestChild : public DTO {
  
  DTO_INIT(TestChild, DTO)
  
  static SharedWrapper createShared(const char* name, const char* secondName){
    auto result = createShared();
    result->name = name;
    result->secondName = secondName;
    return result;
  }
  
  DTO_FIELD(String, name) = "Name";
  DTO_FIELD(String, secondName) = "Second Name";
  
};

class Test : public DTO {
  
  DTO_INIT(Test, DTO)
  
  DTO_FIELD(String, _string);
  DTO_FIELD(Int32, _int32);
  DTO_FIELD(Int64, _int64);
  DTO_FIELD(Float32, _float32);
  DTO_FIELD(Float64, _float64);
  DTO_FIELD(Boolean, _boolean);
  
  DTO_FIELD(List<String>::SharedWrapper, _list_string) = List<String>::createShared();
  DTO_FIELD(List<Int32>::SharedWrapper, _list_int32) = List<Int32>::createShared();
  DTO_FIELD(List<Int64>::SharedWrapper, _list_int64) = List<Int64>::createShared();
  DTO_FIELD(List<Float32>::SharedWrapper, _list_float32) = List<Float32>::createShared();
  DTO_FIELD(List<Float64>::SharedWrapper, _list_float64) = List<Float64>::createShared();
  DTO_FIELD(List<Boolean>::SharedWrapper, _list_boolean) = List<Boolean>::createShared();
  
  DTO_FIELD(List<TestChild::SharedWrapper>::SharedWrapper, _list_object) = List<TestChild::SharedWrapper>::createShared();
  DTO_FIELD(List<List<TestChild::SharedWrapper>::SharedWrapper>::SharedWrapper, _list_list_object) = List<List<TestChild::SharedWrapper>::SharedWrapper>::createShared();
  
  DTO_FIELD(Test::SharedWrapper, obj1);
  DTO_FIELD(TestChild::SharedWrapper, child1);
  
};
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
bool DTOMapperTest::onRun(){
  
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  Test::SharedWrapper test1 = Test::createShared();
  
  test1->_string = "string value";
  test1->_int32 = 32;
  test1->_int64 = 64;
  test1->_float32 = 0.32;
  test1->_float64 = 0.64;
  test1->_boolean = true;
  
  test1->obj1 = Test::createShared();
  test1->obj1->_string = "inner string";
  test1->obj1->_list_string->pushBack("inner str_item_1");
  test1->obj1->_list_string->pushBack("inner str_item_2");
  test1->obj1->_list_string->pushBack("inner str_item_3");
  
  test1->child1 = TestChild::createShared();
  test1->child1->name = "child1_name";
  test1->child1->secondName = "child1_second_name";
  
  test1->_list_string->pushBack("str_item_1");
  test1->_list_string->pushBack("str_item_2");
  test1->_list_string->pushBack("str_item_3");
  
  test1->_list_int32->pushBack(321);
  test1->_list_int32->pushBack(322);
  test1->_list_int32->pushBack(323);
  
  test1->_list_int64->pushBack(641);
  test1->_list_int64->pushBack(642);
  test1->_list_int64->pushBack(643);
  
  test1->_list_float32->pushBack(0.321);
  test1->_list_float32->pushBack(0.322);
  test1->_list_float32->pushBack(0.323);
  
  test1->_list_float64->pushBack(0.641);
  test1->_list_float64->pushBack(0.642);
  test1->_list_float64->pushBack(0.643);
  
  test1->_list_boolean->pushBack(true);
  test1->_list_boolean->pushBack(false);
  test1->_list_boolean->pushBack(true);
  
  test1->_list_object->pushBack(TestChild::createShared("child", "1"));
  test1->_list_object->pushBack(TestChild::createShared("child", "2"));
  test1->_list_object->pushBack(TestChild::createShared("child", "3"));
  
  auto l1 = DTO::List<TestChild::SharedWrapper>::createShared();
  auto l2 = DTO::List<TestChild::SharedWrapper>::createShared();
  auto l3 = DTO::List<TestChild::SharedWrapper>::createShared();
  
  l1->pushBack(TestChild::createShared("list_1", "item_1"));
  l1->pushBack(TestChild::createShared("list_1", "item_2"));
  l1->pushBack(TestChild::createShared("list_1", "item_3"));
  
  l2->pushBack(TestChild::createShared("list_2", "item_1"));
  l2->pushBack(TestChild::createShared("list_2", "item_2"));
  l2->pushBack(TestChild::createShared("list_2", "item_3"));
  
  l3->pushBack(TestChild::createShared("list_3", "item_1"));
  l3->pushBack(TestChild::createShared("list_3", "item_2"));
  l3->pushBack(TestChild::createShared("list_3", "item_3"));
  
  test1->_list_list_object->pushBack(l1);
  test1->_list_list_object->pushBack(l2);
  test1->_list_list_object->pushBack(l3);
  
  auto result = mapper->writeToString(test1);
  
  OATPP_LOGD(TAG, "json='%s'", (const char*) result->getData());
  
  OATPP_LOGD(TAG, "...");
  OATPP_LOGD(TAG, "...");
  OATPP_LOGD(TAG, "...");
  
  auto config = oatpp::parser::json::mapping::Deserializer::Config::createShared();
  auto caret = oatpp::parser::ParsingCaret::createShared(result);
  auto obj = mapper->readFromCaret<Test>(caret);
  
  OATPP_ASSERT(obj->_string.isNull() == false);
  OATPP_ASSERT(obj->_string->equals(test1->_string));
  
  OATPP_ASSERT(obj->_int32.isNull() == false);
  OATPP_ASSERT(obj->_int32->getValue() == test1->_int32->getValue());
  
  OATPP_ASSERT(obj->_int64.isNull() == false);
  OATPP_ASSERT(obj->_int64->getValue() == test1->_int64->getValue());
  
  OATPP_ASSERT(obj->_float32.isNull() == false);
  OATPP_ASSERT(obj->_float32->getValue() == test1->_float32->getValue());
  
  OATPP_ASSERT(obj->_float64.isNull() == false);
  OATPP_ASSERT(obj->_float64->getValue() == test1->_float64->getValue());
  
  OATPP_ASSERT(obj->_boolean.isNull() == false);
  OATPP_ASSERT(obj->_boolean->getValue() == test1->_boolean->getValue());
  
  result = mapper->writeToString(obj);
  
  OATPP_LOGD(TAG, "json='%s'", (const char*) result->getData());
  
  return true;
}
  
#include OATPP_CODEGEN_END(DTO)

}}}}}
