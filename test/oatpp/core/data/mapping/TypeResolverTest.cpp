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

#include "TypeResolverTest.hpp"

#include "oatpp/core/data/mapping/TypeResolver.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class TestDto : public oatpp::DTO {

  DTO_INIT(TestDto, DTO)

  DTO_FIELD(String, f_str);

  DTO_FIELD(Int8, f_int8);
  DTO_FIELD(UInt8, f_uint8);

  DTO_FIELD(Int16, f_int16);
  DTO_FIELD(UInt16, f_uint16);

  DTO_FIELD(Int32, f_int32);
  DTO_FIELD(UInt32, f_uint32);

  DTO_FIELD(Int64, f_int64);
  DTO_FIELD(UInt64, f_uint64);

  DTO_FIELD(Float32, f_float32);
  DTO_FIELD(Float64, f_float64);

  DTO_FIELD(Boolean, f_bool);

  DTO_FIELD(Vector<String>, f_vector);
  DTO_FIELD(List<String>, f_list);
  DTO_FIELD(UnorderedSet<String>, f_set);

  DTO_FIELD(Fields<String>, f_fields);
  DTO_FIELD(UnorderedFields<String>, f_unordered_fields);

  DTO_FIELD(Any, f_any);

  DTO_FIELD(Object<TestDto>, f_dto);

};

#include OATPP_CODEGEN_END(DTO)

}

void TypeResolverTest::onRun() {

  oatpp::data::mapping::TypeResolver::Cache cache;
  oatpp::data::mapping::TypeResolver tr;

  auto dto1 = TestDto::createShared();

  dto1->f_str = "hello dto1";

  dto1->f_int8 = 8;
  dto1->f_uint8 = 88;

  dto1->f_int16 = 16;
  dto1->f_uint16 = 1616;

  dto1->f_int32 = 32;
  dto1->f_uint32 = 3232;

  dto1->f_int64 = 64;
  dto1->f_uint64 = 6464;

  dto1->f_float32 = 0.32f;
  dto1->f_float64 = 0.64;

  dto1->f_bool = true;

  dto1->f_vector = {"hello", "world"};
  dto1->f_list = {"hello", "world"};
  dto1->f_set = {"hello", "world"};

  dto1->f_fields = {{"hello", "world"}};
  dto1->f_unordered_fields = {{"hello", "world"}};

  dto1->f_any = oatpp::String("hey ANY!");

  dto1->f_dto = TestDto::createShared();


  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_str"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::String::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_str"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::String::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_str.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_str"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::String::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_str"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::String::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_str.get());

  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Int8

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_int8"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int8::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_int8"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int8::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_int8.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_int8"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int8::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_int8"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int8::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_int8.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_uint8"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt8::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_uint8"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt8::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_uint8.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_uint8"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt8::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_uint8"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt8::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_uint8.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Int16

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_int16"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int16::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_int16"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int16::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_int16.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_int16"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int16::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_int16"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int16::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_int16.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_uint16"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt16::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_uint16"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt16::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_uint16.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_uint16"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt16::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_uint16"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt16::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_uint16.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Int32

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_int32"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int32::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_int32"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int32::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_int32.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_int32"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int32::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_int32"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int32::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_int32.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_uint32"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt32::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_uint32"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt32::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_uint32.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_uint32"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt32::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_uint32"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt32::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_uint32.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Int64

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_int64"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int64::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_int64"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int64::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_int64.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_int64"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Int64::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_int64"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Int64::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_int64.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_uint64"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt64::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_uint64"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt64::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_uint64.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_uint64"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::UInt64::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_uint64"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::UInt64::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_uint64.get());

  }



  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Float32

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_float32"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Float32::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_float32"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Float32::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_float32.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_float32"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Float32::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_float32"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Float32::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_float32.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Float64

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_float64"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Float64::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_float64"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Float64::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_float64.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_float64"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Float64::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_float64"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Float64::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_float64.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Boolean

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_bool"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Boolean::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_bool"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Boolean::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_bool.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_bool"}, cache);
    OATPP_ASSERT(type != nullptr);
    OATPP_ASSERT(type->classId.id == oatpp::Boolean::Class::CLASS_ID.id);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_bool"}, cache);
    OATPP_ASSERT(val.getValueType()->classId.id == oatpp::Boolean::Class::CLASS_ID.id);
    OATPP_ASSERT(val.get() == dto1->f_dto->f_bool.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vector

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_vector"}, cache);
    OATPP_ASSERT(type == dto1->f_vector.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_vector"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_vector.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_vector.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_vector"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_vector.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_vector"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_vector.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_vector.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // List

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_list"}, cache);
    OATPP_ASSERT(type == dto1->f_list.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_list"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_list.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_list.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_list"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_list.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_list"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_list.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_list.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Set

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_set"}, cache);
    OATPP_ASSERT(type == dto1->f_set.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_set"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_set.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_set.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_set"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_set.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_set"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_set.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_set.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Fields

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_fields"}, cache);
    OATPP_ASSERT(type == dto1->f_fields.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_fields"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_fields.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_fields.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_fields"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_fields.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_fields"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_fields.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_fields.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // UnorderedFields

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_unordered_fields"}, cache);
    OATPP_ASSERT(type == dto1->f_unordered_fields.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_unordered_fields"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_unordered_fields.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_unordered_fields.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_unordered_fields"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_unordered_fields.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_unordered_fields"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_unordered_fields.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_unordered_fields.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Any

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_any"}, cache);
    OATPP_ASSERT(type == dto1->f_any.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_any"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_any.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_any.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_any"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_any.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_any"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_any.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_any.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Dto

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto"}, cache);
    OATPP_ASSERT(type == dto1->f_dto.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto.get());

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_dto"}, cache);
    OATPP_ASSERT(type == dto1->f_dto->f_dto.getValueType());

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_dto"}, cache);
    OATPP_ASSERT(val.getValueType() == dto1->f_dto->f_dto.getValueType());
    OATPP_ASSERT(val.get() == dto1->f_dto->f_dto.get());

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Non-Existing

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_non_existing"}, cache);
    OATPP_ASSERT(type == nullptr);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_non_existing"}, cache);
    OATPP_ASSERT(val.getValueType() == oatpp::Void::Class::getType());
    OATPP_ASSERT(val == nullptr);

  }

  {

    auto type = tr.resolveObjectPropertyType(oatpp::Object<TestDto>::Class::getType(), {"f_dto", "f_non_existing"}, cache);
    OATPP_ASSERT(type == nullptr);

    auto val = tr.resolveObjectPropertyValue(dto1, {"f_dto", "f_non_existing"}, cache);
    OATPP_ASSERT(val.getValueType() == oatpp::Void::Class::getType());
    OATPP_ASSERT(val == nullptr);

  }

}

}}}}}
