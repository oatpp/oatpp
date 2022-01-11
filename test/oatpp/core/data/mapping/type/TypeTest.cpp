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

#include "TypeTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {
  
namespace {
  
#include OATPP_CODEGEN_BEGIN(DTO)
  
  class TestDto : public oatpp::DTO {
    
    DTO_INIT(TestDto, DTO);

    DTO_FIELD(String, field_string);
    DTO_FIELD(Int8, field_int8);
    DTO_FIELD(Int16, field_int16);
    DTO_FIELD(Int32, field_int32);
    DTO_FIELD(Int64, field_int64);
    DTO_FIELD(Float32, field_float32);
    DTO_FIELD(Float64, field_float64);
    DTO_FIELD(Boolean, field_boolean);
    
    DTO_FIELD(List<String>, field_list_string);
    DTO_FIELD(List<Int32>, field_list_int32);
    DTO_FIELD(List<Int64>, field_list_int64);
    DTO_FIELD(List<Float32>, field_list_float32);
    DTO_FIELD(List<Float64>, field_list_float64);
    DTO_FIELD(List<Boolean>, field_list_boolean);
    
    DTO_FIELD(Fields<String>, field_map_string_string);
    
    DTO_FIELD(Object<TestDto>, obj1);
    
  };
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
void TypeTest::onRun() {
  
  auto obj = TestDto::createShared();
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_string.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_string.getValueType()->classId == oatpp::data::mapping::type::__class::String::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_int8.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_int8.getValueType()->classId == oatpp::data::mapping::type::__class::Int8::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_int16.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_int16.getValueType()->classId == oatpp::data::mapping::type::__class::Int16::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_int32.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_int32.getValueType()->classId == oatpp::data::mapping::type::__class::Int32::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_int64.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_int64.getValueType()->classId == oatpp::data::mapping::type::__class::Int64::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_float32.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_float32.getValueType()->classId == oatpp::data::mapping::type::__class::Float32::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_float64.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_float64.getValueType()->classId == oatpp::data::mapping::type::__class::Float64::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_boolean.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_boolean.getValueType()->classId == oatpp::data::mapping::type::__class::Boolean::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_list_string.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_list_string.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_list_int32.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_list_int32.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_list_int64.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_list_int64.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_list_float32.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_list_float32.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_list_float64.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_list_float64.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_list_boolean.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_list_boolean.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->field_map_string_string.getValueType()->classId.name);
  OATPP_ASSERT(obj->field_map_string_string.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractPairList::CLASS_ID);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->obj1.getValueType()->classId.name);
  OATPP_ASSERT(obj->obj1.getValueType()->classId == oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID);

  OATPP_ASSERT(oatpp::String::Class::getType()->extends(oatpp::String::Class::getType()))
  OATPP_ASSERT(oatpp::Int32::Class::getType()->extends(oatpp::Int32::Class::getType()))
  OATPP_ASSERT(!oatpp::String::Class::getType()->extends(oatpp::Int32::Class::getType()))

}
  
}}}}}}
