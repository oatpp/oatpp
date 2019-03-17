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

#ifndef oatpp_utils_ConversionUtils_hpp
#define oatpp_utils_ConversionUtils_hpp

#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/Types.hpp"


#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <string>

namespace oatpp { namespace utils { namespace conversion {

  /**
   * String to 32-bit integer.
   * @param str - string as `const char*`.
   * @return - 32-bit integer value.
   */
  v_int32 strToInt32(const char* str);

  /**
   * String to 32-bit integer.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 32-bit integer value.
   */
  v_int32 strToInt32(const oatpp::String& str, bool& success);

  /**
   * String to 64-bit integer.
   * @param str - string as `const char*`.
   * @return - 64-bit integer value.
   */
  v_int64 strToInt64(const char* str);

  /**
   * String to 64-bit integer.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 64-bit integer value.
   */
  v_int64 strToInt64(const oatpp::String& str, bool& success);

  /**
   * Convert 32-bit integer to it's string representation.
   * @param value - 32-bit integer value.
   * @param data - buffer to write data to.
   * @return - length of the resultant string.
   */
  v_int32 int32ToCharSequence(v_int32 value, p_char8 data);

 /**
  * Convert 64-bit integer to it's string representation.
  * @param value - 64-bit integer value.
  * @param data - buffer to write data to.
  * @return - length of the resultant string.
  */
  v_int32 int64ToCharSequence(v_int64 value, p_char8 data);

  /**
   * Convert 32-bit integer to it's string representation.
   * @param value - 32-bit integer value.
   * @return - value as `oatpp::String`
   */
  oatpp::String int32ToStr(v_int32 value);

  /**
   * Convert 64-bit integer to it's string representation.
   * @param value - 64-bit integer value.
   * @return - value as `oatpp::String`
   */
  oatpp::String int64ToStr(v_int64 value);

  /**
   * Convert 32-bit integer to it's string representation.
   * @param value - 32-bit integer value.
   * @return - value as `std::string`
   */
  std::string int32ToStdStr(v_int32 value);

  /**
   * Convert 64-bit integer to it's string representation.
   * @param value - 64-bit integer value.
   * @return - value as `std::string`
   */
  std::string int64ToStdStr(v_int64 value);

  /**
   * Write value of primitive type (int, float, etc.) as it's string representation with pattern.
   * @tparam T - primitive value type (int, float, etc.).
   * @param value - actual value.
   * @param data - buffer to write data to.
   * @param pattern - pattern as for `sprintf`.
   * @return - length of the resultant string.
   */
  template<typename T>
  v_int32 primitiveToCharSequence(T value, p_char8 data, const char* pattern){
    return sprintf((char*)data, pattern, value);
  }

  /**
   * Write value of primitive type (int, float, etc.) as it's string representation with pattern.
   * @tparam T - primitive value type (int, float, etc.).
   * @param value - actual value.
   * @param pattern - pattern as for `sprintf`.
   * @return - length of the resultant string.
   */
  template<typename T>
  oatpp::String primitiveToStr(T value, const char* pattern){
    v_char8 buff [100];
    v_int32 size = primitiveToCharSequence(value, &buff[0], pattern);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size, true);
    }
    return oatpp::String::empty();
  }

  /**
   * String to 32-bit float.
   * @param str - string as `const char*`.
   * @return - 32-bit float value.
   */
  v_float32 strToFloat32(const char* str);

  /**
   * String to 32-bit float.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 32-bit float value.
   */
  v_float32 strToFloat32(const oatpp::String& str, bool& success);

  /**
   * String to 64-bit float.
   * @param str - string as `const char*`.
   * @return - 64-bit float value.
   */
  v_float64 strToFloat64(const char* str);

  /**
   * String to 64-bit float.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 64-bit float value.
   */
  v_float64 strToFloat64(const oatpp::String& str, bool& success);

  /**
   * Convert 32-bit float to it's string representation.
   * @param value - 32-bit float value.
   * @param data - buffer to write data to.
   * @return - length of the resultant string.
   */
  v_int32 float32ToCharSequence(v_float32 value, p_char8 data);

  /**
   * Convert 64-bit float to it's string representation.
   * @param value - 64-bit float value.
   * @param data - buffer to write data to.
   * @return - length of the resultant string.
   */
  v_int32 float64ToCharSequence(v_float64 value, p_char8 data);

  /**
   * Convert 32-bit float to it's string representation.
   * @param value - 32-bit float value.
   * @return - value as `oatpp::String`
   */
  oatpp::String float32ToStr(v_float32 value);

  /**
   * Convert 64-bit float to it's string representation.
   * @param value - 64-bit float value.
   * @return - value as `oatpp::String`
   */
  oatpp::String float64ToStr(v_float64 value);

  /**
   * Convert boolean to it's string representation.
   * @param value - boolean value.
   * @return - value as `oatpp::String`;
   */
  oatpp::String boolToStr(bool value);

  /**
   * parse string to boolean value.
   * @param str - string to parse.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - boolean value.
   */
  bool strToBool(const oatpp::String& str, bool& success);

  /**
   * Write value of oatpp-primitive type (oatpp::Int32, oatpp::Int64, oatpp::Boolean, etc.) as it's string representation.
   * @tparam T - oatpp-primitive type (oatpp::Int32, oatpp::Int64, oatpp::Boolean, etc.).
   * @param primitive - ObjectWrapper.
   * @return - value as string.
   */
  template<class T>
  oatpp::String
  primitiveToStr(const oatpp::data::mapping::type::PolymorphicWrapper<T>& primitive) {
    auto type = primitive.valueType;
    if(type == oatpp::data::mapping::type::__class::String::getType()) {
      return std::static_pointer_cast<oatpp::base::StrBuffer>(primitive.getPtr());
    } else if(type == oatpp::data::mapping::type::__class::Int32::getType()) {
      return utils::conversion::int32ToStr(static_cast<oatpp::data::mapping::type::Int32::ObjectType*>(primitive.get())->getValue());
    } else if(type == oatpp::data::mapping::type::__class::Int64::getType()) {
      return utils::conversion::int64ToStr(static_cast<oatpp::data::mapping::type::Int64::ObjectType*>(primitive.get())->getValue());
    } else if(type == oatpp::data::mapping::type::__class::Float32::getType()) {
      return utils::conversion::float32ToStr(static_cast<oatpp::data::mapping::type::Float32::ObjectType*>(primitive.get())->getValue());
    } else if(type == oatpp::data::mapping::type::__class::Float64::getType()) {
      return utils::conversion::float64ToStr(static_cast<oatpp::data::mapping::type::Float64::ObjectType*>(primitive.get())->getValue());
    } else if(type == oatpp::data::mapping::type::__class::Boolean::getType()) {
      return utils::conversion::boolToStr(static_cast<oatpp::data::mapping::type::Boolean::ObjectType*>(primitive.get())->getValue());
    }
    throw std::runtime_error("[oatpp::utils::conversion::primitiveToStr]: Invalid primitive type");
  }
  
}}}

#endif /* oatpp_utils_ConversionUtils_hpp */
