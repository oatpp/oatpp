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

#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace encoding {
  
const char* Hex::ALPHABET_UPPER = "0123456789ABCDEF";
const char* Hex::ALPHABET_LOWER = "0123456789abcdef";
    
void Hex::writeUInt16(v_uint16 value, p_char8 buffer){
  *(reinterpret_cast<p_uint32>(buffer)) = htonl((static_cast<v_uint32>(ALPHABET_UPPER[ value & 0x000F       ])      ) |
                               (static_cast<v_uint32>(ALPHABET_UPPER[(value & 0x00F0) >>  4]) <<  8) |
                               (static_cast<v_uint32>(ALPHABET_UPPER[(value & 0x0F00) >>  8]) << 16) |
                               (static_cast<v_uint32>(ALPHABET_UPPER[(value & 0xF000) >> 12]) << 24));
  
}
  
void Hex::writeUInt32(v_uint32 value, p_char8 buffer){
  writeUInt16(static_cast<v_uint16>(value >> 16), buffer);
  writeUInt16(static_cast<v_uint16>(value & 0xffff), buffer + 4);
}
  
v_int32 Hex::readUInt16(const char* buffer, v_uint16& value) {
  value = 0;
  for(v_uint32 i = 0; i < 4; i++){
    v_char8 a = static_cast<v_char8>(buffer[i]);
    if(a >= '0' && a <= '9') {
      value |= static_cast<v_uint16>((a - '0') << ((3 - i) << 2));
    } else if (a >= 'A' && a <= 'F') {
      value |= static_cast<v_uint16>((a - 'A' + 10) << ((3 - i) << 2));
    } else if (a >= 'a' && a <= 'f') {
      value |= static_cast<v_uint16>((a - 'a' + 10) << ((3 - i) << 2));
    } else {
      return ERROR_UNKNOWN_SYMBOL;
    }
  }
  return 0;
}
  
v_int32 Hex::readUInt32(const char* buffer, v_uint32& value) {
  value = 0;
  for(v_uint32 i = 0; i < 8; i++){
    v_char8 a = static_cast<v_char8>(buffer[i]);
    if(a >= '0' && a <= '9') {
      value |= static_cast<v_uint32>(a - '0') << ((7 - i) << 2);
    } else if (a >= 'A' && a <= 'F') {
      value |= static_cast<v_uint32>(a - 'A' + 10) << ((7 - i) << 2);
    } else if (a >= 'a' && a <= 'f') {
      value |= static_cast<v_uint32>(a - 'a' + 10) << ((7 - i) << 2);
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
  auto buffer = reinterpret_cast<const char*>(data);
  char oneByteBuffer[2];
  for(v_buff_size i = 0; i < size; i ++) {
    v_char8 c = static_cast<v_char8>(buffer[i]);
    v_char8 b1 = 0x0F & (c >> 4);
    v_char8 b2 = 0x0F & (c);
    oneByteBuffer[0] = alphabet[b1];
    oneByteBuffer[1] = alphabet[b2];
    stream->writeSimple(oneByteBuffer, 2);
  }
}

oatpp::String Hex::encode(const oatpp::String& data, const char* alphabet) {
  oatpp::data::stream::BufferOutputStream ss(256);
  encode(&ss, data->data(), static_cast<v_buff_size>(data->size()), alphabet);
  return ss.toString();
}

void Hex::decode(data::stream::ConsistentOutputStream* stream,
                 const void* data, v_buff_size size, bool allowSeparators)
{
  auto buffer = reinterpret_cast<const char*>(data);
  v_char8 byte = 0;
  v_int32 shift = 4;
  for(v_buff_size i = 0; i < size; i ++) {

    auto a = buffer[i];

    if(a >= '0' && a <= '9') {
      byte |= static_cast<v_char8>((a - '0') << shift);
      shift -= 4;
    } else if (a >= 'A' && a <= 'F') {
      byte |= static_cast<v_char8>((a - 'A' + 10) << shift);
      shift -= 4;
    } else if (a >= 'a' && a <= 'f') {
      byte |= static_cast<v_char8>((a - 'a' + 10) << shift);
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

oatpp::String Hex::decode(const oatpp::String& data, bool allowSeparators) {
  oatpp::data::stream::BufferOutputStream ss(256);
  decode(&ss, data->data(), static_cast<v_buff_size>(data->size()), allowSeparators);
  return ss.toString();
}

}}
