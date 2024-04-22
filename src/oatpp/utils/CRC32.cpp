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

#include "CRC32.hpp"

namespace oatpp { namespace utils {
  
const p_uint32 CRC32::TABLE_04C11DB7 = generateTable(0x04C11DB7);
  
v_uint32 CRC32::bitReverse(v_uint32 poly) {
  v_uint32 result = 0;
  for(v_int32 i = 0; i < 32; i ++) {
    if((poly & (1U << i)) > 0) {
      result |= (1U << (31 - i));
    }
  }
  return result;
}
  
p_uint32 CRC32::generateTable(v_uint32 poly) {
  
  p_uint32 result = new v_uint32[256];
  v_uint32 polyReverse = bitReverse(poly);
  v_uint32 value;
  
  for(v_uint32 i = 0; i < 256; i++) {
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
  
v_uint32 CRC32::calc(const void *buffer, v_buff_size size, v_uint32 crc, v_uint32 initValue, v_uint32 xorOut, p_uint32 table) {
  
  auto data = reinterpret_cast<const unsigned char*>(buffer);
  crc = crc ^ initValue;
  
  for(v_buff_size i = 0; i < size; i++) {
    crc = table[(crc & 0xFF) ^ data[i]] ^ (crc >> 8);
  }
  
  return crc ^ xorOut;
}
  
}}
