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

#ifndef oatpp_encoding_Hex_hpp
#define oatpp_encoding_Hex_hpp

#include "oatpp/core/data/stream/Stream.hpp"


#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace encoding {

/**
 * Utility class for hex string encoding/decoding .
 */
class Hex {
public:
  static const v_char8 A_D[];
  static const v_uint16 A_W16[];
public:
  /**
   * Unknown symbol error.
   */
  static constexpr v_int32 ERROR_UNKNOWN_SYMBOL = 1;
public:

  /**
   * Write value as hex string to buffer.
   * @param value - value to write.
   * @param buffer - buffer for resultant string.
   */
  static void writeUInt16(v_uint16 value, p_char8 buffer);

  /**
   * Write value as hex string to buffer.
   * @param value - value to write.
   * @param buffer - buffer for resultant string.
   */
  static void writeUInt32(v_uint32 value, p_char8 buffer);

  /**
   * Parse 4-char hex string to int16.
   * @param buffer - buffer containing string to parse.
   * @param value - out parameter. Resultant value.
   * @return - 0 on success. Negative value on failure.
   */
  static v_int32 readUInt16(p_char8 buffer, v_uint16& value);

  /**
   * Parse 8-char hex string to int32.
   * @param buffer - buffer containing string to parse.
   * @param value - out parameter. Resultant value.
   * @return - 0 on success. Negative value on failure.
   */
  static v_int32 readUInt32(p_char8 buffer, v_uint32& value);
  
};
  
}}

#endif /* oatpp_encoding_Hex_hpp */
