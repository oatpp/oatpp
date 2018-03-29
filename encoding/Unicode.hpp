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

#ifndef oatpp_encoding_Unicode_hpp
#define oatpp_encoding_Unicode_hpp

#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace encoding {
  
class Unicode {
public:
  static v_int32 getUtf8CharSequenceLength(v_char8 firstByte);
  static v_int32 getUtf8CharSequenceLengthForCode(v_word32 code);
  static v_int32 encodeUtf8Char(p_char8 sequence, v_int32& length); // returns code
  static v_int32 decodeUtf8Char(v_int32 code, p_char8 buffer); // returns length
  static void codeToUtf16SurrogatePair(v_int32 code, v_int16& high, v_int16& low);
  static v_int32 utf16SurrogatePairToCode(v_int16 high, v_int16 low);
};
  
}}

#endif /* oatpp_encoding_Unicode_hpp */
