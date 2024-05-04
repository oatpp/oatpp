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

#ifndef oatpp_utils_Conversion_hpp
#define oatpp_utils_Conversion_hpp

#include "oatpp/data/type/Primitive.hpp"
#include "oatpp/Types.hpp"


#include "oatpp/base/Countable.hpp"
#include "oatpp/Environment.hpp"

#include <string>

namespace oatpp { namespace utils {

/**
 * Conversion utils
 */
class Conversion {
public:

  /**
   * String to 32-bit integer.
   * @param str - string as `const char*`.
   * @return - 32-bit integer value.
   */
  static v_int32 strToInt32(const char *str);

  /**
   * String to 32-bit integer.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 32-bit integer value.
   */
  static v_int32 strToInt32(const oatpp::String &str, bool &success);

  /**
   * String to 32-bit unsigned integer.
   * @param str - string as `const char*`.
   * @return - 32-bit unsigned integer value.
   */
  static v_uint32 strToUInt32(const char *str);

  /**
   * String to 32-bit unsigned integer.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 32-bit unsigned integer value.
   */
  static v_uint32 strToUInt32(const oatpp::String &str, bool &success);

  /**
   * String to 64-bit integer.
   * @param str - string as `const char*`.
   * @return - 64-bit integer value.
   */
  static v_int64 strToInt64(const char *str);

  /**
   * String to 64-bit integer.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 64-bit integer value.
   */
  static v_int64 strToInt64(const oatpp::String &str, bool &success);

  /**
   * String to 64-bit unsigned integer.
   * @param str - string as `const char*`.
   * @return - 64-bit unsigned integer value.
   */
  static v_uint64 strToUInt64(const char *str);

  /**
   * String to 64-bit unsigned integer.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 64-bit unsigned integer value.
   */
  static v_uint64 strToUInt64(const oatpp::String &str, bool &success);

  /**
   * Convert 32-bit integer to it's string representation.
   * @param value - 32-bit integer value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @return - length of the resultant string.
   */
  static v_buff_size int32ToCharSequence(v_int32 value, p_char8 data, v_buff_size n);

  /**
   * Convert 32-bit unsigned integer to it's string representation.
   * @param value - 32-bit unsigned integer value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @return - length of the resultant string.
   */
  static v_buff_size uint32ToCharSequence(v_uint32 value, p_char8 data, v_buff_size n);

  /**
   * Convert 64-bit integer to it's string representation.
   * @param value - 64-bit integer value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @return - length of the resultant string.
   */
  static v_buff_size int64ToCharSequence(v_int64 value, p_char8 data, v_buff_size n);

  /**
   * Convert 64-bit unsigned integer to it's string representation.
   * @param value - 64-bit unsigned integer value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @return - length of the resultant string.
   */
  static v_buff_size uint64ToCharSequence(v_uint64 value, p_char8 data, v_buff_size n);

  /**
   * Convert 32-bit integer to it's string representation.
   * @param value - 32-bit integer value.
   * @return - value as `oatpp::String`
   */
  static oatpp::String int32ToStr(v_int32 value);

  /**
   * Convert 32-bit unsigned integer to it's string representation.
   * @param value - 32-bit unsigned integer value.
   * @return - value as `oatpp::String`
   */
  static oatpp::String uint32ToStr(v_uint32 value);

  /**
   * Convert 64-bit integer to it's string representation.
   * @param value - 64-bit integer value.
   * @return - value as `oatpp::String`
   */
  static oatpp::String int64ToStr(v_int64 value);

  /**
   * Convert 64-bit unsigned integer to it's string representation.
   * @param value - 64-bit unsigned integer value.
   * @return - value as `oatpp::String`
   */
  static oatpp::String uint64ToStr(v_uint64 value);

  /**
   * Convert 32-bit integer to it's string representation.
   * @param value - 32-bit integer value.
   * @return - value as `std::string`
   */
  static std::string int32ToStdStr(v_int32 value);

  /**
   * Convert 32-bit unsigned integer to it's string representation.
   * @param value - 32-bit unsigned integer value.
   * @return - value as `std::string`
   */
  static std::string uint32ToStdStr(v_uint32 value);

  /**
   * Convert 64-bit integer to it's string representation.
   * @param value - 64-bit integer value.
   * @return - value as `std::string`
   */
  static std::string int64ToStdStr(v_int64 value);

  /**
   * Convert 64-bit unsigned integer to it's string representation.
   * @param value - 64-bit unsigned integer value.
   * @return - value as `std::string`
   */
  static std::string uint64ToStdStr(v_uint64 value);

  /**
   * Write value of primitive type (int, float, etc.) as it's string representation with pattern.
   * @tparam T - primitive value type (int, float, etc.).
   * @param value - actual value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @param pattern - pattern as for `snprintf`.
   * @return - length of the resultant string.
   */
  template<typename T>
  static v_buff_size primitiveToCharSequence(T value, p_char8 data, v_buff_size n, const char *pattern) {
    return snprintf(reinterpret_cast<char *>(data), static_cast<size_t>(n), pattern, value);
  }

  /**
   * Write value of primitive type (int, float, etc.) as it's string representation with pattern.
   * @tparam T - primitive value type (int, float, etc.).
   * @param value - actual value.
   * @param pattern - pattern as for `sprintf`.
   * @return - length of the resultant string.
   */
  template<typename T>
  static oatpp::String primitiveToStr(T value, const char *pattern) {
    v_char8 buff[100];
    auto size = primitiveToCharSequence(value, &buff[0], 100, pattern);
    if (size > 0) {
      return oatpp::String(reinterpret_cast<const char *>(&buff[0]), size);
    }
    return nullptr;
  }

  /**
   * String to 32-bit float.
   * @param str - string as `const char*`.
   * @return - 32-bit float value.
   */
  static v_float32 strToFloat32(const char *str);

  /**
   * String to 32-bit float.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 32-bit float value.
   */
  static v_float32 strToFloat32(const oatpp::String &str, bool &success);

  /**
   * String to 64-bit float.
   * @param str - string as `const char*`.
   * @return - 64-bit float value.
   */
  static v_float64 strToFloat64(const char *str);

  /**
   * String to 64-bit float.
   * @param str - string as `oatpp::String`.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - 64-bit float value.
   */
  static v_float64 strToFloat64(const oatpp::String &str, bool &success);

  /**
   * Convert 32-bit float to it's string representation.
   * @param value - 32-bit float value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @return - length of the resultant string.
   */
  static v_buff_size float32ToCharSequence(v_float32 value, p_char8 data, v_buff_size n, const char *format = OATPP_FLOAT_STRING_FORMAT);

  /**
   * Convert 64-bit float to it's string representation.
   * @param value - 64-bit float value.
   * @param data - buffer to write data to.
   * @param n - buffer size.
   * @return - length of the resultant string.
   */
  static v_buff_size float64ToCharSequence(v_float64 value, p_char8 data, v_buff_size n, const char *format = OATPP_FLOAT_STRING_FORMAT);

  /**
   * Convert 32-bit float to it's string representation.
   * @param value - 32-bit float value.
   * @return - value as `oatpp::String`
   */
  static oatpp::String float32ToStr(v_float32 value, const char *format = OATPP_FLOAT_STRING_FORMAT);

  /**
   * Convert 64-bit float to it's string representation.
   * @param value - 64-bit float value.
   * @return - value as `oatpp::String`
   */
  static oatpp::String float64ToStr(v_float64 value, const char *format = OATPP_FLOAT_STRING_FORMAT);

  /**
   * Convert boolean to it's string representation.
   * @param value - boolean value.
   * @return - value as `oatpp::String`;
   */
  static oatpp::String boolToStr(bool value);

  /**
   * parse string to boolean value.
   * @param str - string to parse.
   * @param success - out parameter. `true` if operation was successful. `false` otherwise.
   * @return - boolean value.
   */
  static bool strToBool(const oatpp::String &str, bool &success);

};

}}

#endif /* oatpp_utils_Conversion_hpp */
