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

#ifndef oatpp_encoding_Unicode_hpp
#define oatpp_encoding_Unicode_hpp

#include "oatpp/Environment.hpp"

namespace oatpp { namespace encoding {

/**
 * Helper class for processing unicode characters.
 */
class Unicode {
public:
  /**
   * Get length in bytes of UTF-8 character by its first byte.
   * @param firstByte - first byte of UTF-8 character.
   * @return - length in bytes of UTF-8 character.
   */
  static v_buff_size getUtf8CharSequenceLength(v_char8 firstByte);

  /**
   * Get length in bytes of UTF-8 character by its code.
   * @param code - code of UTF-8 character.
   * @return - length in bytes of UTF-8 character.
   */
  static v_buff_size getUtf8CharSequenceLengthForCode(v_uint32 code);

  /**
   * Get code of UTF-8 character.
   * @param sequence - pointer to first byte of UTF-8 character.
   * @param length - out parameter. Length in bytes of UTF-8 character.
   * @return - code of UTF-8 character.
   */
  static v_int32 encodeUtf8Char(const char* sequence, v_buff_size& length);

  /**
   * Write UTF-8 character to buffer.
   * @param code  - UTF-8 character code.
   * @param buffer - pointer to write UTF-8 character to.
   * @return - length in bytes of UTF-8 character.
   */
  static v_buff_size decodeUtf8Char(v_int32 code, p_char8 buffer);

  /**
   * Get corresponding UTF-16 surrogate pair for symbol code.
   * @param code - symbol code.
   * @param high - out parameter. High surrogate.
   * @param low - out parameter. Low surrogate.
   */
  static void codeToUtf16SurrogatePair(v_int32 code, v_int16& high, v_int16& low);

  /**
   * Get symbol code of corresponding UTF-16 surrogate pair.
   * @param high - High surrogate.
   * @param low - Low surrogate.
   * @return - symbol code.
   */
  static v_int32 utf16SurrogatePairToCode(v_int16 high, v_int16 low);
};
  
}}

#endif /* oatpp_encoding_Unicode_hpp */
