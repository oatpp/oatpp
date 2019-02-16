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

#include "CRC.hpp"

namespace oatpp { namespace algorithm {
  
const p_word32 CRC32::TABLE_04C11DB7 = generateTable(0x04C11DB7);
  
v_word32 CRC32::bitReverse(v_word32 poly) {
  v_word32 result = 0;
  for(v_int32 i = 0; i < 32; i ++) {
    if((poly & (1 << i)) > 0) {
      result |= 1 << (31 - i);
    }
  }
  return result;
}
  
p_word32 CRC32::generateTable(v_word32 poly) {
  
  p_word32 result = new v_word32[256];
  v_word32 polyReverse = bitReverse(poly);
  v_word32 value;
  
  for(v_int32 i = 0; i < 256; i++) {
    value = i;
    for (v_int32 bit = 0; bit < 8; bit++) {
      if (value & 1) {
        value = (value >> 1) ^ polyReverse;
      } else {
        value = (value >> 1);
      }
    }
    
    result[i] = value;
    
  }
  
  return result;
  
}
  
v_word32 CRC32::calc(const void *buffer, v_int32 size, v_word32 crc, v_word32 initValue, v_word32 xorOut, p_word32 table) {
  
  p_word8 data = (p_word8) buffer;
  crc = crc ^ initValue;
  
  for(v_int32 i = 0; i < size; i++) {
    crc = table[(crc & 0xFF) ^ data[i]] ^ (crc >> 8);
  }
  
  return crc ^ xorOut;
}
  
}}
