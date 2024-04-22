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

#ifndef oatpp_utils_String_hpp
#define oatpp_utils_String_hpp

#include "oatpp/Environment.hpp"

namespace oatpp { namespace utils {

/**
 * Utility class for Strings
 */
class String {
public:

  /**
   * Compare data1, data2 using `std::memcmp`.
   * *It's safe to pass nullptr for data1/data2*
   * @param data1 - pointer to data1.
   * @param size1 - size of data1.
   * @param data2 - pointer to data2.
   * @param size2 - size of data2.
   * @return - Negative value if the first differing byte (reinterpreted as unsigned char) in data1 is less than the corresponding byte in data2.<br>
   * 0 if all count bytes of data1 and data2 are equal.<br>
   * Positive value if the first differing byte in data1 is greater than the corresponding byte in data2.
   */
  static v_buff_size compare(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2);

  /**
   * Compare data1, data2 - case insensitive (ASCII only).
   * *It's safe to pass nullptr for data1/data2*
   * @param data1 - pointer to data1.
   * @param size1 - size of data1.
   * @param data2 - pointer to data2.
   * @param size2 - size of data2.
   * @return - Negative value if the first differing byte (reinterpreted as unsigned char) in data1 is less than the corresponding byte in data2.<br>
   * 0 if all count bytes of data1 and data2 are equal.<br>
   * Positive value if the first differing byte in data1 is greater than the corresponding byte in data2.
   */
  static v_buff_size compareCI_ASCII(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2);

  /**
   * Change characters in data to lowercase (ASCII only).
   * @param data - pointer to data.
   * @param size - size of the data.
   */
  static void lowerCase_ASCII(void* data, v_buff_size size);

  /**
   * Change characters in data to uppercase (ASCII only).
   * @param data - pointer to data.
   * @param size - size of the data.
   */
  static void upperCase_ASCII(void* data, v_buff_size size);

};

}}

#endif // oatpp_utils_String_hpp
