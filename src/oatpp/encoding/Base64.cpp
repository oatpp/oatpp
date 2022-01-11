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

#include "Base64.hpp"

namespace oatpp { namespace encoding {
  
const char* const Base64::ALPHABET_BASE64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const char* const Base64::ALPHABET_BASE64_URL = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";
const char* const Base64::ALPHABET_BASE64_URL_SAFE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-";

const char* const Base64::ALPHABET_BASE64_AUXILIARY_CHARS = "+/=";
const char* const Base64::ALPHABET_BASE64_URL_AUXILIARY_CHARS = "-_=";
const char* const Base64::ALPHABET_BASE64_URL_SAFE_AUXILIARY_CHARS = "._-";
  
v_char8 Base64::getAlphabetCharIndex(v_char8 a, const char* auxiliaryChars) {
  if(a >= 'A' && a <='Z') {
    return a - 'A';
  }
  if(a >= 'a' && a <='z') {
    return a - 'a' + 26;
  }
  if(a >= '0' && a <='9') {
    return a - '0' + 52;
  }
  if(a == auxiliaryChars[0]) {
    return 62;
  }
  if(a == auxiliaryChars[1]) {
    return 63;
  }
  return 255;
}
  
v_buff_size Base64::calcEncodedStringSize(v_buff_size size) {
  v_buff_size size3 = size / 3;
  v_buff_size rSize = size3 * 3;
  if(rSize < size){
    rSize += 4;
  }
  return rSize + size3; // resultSize = (size3 * 3 + size3) = size3 * 4
}
  
v_buff_size Base64::calcDecodedStringSize(const char* data, v_buff_size size, v_buff_size& base64StrLength, const char* auxiliaryChars) {
  
  base64StrLength = size;
  
  v_char8 auxChar1 = auxiliaryChars[0];
  v_char8 auxChar2 = auxiliaryChars[1];
  v_char8 paddingChar = auxiliaryChars[2];
  
  v_buff_size i = 0;
  while (i < size) {
    
    v_char8 a = data[i];
    
    bool isValidChar = (a >= 'A' && a <='Z') || (a >= 'a' && a <='z') || (a >= '0' && a <='9') || (a == auxChar1) || (a == auxChar2);
    if(!isValidChar) {
      if(a == paddingChar){
        base64StrLength = i;
        break;
      }
      return -1;
    }
    
    i++;
    
  }
  
  v_buff_size size4 = i >> 2;
  v_buff_size size4d = i - (size4 << 2);
  v_buff_size resultSize = size4 * 3;
  if(size4d > 0) {
    resultSize += size4d - 1;
  }
  return resultSize;
  
}
  
bool Base64::isBase64String(const char* data, v_buff_size size, const char* auxiliaryChars) {
  v_buff_size base64StrLength;
  return (calcDecodedStringSize(data, size, base64StrLength, auxiliaryChars) >= 0);
}
  
oatpp::String Base64::encode(const void* data, v_buff_size size, const char* alphabet) {
  
  auto resultSize = calcEncodedStringSize(size);
  
  auto result = oatpp::String(resultSize);
  
  p_char8 bdata = (p_char8) data;
  p_char8 resultData = (p_char8) result->data();
  
  v_buff_size pos = 0;
  while (pos + 2 < size) {
    
    v_char8 b0 = bdata[pos];
    v_char8 b1 = bdata[pos + 1];
    v_char8 b2 = bdata[pos + 2];
    resultData[0] = alphabet[(b0 & 252) >> 2];
    resultData[1] = alphabet[((b0 & 3) << 4) | ((b1 >> 4) & 15)];
    resultData[2] = alphabet[((b1 & 15) << 2) | ((b2 >> 6) & 3)];
    resultData[3] = alphabet[(b2 & 63)];
    resultData += 4;
    pos += 3;
    
  }
  
  if(pos + 1 < size) {
    v_char8 b0 = bdata[pos];
    v_char8 b1 = bdata[pos + 1];
    resultData[0] = alphabet[(b0 & 252) >> 2];
    resultData[1] = alphabet[((b0 & 3) << 4) | ((b1 >> 4) & 15)];
    resultData[2] = alphabet[((b1 & 15) << 2)];
    resultData[3] = alphabet[64];
  } else if(pos < size) {
    v_char8 b0 = bdata[pos];
    resultData[0] = alphabet[(b0 & 252) >> 2];
    resultData[1] = alphabet[(b0 & 3) << 4];
    resultData[2] = alphabet[64];
    resultData[3] = alphabet[64];
  }
  
  return result;
  
}
  
oatpp::String Base64::encode(const oatpp::String& data, const char* alphabet) {
  return encode(data->data(), data->size(), alphabet);
}
  
oatpp::String Base64::decode(const char* data, v_buff_size size, const char* auxiliaryChars) {
  
  v_buff_size base64StrLength;
  auto resultSize = calcDecodedStringSize(data, size, base64StrLength, auxiliaryChars);
  if(resultSize < 0) {
    throw DecodingError("Data is no base64 string. Make sure that auxiliaryChars match with encoder alphabet");
  }
  
  auto result = oatpp::String(resultSize);
  p_char8 resultData = (p_char8) result->data();
  v_buff_size pos = 0;
  while (pos + 3 < base64StrLength) {
    v_char8 b0 = getAlphabetCharIndex(data[pos], auxiliaryChars);
    v_char8 b1 = getAlphabetCharIndex(data[pos + 1], auxiliaryChars);
    v_char8 b2 = getAlphabetCharIndex(data[pos + 2], auxiliaryChars);
    v_char8 b3 = getAlphabetCharIndex(data[pos + 3], auxiliaryChars);
    
    resultData[0] = (b0 << 2) | ((b1 >> 4) & 3);
    resultData[1] = ((b1 & 15) << 4) | ((b2 >> 2) & 15);
    resultData[2] = ((b2 & 3) << 6) | b3;
    
    resultData += 3;
    pos += 4;
  }
  
  v_buff_size posDiff = base64StrLength - pos;
  if(posDiff == 3) {
    v_char8 b0 = getAlphabetCharIndex(data[pos], auxiliaryChars);
    v_char8 b1 = getAlphabetCharIndex(data[pos + 1], auxiliaryChars);
    v_char8 b2 = getAlphabetCharIndex(data[pos + 2], auxiliaryChars);
    resultData[0] = (b0 << 2) | ((b1 >> 4) & 3);
    resultData[1] = ((b1 & 15) << 4) | ((b2 >> 2) & 15);
  } else if(posDiff == 2) {
    v_char8 b0 = getAlphabetCharIndex(data[pos], auxiliaryChars);
    v_char8 b1 = getAlphabetCharIndex(data[pos + 1], auxiliaryChars);
    resultData[0] = (b0 << 2) | ((b1 >> 4) & 3);
  }
  
  return result;
  
}
  
oatpp::String Base64::decode(const oatpp::String& data, const char* auxiliaryChars) {
  return decode((const char*)data->data(), data->size(), auxiliaryChars);
}
  
}}
