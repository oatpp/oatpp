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

#include "UnicodeTest.hpp"

#include "oatpp/encoding/Hex.hpp"
#include "oatpp/encoding/Unicode.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace test { namespace encoding {
  
namespace {

void writeBinaryInt(v_int32 value){
  
  v_char8 buff [37];
  buff[36] = '\0';
  v_int32 index = 0;
  for(v_int32 i = 0; i < 36; i++){
    if((i + 1) % 9 == 0){
      buff[i] = ',';
    } else {
      v_int32 unit = 1 << index;
      if((unit & value) == 0){
        buff[i] = '0';
      } else {
        buff[i] = '1';
      }
      index++;
    }
  }
  
  OATPP_LOGv("bin", "value='{}'", reinterpret_cast<const char*>(&buff))
  
}
  
}

// 38327
  
void UnicodeTest::onRun(){
  
  v_char8 buff[128];
  v_buff_size cnt;

  // 2 byte test
  
  for(v_int32 c = 128; c < 2048; c ++){
    auto size = oatpp::encoding::Unicode::decodeUtf8Char(c, buff);
    OATPP_ASSERT(size == 2)
    auto code = oatpp::encoding::Unicode::encodeUtf8Char(reinterpret_cast<const char*>(buff), cnt);
    OATPP_ASSERT(cnt == 2)
    OATPP_ASSERT(code == c)
  }
  
  // 3 byte test
  
  for(v_int32 c = 2048; c < 65536; c ++){
    auto size = oatpp::encoding::Unicode::decodeUtf8Char(c, buff);
    OATPP_ASSERT(size == 3)
    auto code = oatpp::encoding::Unicode::encodeUtf8Char(reinterpret_cast<const char*>(buff), cnt);
    OATPP_ASSERT(cnt == 3)
    OATPP_ASSERT(code == c)
  }
  
  // 4 byte test
  
  for(v_int32 c = 65536; c < 2097152; c ++){
    auto size = oatpp::encoding::Unicode::decodeUtf8Char(c, buff);
    OATPP_ASSERT(size == 4)
    auto code = oatpp::encoding::Unicode::encodeUtf8Char(reinterpret_cast<const char*>(buff), cnt);
    OATPP_ASSERT(cnt == 4)
    OATPP_ASSERT(code == c)
  }
  
  // 5 byte test
  
  for(v_int32 c = 2097152; c < 67108864; c ++){
    auto size = oatpp::encoding::Unicode::decodeUtf8Char(c, buff);
    OATPP_ASSERT(size == 5)
    auto code = oatpp::encoding::Unicode::encodeUtf8Char(reinterpret_cast<const char*>(buff), cnt);
    OATPP_ASSERT(cnt == 5)
    OATPP_ASSERT(code == c)
  }
  
  // 6 byte test

  for (v_int64 c = 67108864; c < 2147483647; c = c + 100) {
    auto size = oatpp::encoding::Unicode::decodeUtf8Char(static_cast<v_int32>(c), buff);
    OATPP_ASSERT(size == 6)
    auto code = oatpp::encoding::Unicode::encodeUtf8Char(reinterpret_cast<const char*>(buff), cnt);
    OATPP_ASSERT(cnt == 6)
    OATPP_ASSERT(code == c)
  }

  // */
  
  const char* sequence = "𐐷";
  auto code = oatpp::encoding::Unicode::encodeUtf8Char(sequence, cnt);
  
  v_int16 high;
  v_int16 low;
  oatpp::encoding::Unicode::codeToUtf16SurrogatePair(code, high, low);
  auto check = oatpp::encoding::Unicode::utf16SurrogatePairToCode(high, low);
  writeBinaryInt(code);
  writeBinaryInt(check);
  OATPP_ASSERT(code == check)
  
  for(v_int32 c = 0x010000; c <= 0x10FFFF; c++) {
    oatpp::encoding::Unicode::codeToUtf16SurrogatePair(code, high, low);
    check = oatpp::encoding::Unicode::utf16SurrogatePairToCode(high, low);
    OATPP_ASSERT(code == check)
  }

}
  
}}}
