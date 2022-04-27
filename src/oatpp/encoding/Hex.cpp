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
  #include <winsock2.h>
#else
  #include <arpa/inet.h>
#endif

namespace oatpp { namespace encoding {
  
const char* Hex::ALPHABET_UPPER = "0123456789ABCDEF";
const char* Hex::ALPHABET_LOWER = "0123456789abcdef";
    
void Hex::writeUInt16(v_uint16 value, p_char8 buffer){
  *((p_uint32) buffer) = htonl((ALPHABET_UPPER[ value & 0x000F       ]      ) |
                               (ALPHABET_UPPER[(value & 0x00F0) >>  4] <<  8) |
                               (ALPHABET_UPPER[(value & 0x0F00) >>  8] << 16) |
                               (ALPHABET_UPPER[(value & 0xF000) >> 12] << 24));
  
}
  
void Hex::writeUInt32(v_uint32 value, p_char8 buffer){
  writeUInt16(value >> 16, buffer);
  writeUInt16(v_uint16(value), buffer + 4);
}
  
v_int32 Hex::readUInt16(const char* buffer, v_uint16& value) {
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
  
v_int32 Hex::readUInt32(const char* buffer, v_uint32& value) {
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

void Hex::encode(data::stream::ConsistentOutputStream* stream,
                 const void* data, v_buff_size size,
                 const char* alphabet)
{
  p_char8 buffer = (p_char8) data;
  v_char8 oneByteBuffer[2];
  for(v_buff_size i = 0; i < size; i ++) {
    auto c = buffer[i];
    v_char8 b1 = 0x0F & (c >> 4);
    v_char8 b2 = 0x0F & (c);
    oneByteBuffer[0] = alphabet[b1];
    oneByteBuffer[1] = alphabet[b2];
    stream->writeSimple(oneByteBuffer, 2);
  }
}

void Hex::decode(data::stream::ConsistentOutputStream* stream,
                 const void* data, v_buff_size size, bool allowSeparators)
{
  p_char8 buffer = (p_char8) data;
  v_char8 byte = 0;
  v_int32 shift = 4;
  for(v_buff_size i = 0; i < size; i ++) {

    auto a = buffer[i];

    if(a >= '0' && a <= '9') {
      byte |= (a - '0') << shift;
      shift -= 4;
    } else if (a >= 'A' && a <= 'F') {
      byte |= (a - 'A' + 10) << shift;
      shift -= 4;
    } else if (a >= 'a' && a <= 'f') {
      byte |= (a - 'a' + 10) << shift;
      shift -= 4;
    } else if(!allowSeparators) {
      throw DecodingError("Invalid Character");
    }

    if(shift < 0) {
      stream->writeCharSimple(byte);
      byte = 0;
      shift = 4;
    }

  }

  if(shift != 4) {
    throw DecodingError("Invalid input data size");
  }

}

}}
