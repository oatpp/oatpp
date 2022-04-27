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

#include "Unicode.hpp"

#if defined(WIN32) || defined(_WIN32)
  #include <winsock2.h>
#else
  #include <arpa/inet.h>
#endif

namespace oatpp { namespace encoding {
  
v_buff_size Unicode::getUtf8CharSequenceLength(v_char8 firstByte) {
  
  if(firstByte < 128){
    return 1;
  }
  
  if((firstByte | 192) != firstByte){
    return 0;
  }
  
  if((firstByte | 32) != firstByte){
    return 2;
  } else if((firstByte | 16) != firstByte){
    return 3;
  } else if((firstByte | 8) != firstByte){
    return 4;
  } else if((firstByte | 4) != firstByte){
    return 5;
  } else if((firstByte | 2) != firstByte){
    return 6;
  } else {
    return 0;
  }
  
}
  
v_buff_size Unicode::getUtf8CharSequenceLengthForCode(v_uint32 code){
  if(code < 128) {
    return 1;
  } else if(code < 0x00000800){
    return 2;
  } else if(code < 0x00010000){
    return 3;
  } else if(code < 0x00200000){
    return 4;
  } else if(code < 0x04000000){
    return 5;
  } else {
    return 6;
  }
}
  
v_int32 Unicode::encodeUtf8Char(const char* sequence, v_buff_size& length){
  v_char8 byte = sequence[0];
  if(byte > 127){
    v_int32 code;
    if((byte | 32) != byte){
      length = 2;
      code =  ((31 & byte) << 6) | (sequence[1] & 63);
      return code;
    } else if((byte | 16) != byte){
      code = (15 & byte) << 12;
      length = 3;
    } else if((byte | 8) != byte){
      length = 4;
      v_int32 value = *((p_int32)sequence);
      code =  ((7 & byte) << 18)                  |
              (((value >> 24) & 0xFF) & 63)       |
              (((value >> 16) & 0xFF) & 63) << 6  |
              (((value >>  8) & 0xFF) & 63) << 12;
      return code;
    } else if((byte | 4) != byte){
      code = (3 & byte) << 24;
      length = 5;
    } else if((byte | 2) != byte){
      code = (1 & byte) << 30;
      length = 6;
    } else {
      return -1;
    }
    
    v_char8 bitIndex = 0;
    for(v_buff_size i = length; i > 1; i--){
      code |= (sequence[i - 1] & 63) << bitIndex;
      bitIndex += 6;
    }
    return code;
  } else {
    length = 1;
    return byte;
  }
}
  
v_buff_size Unicode::decodeUtf8Char(v_int32 code, p_char8 buffer) {
  if(code >= 0x00000080 && code < 0x00000800){
    *((p_int16) buffer) = htons(((((code >> 6) & 31) | 192) << 8) | ((code & 63) | 128));
    return 2;
  } else if(code >= 0x00000800 && code < 0x00010000){
    *((p_int16) buffer) = htons((((( code >> 12 ) & 15) | 224) << 8) |
                                  (((code >>  6 ) & 63) | 128));
    buffer[2] = (code & 63) | 128;
    return 3;
  } else if(code >= 0x00010000 && code < 0x00200000){
    *((p_int32) buffer) = htonl(((((code >> 18 ) &  7) | 240) << 24) |
                                ((((code >> 12 ) & 63) | 128) << 16) |
                                ((((code >>  6 ) & 63) | 128) <<  8) |
                                 (( code         & 63) | 128)      );
    return 4;
  } else if(code >= 0x00200000 && code < 0x04000000){
    *((p_int32) buffer) = htonl(((((code >> 24 ) &  3) | 248) << 24) |
                                ((((code >> 18 ) & 63) | 128) << 16) |
                                ((((code >> 12 ) & 63) | 128) <<  8) |
                                 (((code >>  6 ) & 63) | 128));
    buffer[4] = (code & 63) | 128;
    return 5;
  } else if(code >= 0x04000000){
    *((p_int32) buffer) = htonl(((((code >> 30 ) &  1) | 252) << 24) |
                                ((((code >> 24 ) & 63) | 128) << 16) |
                                ((((code >> 18 ) & 63) | 128) <<  8) |
                                 (((code >> 12 ) & 63) | 128));
    *((p_int16) &buffer[4]) = htons(((((code >> 6 ) & 63) | 128) << 8) | (code & 63));
    return 6;
  }
  buffer[0] = v_char8(code);
  return 1;
}
  
void Unicode::codeToUtf16SurrogatePair(v_int32 code, v_int16& high, v_int16& low){
  code -= 0x010000;
  high = 0xD800 + ((code >> 10) & 1023);
  low = 0xDC00 + (code & 1023);
}
  
v_int32 Unicode::utf16SurrogatePairToCode(v_int16 high, v_int16 low){
  return (((low - 0xDC00) & 1023) | (((high - 0xD800) & 1023) << 10)) + 0x010000;
}
  
}}
