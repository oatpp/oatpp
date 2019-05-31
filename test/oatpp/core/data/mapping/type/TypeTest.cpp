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

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {
  
namespace {
  
#include OATPP_CODEGEN_BEGIN(DTO)
  
  typedef oatpp::data::mapping::type::Object DTO;
  
  class TestDto : public DTO {
    
    DTO_INIT(TestDto, DTO)
    
    DTO_FIELD(String, _string);
    DTO_FIELD(Int8, _int8);
    DTO_FIELD(Int16, _int16);
    DTO_FIELD(Int32, _int32);
    DTO_FIELD(Int64, _int64);
    DTO_FIELD(Float32, _float32);
    DTO_FIELD(Float64, _float64);
    DTO_FIELD(Boolean, _boolean);
    
    DTO_FIELD(List<String>::ObjectWrapper, _list_string);
    DTO_FIELD(List<Int32>::ObjectWrapper, _list_int32);
    DTO_FIELD(List<Int64>::ObjectWrapper, _list_int64);
    DTO_FIELD(List<Float32>::ObjectWrapper, _list_float32);
    DTO_FIELD(List<Float64>::ObjectWrapper, _list_float64);
    DTO_FIELD(List<Boolean>::ObjectWrapper, _list_boolean);
    
    DTO_FIELD(Fields<String>::ObjectWrapper, _map_string_string);
    
    DTO_FIELD(TestDto::ObjectWrapper, obj1);
    
  };
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
void TypeTest::onRun() {
  
  auto obj = TestDto::createShared();
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_string.valueType->name);
  OATPP_ASSERT(obj->_string.valueType->name == oatpp::data::mapping::type::__class::String::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_int8.valueType->name);
  OATPP_ASSERT(obj->_int8.valueType->name == oatpp::data::mapping::type::__class::Int8::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_int16.valueType->name);
  OATPP_ASSERT(obj->_int16.valueType->name == oatpp::data::mapping::type::__class::Int16::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_int32.valueType->name);
  OATPP_ASSERT(obj->_int32.valueType->name == oatpp::data::mapping::type::__class::Int32::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_int64.valueType->name);
  OATPP_ASSERT(obj->_int64.valueType->name == oatpp::data::mapping::type::__class::Int64::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_float32.valueType->name);
  OATPP_ASSERT(obj->_float32.valueType->name == oatpp::data::mapping::type::__class::Float32::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_float64.valueType->name);
  OATPP_ASSERT(obj->_float64.valueType->name == oatpp::data::mapping::type::__class::Float64::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_boolean.valueType->name);
  OATPP_ASSERT(obj->_boolean.valueType->name == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_list_string.valueType->name);
  OATPP_ASSERT(obj->_list_string.valueType->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_list_int32.valueType->name);
  OATPP_ASSERT(obj->_list_int32.valueType->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_list_int64.valueType->name);
  OATPP_ASSERT(obj->_list_int64.valueType->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_list_float32.valueType->name);
  OATPP_ASSERT(obj->_list_float32.valueType->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_list_float64.valueType->name);
  OATPP_ASSERT(obj->_list_float64.valueType->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_list_boolean.valueType->name);
  OATPP_ASSERT(obj->_list_boolean.valueType->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->_map_string_string.valueType->name);
  OATPP_ASSERT(obj->_map_string_string.valueType->name == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME);
  
  OATPP_LOGV(TAG, "type: '%s'", obj->obj1.valueType->name);
  OATPP_ASSERT(obj->obj1.valueType->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME);

}
  
}}}}}}
