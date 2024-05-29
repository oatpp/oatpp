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

#ifndef oatpp_encoding_Hex_hpp
#define oatpp_encoding_Hex_hpp

#include "oatpp/data/stream/Stream.hpp"
#include "oatpp/Types.hpp"

namespace oatpp { namespace encoding {

/**
 * Utility class for hex string encoding/decoding .
 */
class Hex {
public:
  static const char* ALPHABET_UPPER;
  static const char* ALPHABET_LOWER;
public:
  /**
   * Unknown symbol error.
   */
  static constexpr v_int32 ERROR_UNKNOWN_SYMBOL = 1;
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

public:

  /**
   * Write value as hex string to buffer.
   * @param value - value to write.
   * @param buffer - buffer for resultant string.
   */
  static void writeUInt16(v_uint16 value, p_char8 buffer);

  /**
   * Write value as hex string to buffer.
   * @param value - value to write.
   * @param buffer - buffer for resultant string.
   */
  static void writeUInt32(v_uint32 value, p_char8 buffer);

  /**
   * Parse 4-char hex string to int16.
   * @param buffer - buffer containing string to parse.
   * @param value - out parameter. Resultant value.
   * @return - 0 on success. Negative value on failure.
   */
  static v_int32 readUInt16(const char* buffer, v_uint16& value);

  /**
   * Parse 8-char hex string to int32.
   * @param buffer - buffer containing string to parse.
   * @param value - out parameter. Resultant value.
   * @return - 0 on success. Negative value on failure.
   */
  static v_int32 readUInt32(const char* buffer, v_uint32& value);

  /**
   * Write binary data as HEX string.
   * @param stream
   * @param data
   * @param size
   * @param alphabet
   */
  static void encode(data::stream::ConsistentOutputStream* stream,
                     const void* data, v_buff_size size,
                     const char* alphabet = ALPHABET_UPPER);

  /**
   * Write binary data as HEX string.
   * @param data
   * @param alphabet
   * @return
   */
  static oatpp::String encode(const oatpp::String& data, const char* alphabet = ALPHABET_UPPER);

  /**
   * Read binary data from hex string.
   * @param stream
   * @param data
   * @param size
   * @param allowSeparators - skip any char which is not ([A-Z], [a-z], [0-9]) without error.
   * @throws - &l:Hex::DecodingError;
   */
  static void decode(data::stream::ConsistentOutputStream* stream,
                     const void* data, v_buff_size size, bool allowSeparators = false);

  /**
   * Read binary data from hex string.
   * @param data
   * @param allowSeparators
   * @return
   */
  static oatpp::String decode(const oatpp::String& data, bool allowSeparators = false);
  
};
  
}}

#endif /* oatpp_encoding_Hex_hpp */
