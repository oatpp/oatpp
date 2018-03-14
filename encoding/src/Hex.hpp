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

#ifndef oatpp_encoding_Hex_hpp
#define oatpp_encoding_Hex_hpp

#include "../../../oatpp-lib/core/src/data/stream/Stream.hpp"

#include "../../../oatpp-lib/core/src/base/PtrWrapper.hpp"
#include "../../../oatpp-lib/core/src/base/Environment.hpp"

namespace oatpp { namespace encoding {
  
class Hex {
public:
  static const v_char8 A_D[];
  static const v_word16 A_W16[];
public:
 static const char* const ERROR_UNKNOWN_SYMBOL;
public:
  
  static void writeWord16(v_word16 value, p_char8 buffer);
  static void writeWord32(v_word32 value, p_char8 buffer);
  
  static const char* readWord16(p_char8 buffer, v_word16& value);
  static const char* readWord32(p_char8 buffer, v_word32& value);
  
};
  
}}

#endif /* oatpp_encoding_Hex_hpp */
