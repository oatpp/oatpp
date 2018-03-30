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

#include "./Primitive.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

StringPtrWrapper::StringPtrWrapper(const std::shared_ptr<oatpp::base::String>& ptr, const type::Type* const valueType)
  : oatpp::data::mapping::type::PtrWrapper<oatpp::base::String, __class::String>(ptr)
{
  if(type::__class::String::getType() != valueType) {
    throw std::runtime_error("Value type does not match");
  }
}
  
StringPtrWrapper::operator std::string() const {
  if(m_ptr){
    return m_ptr->std_str();
  }
  return "";
}
  
namespace __class {
  
  type::Int32::PtrWrapper Int32::parseFromString(const oatpp::base::String::PtrWrapper& str, bool& success) {
    return utils::conversion::strToInt32(str, success);
  }
  
  type::Int64::PtrWrapper Int64::parseFromString(const oatpp::base::String::PtrWrapper& str, bool& success) {
    return utils::conversion::strToInt64(str, success);
  }
  
  type::Float32::PtrWrapper Float32::parseFromString(const oatpp::base::String::PtrWrapper& str, bool& success) {
    return utils::conversion::strToFloat32(str, success);
  }
  
  type::Float64::PtrWrapper Float64::parseFromString(const oatpp::base::String::PtrWrapper& str, bool& success) {
    return utils::conversion::strToFloat64(str, success);
  }
  
  type::Boolean::PtrWrapper Boolean::parseFromString(const oatpp::base::String::PtrWrapper& str, bool& success) {
    return utils::conversion::strToBool(str, success);
  }
  
}
  
}}}}
