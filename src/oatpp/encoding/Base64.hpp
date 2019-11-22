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

#ifndef oatpp_encoding_Base64_hpp
#define oatpp_encoding_Base64_hpp

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace encoding {

/**
 * Base64 - encoder/decoder.
 */
class Base64 {
public:

  /**
   * DecodingError.
   */
  class DecodingError : public std::runtime_error {
  public:

    /**
     * Constructor.
     * @param message - error message.
     */
    DecodingError(const char* message)
      :std::runtime_error(message)
    {}
    
  };
  
private:
  
  static v_char8 getAlphabetCharIndex(v_char8 a, const char* auxiliaryChars);
  
public:
  /**
   * Standard base64 Alphabet - `['A'-'Z', 'a'-'z', '0'-'9', '+', '/', '=']`.
   * Alphabet is array of 65 chars. 64 chars encoding chars, and 65th padding char.<br>
   */
  static const char* const ALPHABET_BASE64;

  /**
   * URL base64 Alphabet - `['A'-'Z', 'a'-'z', '0'-'9', '-', '_', '=']`.
   * Alphabet is array of 65 chars. 64 chars encoding chars, and 65th padding char.<br>
   */
  static const char* const ALPHABET_BASE64_URL;

  /**
   * URL safe base64 Alphabet - `['A'-'Z', 'a'-'z', '0'-'9', '.', '_', '-']`.
   * Alphabet is array of 65 chars. 64 chars encoding chars, and 65th padding char.<br>
   */
  static const char* const ALPHABET_BASE64_URL_SAFE;
  
  /**
   * Standard base64 Alphabet auxiliary chars ['+', '/', '='].
   * alphabet auxiliary chars - last 3 chars of alphabet including padding char.
   */
  static const char* const ALPHABET_BASE64_AUXILIARY_CHARS;

  /**
   * URL base64 Alphabet auxiliary chars ['-', '_', '='].
   * alphabet auxiliary chars - last 3 chars of alphabet including padding char.
   */
  static const char* const ALPHABET_BASE64_URL_AUXILIARY_CHARS;

  /**
   * URL safe base64 Alphabet auxiliary chars ['.', '_', '='].
   * alphabet auxiliary chars - last 3 chars of alphabet including padding char.
   */
  static const char* const ALPHABET_BASE64_URL_SAFE_AUXILIARY_CHARS;

  /**
   * Calculate size of encoding result of a string of the given size.
   * @param size - size of string to encode.
   * @return - size of encoding result of a string of the given size
   */
  static v_buff_size calcEncodedStringSize(v_buff_size size);

  /**
   * Calculate size of decoding result. this method assumes that data passed as a param consists of standard base64 set of chars
   * `['A'-'Z', 'a'-'z', '0'-'9']` and three configurable auxiliary chars.
   * @param data - pointer to data.
   * @param size - size of the data.
   * @param base64StrLength - out parameter. Size of base64 valid encoded string. It may appear to be less then size.
   * @param auxiliaryChars - configurable auxiliary chars.
   * @return - size of decoded data. If data passed is not a base64 string then -1 is returned.
   */
  static v_buff_size calcDecodedStringSize(const char* data, v_buff_size size, v_buff_size& base64StrLength, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);

  /**
   * Check if data is a valid base64 encoded string.
   * @param data - pointer to data.
   * @param size - data size.
   * @param auxiliaryChars - configurable auxiliary chars.
   * @return `(calcDecodedStringSize(data, size, base64StrLength, auxiliaryChars) >= 0)`.
   */
  static bool isBase64String(const char* data, v_buff_size size, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);

  /**
   * Encode data as base64 string.
   * @param data - pointer to data.
   * @param size - data size.
   * @param alphabet - base64 alphabet to use.
   * @return - encoded base64 string as &id:oatpp::String;.
   */
  static oatpp::String encode(const void* data, v_buff_size size, const char* alphabet = ALPHABET_BASE64);

  /**
   * Encode data as base64 string.
   * @param data - data to encode.
   * @param alphabet - base64 alphabet to use.
   * @return - encoded base64 string as &id:oatpp::String;.
   */
  static oatpp::String encode(const oatpp::String& data, const char* alphabet = ALPHABET_BASE64);

  /**
   * Decode base64 encoded data. This method assumes that data passed as a param consists of standard base64 set of chars
   * `['A'-'Z', 'a'-'z', '0'-'9']` and three configurable auxiliary chars.
   * @param data - pointer to data to decode.
   * @param size - encoded data size.
   * @param auxiliaryChars - configurable auxiliary chars.
   * @return - decoded data as &id:oatpp::String;.
   * @throws - &l:Base64::DecodingError;
   */
  static oatpp::String decode(const char* data, v_buff_size size, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);
  
  /**
   * Decode base64 encoded data. This method assumes that data passed as a param consists of standard base64 set of chars
   * `['A'-'Z', 'a'-'z', '0'-'9']` and three configurable auxiliary chars.
   * @param data - data to decode.
   * @param auxiliaryChars - configurable auxiliary chars.
   * @return - decoded data as &id:oatpp::String;.
   * @throws - &l:Base64::DecodingError;
   */
  static oatpp::String decode(const oatpp::String& data, const char* auxiliaryChars = ALPHABET_BASE64_AUXILIARY_CHARS);
  
};
  
}}

#endif /* oatpp_encoding_Base64_hpp */
