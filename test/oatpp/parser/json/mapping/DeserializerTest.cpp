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

#include <cmath>

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

class TestDtoEM : public DTO {

  DTO_INIT(TestDtoEM, DTO)

  DTO_FIELD_INFO(test_int8) {
    info->errorMessage = "error test_int8";
  }
  DTO_FIELD(Int8,test_int8);
  DTO_FIELD_INFO(test_int16) {
    info->errorMessage = "error test_int16";
  }
  DTO_FIELD(Int16, test_int16);
  DTO_FIELD_INFO(test_int32) {
    info->errorMessage = "error test_int32";
  }
  DTO_FIELD(Int32, test_int32);
  DTO_FIELD_INFO(test_int64) {
    info->errorMessage = "error test_int64";
  }
  DTO_FIELD(Int64, test_int64);

  DTO_FIELD_INFO(test_uint8) {
    info->errorMessage = "error test_uint8";
  }
  DTO_FIELD(UInt8, test_uint8);
  DTO_FIELD_INFO(test_uint16) {
    info->errorMessage = "error test_uint16";
  }
  DTO_FIELD(UInt16, test_uint16);
  DTO_FIELD_INFO(test_uint32) {
    info->errorMessage = "error test_uint32";
  }
  DTO_FIELD(UInt32, test_uint32);
  DTO_FIELD_INFO(test_uint64) {
    info->errorMessage = "error test_uint64";
  }
  DTO_FIELD(UInt64, test_uint64);

  DTO_FIELD_INFO(test_float32) {
    info->errorMessage = "error test_float32";
  }
  DTO_FIELD(Float32, test_float32);
  DTO_FIELD_INFO(test_float64) {
    info->errorMessage = "error test_float64";
  }
  DTO_FIELD(Float64 , test_float64);
};

#include OATPP_CODEGEN_END(DTO)
  
}
  
void DeserializerTest::onRun(){
  
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  auto obj1 = mapper->readFromString<oatpp::Object<Test1>>("{}");
  
  OATPP_ASSERT(obj1)
  OATPP_ASSERT(!obj1->strF)
  
  obj1 = mapper->readFromString<oatpp::Object<Test1>>("{\"strF\":\"value1\"}");
  
  OATPP_ASSERT(obj1)
  OATPP_ASSERT(obj1->strF)
  OATPP_ASSERT(obj1->strF == "value1")
  
  obj1 = mapper->readFromString<oatpp::Object<Test1>>("{\n\r\t\f\"strF\"\n\r\t\f:\n\r\t\f\"value1\"\n\r\t\f}");
  
  OATPP_ASSERT(obj1)
  OATPP_ASSERT(obj1->strF)
  OATPP_ASSERT(obj1->strF == "value1")
  
  auto obj2 = mapper->readFromString<oatpp::Object<Test2>>("{\"int32F\": null}");
  
  OATPP_ASSERT(obj2)
  OATPP_ASSERT(!obj2->int32F)
  
  obj2 = mapper->readFromString<oatpp::Object<Test2>>("{\"int32F\": 32}");
  
  OATPP_ASSERT(obj2)
  OATPP_ASSERT(obj2->int32F == 32)
  
  obj2 = mapper->readFromString<oatpp::Object<Test2>>("{\"int32F\":    -32}");
  
  OATPP_ASSERT(obj2)
  OATPP_ASSERT(obj2->int32F == -32)
  
  auto obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": null}");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(!obj3->float32F)
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 32}");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(fabsf(obj3->float32F - 32) < std::numeric_limits<float>::epsilon())
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 1.32e1}");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(obj3->float32F)
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 1.32e+1 }");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(obj3->float32F)
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": 1.32e-1 }");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(obj3->float32F)
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": -1.32E-1 }");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(obj3->float32F)
  
  obj3 = mapper->readFromString<oatpp::Object<Test3>>("{\"float32F\": -1.32E1 }");
  
  OATPP_ASSERT(obj3)
  OATPP_ASSERT(obj3->float32F)
  
  auto list = mapper->readFromString<oatpp::List<oatpp::Int32>>("[1, 2, 3]");
  OATPP_ASSERT(list)
  OATPP_ASSERT(list->size() == 3)
  OATPP_ASSERT(list[0] == 1)
  OATPP_ASSERT(list[1] == 2)
  OATPP_ASSERT(list[2] == 3)

  // Empty test

  auto obj4 = mapper->readFromString<oatpp::Object<Test4>>("{\"object\": {}, \"list\": [], \"map\": {}}");
  OATPP_ASSERT(obj4)
  OATPP_ASSERT(obj4->object)
  OATPP_ASSERT(obj4->list)
  OATPP_ASSERT(obj4->list->size() == 0)
  OATPP_ASSERT(obj4->map->size() == 0)

  obj4 = mapper->readFromString<oatpp::Object<Test4>>("{\"object\": {\n\r\t}, \"list\": [\n\r\t], \"map\": {\n\r\t}}");
  OATPP_ASSERT(obj4)
  OATPP_ASSERT(obj4->object)
  OATPP_ASSERT(obj4->list)
  OATPP_ASSERT(obj4->list->size() == 0)
  OATPP_ASSERT(obj4->map->size() == 0)

  OATPP_LOGD(TAG, "Any: String")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":"my_string"})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == String::Class::getType())
    OATPP_ASSERT(dto->any.retrieve<String>() == "my_string")
  }
  OATPP_LOGD(TAG, "Any: Boolean")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":false})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == Boolean::Class::getType())
    OATPP_ASSERT(dto->any.retrieve<Boolean>() == false)
  }
  OATPP_LOGD(TAG, "Any: Negative Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":-1.23456789,"another":1.1})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType())
    OATPP_ASSERT(fabs(dto->any.retrieve<Float64>() - -1.23456789) < std::numeric_limits<double>::epsilon())
  }
  OATPP_LOGD(TAG, "Any: Positive Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":1.23456789,"another":1.1})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType())
    OATPP_ASSERT(fabs(dto->any.retrieve<Float64>() - 1.23456789) < std::numeric_limits<double>::epsilon())
  }
  OATPP_LOGD(TAG, "Any: Negative exponential Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":-1.2345e30,"another":1.1})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType())
    OATPP_ASSERT(fabs(dto->any.retrieve<Float64>() - -1.2345e30) < std::numeric_limits<double>::epsilon())
  }
  OATPP_LOGD(TAG, "Any: Positive exponential Float")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":1.2345e30,"another":1.1})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == Float64::Class::getType())
    OATPP_ASSERT(fabs(dto->any.retrieve<Float64>() - 1.2345e30) < std::numeric_limits<double>::epsilon())
  }
  OATPP_LOGD(TAG, "Any: Unsigned Integer")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":12345678901234567890,"another":1.1})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == UInt64::Class::getType())
    OATPP_ASSERT(dto->any.retrieve<UInt64>() == 12345678901234567890u)
  }
  OATPP_LOGD(TAG, "Any: Signed Integer")
  {
    auto dto = mapper->readFromString<oatpp::Object<AnyDto>>(R"({"any":-1234567890,"another":1.1})");
    OATPP_ASSERT(dto)
    OATPP_ASSERT(dto->any.getStoredType() == Int64::Class::getType())
    OATPP_ASSERT(dto->any.retrieve<Int64>() == -1234567890)
  }

  {
    std::string test_str1 = R"({"test_int8":abc,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str2 = R"({"test_int8":42,"test_int16":abc,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str3 = R"({"test_int8":42,"test_int16":12345,"test_int32":abc,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str4 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":abc,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str5 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":abc,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str6 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":abc,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str7 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":abc,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str8 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":abc,
                                "test_float32":3.14159,"test_float64":2.71828})";
    std::string test_str9 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":abc,"test_float64":2.71828})";
    std::string test_str10 = R"({"test_int8":42,"test_int16":12345,"test_int32":987654,"test_int64":1234567890123,
                                "test_uint8":255,"test_uint16":65535,"test_uint32":4294967295,"test_uint64":18446744073709551615,
                                "test_float32":3.14159,"test_float64":abc})";

    data::mapping::type::DTOWrapper<TestDtoEM> testDtoEM;
    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str1);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_int8")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str2);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_int16")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str3);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_int32")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str4);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_int64")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str5);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_uint8")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str6);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_uint16")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str7);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_uint32")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str8);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_uint64")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str9);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_float32")
    }
    OATPP_ASSERT(testDtoEM == nullptr)

    try {
      testDtoEM = mapper->readFromString<oatpp::Object<TestDtoEM>>(test_str10);
    } catch (oatpp::parser::ParsingError& e) {
      OATPP_ASSERT(e.getMessage() == "error test_float64")
    }
    OATPP_ASSERT(testDtoEM == nullptr)
  }
}
  
}}}}}
