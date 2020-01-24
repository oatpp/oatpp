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

#include "./Primitive.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

String::String(const std::shared_ptr<oatpp::base::StrBuffer>& ptr, const type::Type* const valueType)
  : oatpp::data::mapping::type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(ptr)
{
  if(type::__class::String::getType() != valueType) {
    throw std::runtime_error("Value type does not match");
  }
}
  
String operator + (const char* a, const String& b) {
  return oatpp::base::StrBuffer::createSharedConcatenated(a, (v_int32) std::strlen(a), b->getData(), b->getSize());
}

String operator + (const String& b, const char* a) {
  return oatpp::base::StrBuffer::createSharedConcatenated(b->getData(), b->getSize(), a, (v_int32) std::strlen(a));
}

String operator + (const String& a, const String& b) {
  return oatpp::base::StrBuffer::createSharedConcatenated(a->getData(), a->getSize(), b->getData(), b->getSize());
}
  
namespace __class {
  
  const ClassId String::CLASS_ID("String");

  const ClassId Int8::CLASS_ID("Int8");
  const ClassId UInt8::CLASS_ID("UInt8");

  const ClassId Int16::CLASS_ID("Int16");
  const ClassId UInt16::CLASS_ID("UInt16");

  const ClassId Int32::CLASS_ID("Int32");
  const ClassId UInt32::CLASS_ID("UInt32");

  const ClassId Int64::CLASS_ID("Int64");
  const ClassId UInt64::CLASS_ID("UInt64");

  const ClassId Float32::CLASS_ID("Float32");
  const ClassId Float64::CLASS_ID("Float64");

  const ClassId Boolean::CLASS_ID("Boolean");

}
  
}}}}
