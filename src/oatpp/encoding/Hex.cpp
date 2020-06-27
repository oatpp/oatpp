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

#include "Hex.hpp"

#if defined(WIN32) || defined(_WIN32)
  #include <WinSock2.h>
#else
  #include <arpa/inet.h>
#endif

namespace oatpp { namespace encoding {
  
const v_char8 Hex::A_D[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  /*
const v_uint16 Hex::A_W16[] = {
  htons('0' | ('0' << 8)), htons('1' | ('0' << 8)), htons('2' | ('0' << 8)), htons('3' | ('0' << 8)), htons('4' | ('0' << 8)),
  htons('5' | ('0' << 8)), htons('6' | ('0' << 8)), htons('7' | ('0' << 8)), htons('8' | ('0' << 8)), htons('9' | ('0' << 8)),
  htons('A' | ('0' << 8)), htons('B' | ('0' << 8)), htons('C' | ('0' << 8)), htons('D' | ('0' << 8)), htons('E' | ('0' << 8)),
  htons('F' | ('0' << 8)), htons('0' | ('1' << 8)), htons('1' | ('1' << 8)), htons('2' | ('1' << 8)), htons('3' | ('1' << 8)),
  htons('4' | ('1' << 8)), htons('5' | ('1' << 8)), htons('6' | ('1' << 8)), htons('7' | ('1' << 8)), htons('8' | ('1' << 8)),
  htons('9' | ('1' << 8)), htons('A' | ('1' << 8)), htons('B' | ('1' << 8)), htons('C' | ('1' << 8)), htons('D' | ('1' << 8)),
  htons('E' | ('1' << 8)), htons('F' | ('1' << 8)), htons('0' | ('2' << 8)), htons('1' | ('2' << 8)), htons('2' | ('2' << 8)),
  htons('3' | ('2' << 8)), htons('4' | ('2' << 8)), htons('5' | ('2' << 8)), htons('6' | ('2' << 8)), htons('7' | ('2' << 8)),
  htons('8' | ('2' << 8)), htons('9' | ('2' << 8)), htons('A' | ('2' << 8)), htons('B' | ('2' << 8)), htons('C' | ('2' << 8)),
  htons('D' | ('2' << 8)), htons('E' | ('2' << 8)), htons('F' | ('2' << 8)), htons('0' | ('3' << 8)), htons('1' | ('3' << 8)),
  htons('2' | ('3' << 8)), htons('3' | ('3' << 8)), htons('4' | ('3' << 8)), htons('5' | ('3' << 8)), htons('6' | ('3' << 8)),
  htons('7' | ('3' << 8)), htons('8' | ('3' << 8)), htons('9' | ('3' << 8)), htons('A' | ('3' << 8)), htons('B' | ('3' << 8)),
  htons('C' | ('3' << 8)), htons('D' | ('3' << 8)), htons('E' | ('3' << 8)), htons('F' | ('3' << 8)), htons('0' | ('4' << 8)),
  htons('1' | ('4' << 8)), htons('2' | ('4' << 8)), htons('3' | ('4' << 8)), htons('4' | ('4' << 8)), htons('5' | ('4' << 8)),
  htons('6' | ('4' << 8)), htons('7' | ('4' << 8)), htons('8' | ('4' << 8)), htons('9' | ('4' << 8)), htons('A' | ('4' << 8)),
  htons('B' | ('4' << 8)), htons('C' | ('4' << 8)), htons('D' | ('4' << 8)), htons('E' | ('4' << 8)), htons('F' | ('4' << 8)),
  htons('0' | ('5' << 8)), htons('1' | ('5' << 8)), htons('2' | ('5' << 8)), htons('3' | ('5' << 8)), htons('4' | ('5' << 8)),
  htons('5' | ('5' << 8)), htons('6' | ('5' << 8)), htons('7' | ('5' << 8)), htons('8' | ('5' << 8)), htons('9' | ('5' << 8)),
  htons('A' | ('5' << 8)), htons('B' | ('5' << 8)), htons('C' | ('5' << 8)), htons('D' | ('5' << 8)), htons('E' | ('5' << 8)),
  htons('F' | ('5' << 8)), htons('0' | ('6' << 8)), htons('1' | ('6' << 8)), htons('2' | ('6' << 8)), htons('3' | ('6' << 8)),
  htons('4' | ('6' << 8)), htons('5' | ('6' << 8)), htons('6' | ('6' << 8)), htons('7' | ('6' << 8)), htons('8' | ('6' << 8)),
  htons('9' | ('6' << 8)), htons('A' | ('6' << 8)), htons('B' | ('6' << 8)), htons('C' | ('6' << 8)), htons('D' | ('6' << 8)),
  htons('E' | ('6' << 8)), htons('F' | ('6' << 8)), htons('0' | ('7' << 8)), htons('1' | ('7' << 8)), htons('2' | ('7' << 8)),
  htons('3' | ('7' << 8)), htons('4' | ('7' << 8)), htons('5' | ('7' << 8)), htons('6' | ('7' << 8)), htons('7' | ('7' << 8)),
  htons('8' | ('7' << 8)), htons('9' | ('7' << 8)), htons('A' | ('7' << 8)), htons('B' | ('7' << 8)), htons('C' | ('7' << 8)),
  htons('D' | ('7' << 8)), htons('E' | ('7' << 8)), htons('F' | ('7' << 8)), htons('0' | ('8' << 8)), htons('1' | ('8' << 8)),
  htons('2' | ('8' << 8)), htons('3' | ('8' << 8)), htons('4' | ('8' << 8)), htons('5' | ('8' << 8)), htons('6' | ('8' << 8)),
  htons('7' | ('8' << 8)), htons('8' | ('8' << 8)), htons('9' | ('8' << 8)), htons('A' | ('8' << 8)), htons('B' | ('8' << 8)),
  htons('C' | ('8' << 8)), htons('D' | ('8' << 8)), htons('E' | ('8' << 8)), htons('F' | ('8' << 8)), htons('0' | ('9' << 8)),
  htons('1' | ('9' << 8)), htons('2' | ('9' << 8)), htons('3' | ('9' << 8)), htons('4' | ('9' << 8)), htons('5' | ('9' << 8)),
  htons('6' | ('9' << 8)), htons('7' | ('9' << 8)), htons('8' | ('9' << 8)), htons('9' | ('9' << 8)), htons('A' | ('9' << 8)),
  htons('B' | ('9' << 8)), htons('C' | ('9' << 8)), htons('D' | ('9' << 8)), htons('E' | ('9' << 8)), htons('F' | ('9' << 8)),
  htons('0' | ('A' << 8)), htons('1' | ('A' << 8)), htons('2' | ('A' << 8)), htons('3' | ('A' << 8)), htons('4' | ('A' << 8)),
  htons('5' | ('A' << 8)), htons('6' | ('A' << 8)), htons('7' | ('A' << 8)), htons('8' | ('A' << 8)), htons('9' | ('A' << 8)),
  htons('A' | ('A' << 8)), htons('B' | ('A' << 8)), htons('C' | ('A' << 8)), htons('D' | ('A' << 8)), htons('E' | ('A' << 8)),
  htons('F' | ('A' << 8)), htons('0' | ('B' << 8)), htons('1' | ('B' << 8)), htons('2' | ('B' << 8)), htons('3' | ('B' << 8)),
  htons('4' | ('B' << 8)), htons('5' | ('B' << 8)), htons('6' | ('B' << 8)), htons('7' | ('B' << 8)), htons('8' | ('B' << 8)),
  htons('9' | ('B' << 8)), htons('A' | ('B' << 8)), htons('B' | ('B' << 8)), htons('C' | ('B' << 8)), htons('D' | ('B' << 8)),
  htons('E' | ('B' << 8)), htons('F' | ('B' << 8)), htons('0' | ('C' << 8)), htons('1' | ('C' << 8)), htons('2' | ('C' << 8)),
  htons('3' | ('C' << 8)), htons('4' | ('C' << 8)), htons('5' | ('C' << 8)), htons('6' | ('C' << 8)), htons('7' | ('C' << 8)),
  htons('8' | ('C' << 8)), htons('9' | ('C' << 8)), htons('A' | ('C' << 8)), htons('B' | ('C' << 8)), htons('C' | ('C' << 8)),
  htons('D' | ('C' << 8)), htons('E' | ('C' << 8)), htons('F' | ('C' << 8)), htons('0' | ('D' << 8)), htons('1' | ('D' << 8)),
  htons('2' | ('D' << 8)), htons('3' | ('D' << 8)), htons('4' | ('D' << 8)), htons('5' | ('D' << 8)), htons('6' | ('D' << 8)),
  htons('7' | ('D' << 8)), htons('8' | ('D' << 8)), htons('9' | ('D' << 8)), htons('A' | ('D' << 8)), htons('B' | ('D' << 8)),
  htons('C' | ('D' << 8)), htons('D' | ('D' << 8)), htons('E' | ('D' << 8)), htons('F' | ('D' << 8)), htons('0' | ('E' << 8)),
  htons('1' | ('E' << 8)), htons('2' | ('E' << 8)), htons('3' | ('E' << 8)), htons('4' | ('E' << 8)), htons('5' | ('E' << 8)),
  htons('6' | ('E' << 8)), htons('7' | ('E' << 8)), htons('8' | ('E' << 8)), htons('9' | ('E' << 8)), htons('A' | ('E' << 8)),
  htons('B' | ('E' << 8)), htons('C' | ('E' << 8)), htons('D' | ('E' << 8)), htons('E' | ('E' << 8)), htons('F' | ('E' << 8)),
  htons('0' | ('F' << 8)), htons('1' | ('F' << 8)), htons('2' | ('F' << 8)), htons('3' | ('F' << 8)), htons('4' | ('F' << 8)),
  htons('5' | ('F' << 8)), htons('6' | ('F' << 8)), htons('7' | ('F' << 8)), htons('8' | ('F' << 8)), htons('9' | ('F' << 8)),
  htons('A' | ('F' << 8)), htons('B' | ('F' << 8)), htons('C' | ('F' << 8)), htons('D' | ('F' << 8)), htons('E' | ('F' << 8)),
  htons('F' | ('F' << 8))
};
   */
    
void Hex::writeUInt16(v_uint16 value, p_char8 buffer){
  *((p_uint32) buffer) = htonl((A_D[ value & 0x000F       ]      ) |
                                  (A_D[(value & 0x00F0) >>  4] <<  8) |
                                  (A_D[(value & 0x0F00) >>  8] << 16) |
                                  (A_D[(value & 0xF000) >> 12] << 24));

  //*((p_int16) buffer) = A_W16[(value >> 8) & 0xFF];
  //*((p_int16) (buffer + 2)) = A_W16[value & 0xFF];
  
}
  
void Hex::writeUInt32(v_uint32 value, p_char8 buffer){
  writeUInt16(value >> 16, buffer);
  writeUInt16(v_uint16(value), buffer + 4);
}
  
v_int32 Hex::readUInt16(p_char8 buffer, v_uint16& value) {
  value = 0;
  for(v_int32 i = 0; i < 4; i++){
    v_char8 a = buffer[i];
    if(a >= '0' && a <= '9') {
      value |= (a - '0') << ((3 - i) << 2);
    } else if (a >= 'A' && a <= 'F') {
      value |= (a - 'A' + 10) << ((3 - i) << 2);
    } else if (a >= 'a' && a <= 'f') {
      value |= (a - 'a' + 10) << ((3 - i) << 2);
    } else {
      return ERROR_UNKNOWN_SYMBOL;
    }
  }
  return 0;
}
  
v_int32 Hex::readUInt32(p_char8 buffer, v_uint32& value) {
  value = 0;
  for(v_int32 i = 0; i < 8; i++){
    v_char8 a = buffer[i];
    if(a >= '0' && a <= '9') {
      value |= (a - '0') << ((7 - i) << 2);
    } else if (a >= 'A' && a <= 'F') {
      value |= (a - 'A' + 10) << ((7 - i) << 2);
    } else if (a >= 'a' && a <= 'f') {
      value |= (a - 'a' + 10) << ((7 - i) << 2);
    } else {
      return ERROR_UNKNOWN_SYMBOL;
    }
  }
  return 0;
}

}}
