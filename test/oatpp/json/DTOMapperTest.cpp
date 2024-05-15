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

#include <cmath>

#include "oatpp/json/ObjectMapper.hpp"

#include "oatpp/data/type/Object.hpp"
#include "oatpp/data/type/List.hpp"
#include "oatpp/data/type/Primitive.hpp"

#include "oatpp/utils/Conversion.hpp"

#include "oatpp/macro/codegen.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace json {
  
namespace {
  
#include OATPP_CODEGEN_BEGIN(DTO)
  
class TestChild : public oatpp::DTO {
  
  DTO_INIT(TestChild, DTO)
  
  DTO_FIELD(String, name) = "Name";
  DTO_FIELD(String, secondName) = "Second Name";

public:

  TestChild() = default;

  TestChild(const char* pName, const char* pSecondName)
    : name(pName)
    , secondName(pSecondName)
  {}

};

class Test : public oatpp::DTO {
  
  DTO_INIT(Test, DTO)
  
  DTO_FIELD(String, field_string, "string-field-name-qualifier");
  DTO_FIELD(Int32, field_int32, "int32-field-name-qualifier");
  DTO_FIELD(Int64, field_int64);
  DTO_FIELD(Float32, field_float32);
  DTO_FIELD(Float64, field_float64);
  DTO_FIELD(Boolean, field_boolean);
  
  DTO_FIELD(List<String>, field_list_string) = {};
  DTO_FIELD(List<Int32>, field_list_int32) = {};
  DTO_FIELD(List<Int64>, field_list_int64) = {};
  DTO_FIELD(List<Float32>, field_list_float32) = {};
  DTO_FIELD(List<Float64>, field_list_float64) = {};
  DTO_FIELD(List<Boolean>, field_list_boolean) = {};
  
  DTO_FIELD(List<Object<TestChild>>, field_list_object) = {};
  DTO_FIELD(List<List<Object<TestChild>>>, field_list_list_object) = {};

  DTO_FIELD(Vector<String>, field_vector);
  DTO_FIELD(Fields<String>, field_fields);
  DTO_FIELD(UnorderedFields<String>, field_unordered_fields);
  
  DTO_FIELD(Object<Test>, obj1);
  DTO_FIELD(Object<TestChild>, child1);
  
};

class Test2 : public oatpp::DTO {

  DTO_INIT(Test2, DTO)

  DTO_FIELD_INFO(field_string) {
    info->required = true;
  }
  DTO_FIELD(String, field_string);
};

class Test3 : public oatpp::DTO {
  DTO_INIT(Test3, DTO)

  DTO_FIELD(String, field_string);
};

class TestChild1 : public oatpp::DTO {

  DTO_INIT(TestChild1, DTO)

  DTO_FIELD_INFO(name) {
    info->required = true;
  }
  DTO_FIELD(String, name);
};

class Test4 : public oatpp::DTO {

  DTO_INIT(Test4, DTO)

  DTO_FIELD(String, field_string);

  DTO_FIELD(Object<TestChild1>, child);
};

class TestChild2 : public oatpp::DTO {

  DTO_INIT(TestChild2, DTO)

  DTO_FIELD(String, name);
};

class Test5 : public oatpp::DTO {

  DTO_INIT(Test5, DTO)

  DTO_FIELD(String, field_string);

  DTO_FIELD(Object<TestChild2>, child);
};

class TestAny : public oatpp::DTO {

  DTO_INIT(TestAny, DTO)

  DTO_FIELD(List<Any>, anyList) = List<Any>::createShared();

};

class TestAnyNested : public oatpp::DTO {

  DTO_INIT(TestAnyNested, DTO)

  DTO_FIELD(String, f1) = "Field_1";
  DTO_FIELD(String, f2) = "Field_2";

};
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
void DTOMapperTest::onRun(){

  oatpp::json::ObjectMapper mapper;
  mapper.serializerConfig().json.useBeautifier = true;

  {
    auto test1 = Test::createShared();

    test1->field_string = "string value";
    test1->field_int32 = 32;
    test1->field_int64 = 64;
    test1->field_float32 = 0.32f;
    test1->field_float64 = 0.64;
    test1->field_boolean = true;

    test1->obj1 = Test::createShared();
    test1->obj1->field_string = "inner string";
    test1->obj1->field_list_string->push_back("inner str_item_1");
    test1->obj1->field_list_string->push_back("inner str_item_2");
    test1->obj1->field_list_string->push_back("inner str_item_3");

    test1->child1 = TestChild::createShared();
    test1->child1->name = "child1_name";
    test1->child1->secondName = "child1_second_name";

    test1->field_list_string->push_back("str_item_1");
    test1->field_list_string->push_back("str_item_2");
    test1->field_list_string->push_back("str_item_3");

    test1->field_list_int32->push_back(321);
    test1->field_list_int32->push_back(322);
    test1->field_list_int32->push_back(323);

    test1->field_list_int64->push_back(641);
    test1->field_list_int64->push_back(642);
    test1->field_list_int64->push_back(643);

    test1->field_list_float32->push_back(0.321f);
    test1->field_list_float32->push_back(0.322f);
    test1->field_list_float32->push_back(0.323f);

    test1->field_list_float64->push_back(0.641);
    test1->field_list_float64->push_back(0.642);
    test1->field_list_float64->push_back(0.643);

    test1->field_list_boolean->push_back(true);
    test1->field_list_boolean->push_back(false);
    test1->field_list_boolean->push_back(true);

    test1->field_list_object->push_back(TestChild::createShared("child", "1"));
    test1->field_list_object->push_back(TestChild::createShared("child", "2"));
    test1->field_list_object->push_back(TestChild::createShared("child", "3"));

    auto l1 = oatpp::List<oatpp::Object<TestChild>>::createShared();
    auto l2 = oatpp::List<oatpp::Object<TestChild>>::createShared();
    auto l3 = oatpp::List<oatpp::Object<TestChild>>::createShared();

    l1->push_back(TestChild::createShared("list_1", "item_1"));
    l1->push_back(TestChild::createShared("list_1", "item_2"));
    l1->push_back(TestChild::createShared("list_1", "item_3"));

    l2->push_back(TestChild::createShared("list_2", "item_1"));
    l2->push_back(TestChild::createShared("list_2", "item_2"));
    l2->push_back(TestChild::createShared("list_2", "item_3"));

    l3->push_back(TestChild::createShared("list_3", "item_1"));
    l3->push_back(TestChild::createShared("list_3", "item_2"));
    l3->push_back(TestChild::createShared("list_3", "item_3"));

    test1->field_list_list_object->push_back(l1);
    test1->field_list_list_object->push_back(l2);
    test1->field_list_list_object->push_back(l3);

    test1->field_vector = {"vector_item1", "vector_item2", "vector_item3"};

    test1->field_fields = {
        {"key0", "pair_item0"},
        {"key1", "pair_item1"},
        {"key2", "pair_item2"},
        {"key3", "pair_item3"},
        {"key4", "pair_item4"},
        {"key5", "pair_item5"},
        {"key6", "pair_item6"},
        {"key7", "pair_item7"},
        {"key8", "pair_item8"},
        {"key9", "pair_item9"},
        {"key10", "pair_item10"},
        {"key11", "pair_item11"}
    };

    test1->field_unordered_fields = {
        {"key0", "map_item0"},
        {"key1", "map_item1"},
        {"key2", "map_item2"},
        {"key3", "map_item3"},
        {"key4", "map_item4"},
        {"key5", "map_item5"},
        {"key6", "map_item6"},
        {"key7", "map_item7"},
        {"key8", "map_item8"},
        {"key9", "map_item9"},
        {"key10", "map_item10"},
        {"key11", "map_item11"}
    };

    auto result = mapper.writeToString(test1);

    OATPP_LOGv(TAG, "json='{}'", result->c_str())

    OATPP_LOGv(TAG, "...")
    OATPP_LOGv(TAG, "...")
    OATPP_LOGv(TAG, "...")

    oatpp::utils::parser::Caret caret(result);
    auto obj1 = mapper.readFromCaret<oatpp::Object<Test>>(caret);

    OATPP_ASSERT(obj1->field_string)
    OATPP_ASSERT(obj1->field_string == test1->field_string)

    OATPP_ASSERT(obj1->field_int32)
    OATPP_ASSERT(obj1->field_int32 == test1->field_int32)

    OATPP_ASSERT(obj1->field_int64)
    OATPP_ASSERT(obj1->field_int64 == test1->field_int64)

    OATPP_ASSERT(obj1->field_float32)
    OATPP_ASSERT(fabsf(obj1->field_float32 - test1->field_float32) < std::numeric_limits<float>::epsilon())

    OATPP_ASSERT(obj1->field_float64)
    OATPP_ASSERT(fabs(obj1->field_float64 - test1->field_float64) < std::numeric_limits<double>::epsilon())

    OATPP_ASSERT(obj1->field_boolean)
    OATPP_ASSERT(obj1->field_boolean == test1->field_boolean)

    {
      auto c = obj1->field_vector;
      OATPP_ASSERT(c[0] == "vector_item1")
      OATPP_ASSERT(c[1] == "vector_item2")
      OATPP_ASSERT(c[2] == "vector_item3")
    }

    {
      auto c = obj1->field_fields;
      v_int32 i = 0;
      for(auto& pair : *c) {
        OATPP_ASSERT(pair.first == "key" + oatpp::utils::Conversion::int32ToStr(i))
        OATPP_ASSERT(pair.second == "pair_item" + oatpp::utils::Conversion::int32ToStr(i))
        i++;
      }
    }

    {
      auto c = obj1->field_unordered_fields;
      OATPP_ASSERT(c["key1"] == "map_item1")
      OATPP_ASSERT(c["key2"] == "map_item2")
      OATPP_ASSERT(c["key3"] == "map_item3")
    }

    result = mapper.writeToString(obj1);

    OATPP_LOGv(TAG, "json='{}'", result->c_str())
  }

  {
    auto test2 = Test2::createShared();
    oatpp::String result;
    try {
      result = mapper.writeToString(test2);
    } catch(std::runtime_error&) {
      OATPP_LOGv(TAG, "Test2::field_string is required!")
    }
    OATPP_ASSERT(result == nullptr)
  }

  {
    auto test3 = Test3::createShared();
    try {
      auto result = mapper.writeToString(test3);
    } catch(std::runtime_error&) {
      OATPP_ASSERT(false)
    }
  }

  {
    auto test4 = Test4::createShared();
    test4->field_string = "string value";
    test4->child = TestChild1::createShared();
    oatpp::String result;
    try {
      result = mapper.writeToString(test4);
    } catch(std::runtime_error&) {
      OATPP_LOGv(TAG, "TestChild1::name is required!")
    }
    OATPP_ASSERT(result == nullptr)
  }

  {
    auto test5 = Test5::createShared();
    test5->field_string = "string value";
    test5->child = TestChild2::createShared();
    try {
      auto result = mapper.writeToString(test5);
    } catch(std::runtime_error&) {
      OATPP_ASSERT(false)
    }
  }

  {

    auto obj2 = TestAny::createShared();
    obj2->anyList = {
      oatpp::String("Hello Any!!!"),
      oatpp::Int32(32),
      oatpp::Int64(64),
      oatpp::Float64(0.64),
      oatpp::Float64(0.64),
      TestAnyNested::createShared()
    };

    auto map = oatpp::Fields<Any>::createShared();
    map["bool-field"] = oatpp::Boolean(false);
    map["vector"] = oatpp::Vector<oatpp::String>({"vector_v1", "vector_v2", "vector_v3"});
    map["unordered_map"] = oatpp::UnorderedFields<oatpp::String>({{"key1", "value1"}, {"key2", "value2"}});

    obj2->anyList->push_back(map);

    auto json = mapper.writeToString(obj2);
    OATPP_LOGv(TAG, "any json='{}'", json->c_str())

    auto deserializedAny = mapper.readFromString<oatpp::Fields<oatpp::Any>>(json);

    auto json2 = mapper.writeToString(deserializedAny);
    OATPP_LOGv(TAG, "any json='{}'", json2->c_str())

  }

}
  
#include OATPP_CODEGEN_END(DTO)

}}
