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

#ifndef oatpp_encoding_Base64_hpp
#define oatpp_encoding_Base64_hpp

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace encoding {
  
class Base64 {
public:
  
  class DecodingError : public std::runtime_error {
  public:
    
    DecodingError(const char* message)
      :std::runtime_error(message)
    {}
    
  };
  
private:
  
  static v_char8 getAlphabetCharIndex(v_char8 a, const char* auxiliaryChars);
  
public:
  /**
   *  Alphabet is array of 65 chars. 64 chars encoding chars, and 65th padding char
   */
  constexpr static const char* ALPHABET_BASE64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
  constexpr static const char* ALPHABET_BASE64_URL = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";
  constexpr static const char* ALPHABET_BASE64_URL_SAFE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-";
  
  /**
   *  alphabet auxiliary chars - last 3 chars of alphabet including padding char.
   */
  constexpr static const char* ALPHABET_BASE64_AUXILIARY_CHARS = "+/=";
  constexpr static const char* ALPHABET_BASE64_URL_AUXILIARY_CHARS = "-_=";
  constexpr static const char* ALPHABET_BASE64_URL_SAFE_AUXILIARY_CHARS = "._-";
  
  /**
   *  Returns size of encoding result of a string of the given size
   */
  static v_int32 calcEncodedStringSize(v_int32 size);
  
  /**
   *  Returns size of decoding result. this method assumes that data passed as a param consists of standard base64 set of chars
   *  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 and three configurable auxiliary chars.
   *
   *  if data passed is not a base64 string then -1 is returned
   */
  static v_int32 calcDecodedStringSize(const char* data, v_int32 size, v_int32& base64StrLength, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);
  
  /**
   *  return (calcDecodedStringSize(data, size, auxiliaryChars) >= 0);
   */
  static bool isBase64String(const char* data, v_int32 size, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);
  
  /**
   *  encode data as base64 string
   */
  static oatpp::String encode(const void* data, v_int32 size, const char* alphabet = ALPHABET_BASE64);
  
  /**
   *  return encode(data->getData(), data->getSize(), alphabet);
   */
  static oatpp::String encode(const oatpp::String& data, const char* alphabet = ALPHABET_BASE64);
  
  /**
   *  decode() this method assumes that data passed as a param consists of standard base64 set of chars
   *  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 and three configurable auxiliary chars.
   *
   *  throws DecodingError(). in case invalid char found
   */
  static oatpp::String decode(const char* data, v_int32 size, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);
  
  /**
   *  return decode(data->getData(), data->getSize(), auxiliaryChars);
   */
  static oatpp::String decode(const oatpp::String& data, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);
  
};
  
}}

#endif /* oatpp_encoding_Base64_hpp */
