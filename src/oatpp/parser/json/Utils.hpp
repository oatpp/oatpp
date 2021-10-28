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

#ifndef oatpp_parser_json_Utils_hpp
#define oatpp_parser_json_Utils_hpp

#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/Types.hpp"

#include <string>

namespace oatpp { namespace parser { namespace json {

/**
 * Utility class for json serializer/deserializer.
 * Used by &id:oatpp::parser::json::mapping::Serializer;, &id:oatpp::parser::json::mapping::Deserializer;.
 */
class Utils {
public:

  static constexpr v_uint32 FLAG_ESCAPE_SOLIDUS = 1;

  static constexpr v_uint32 FLAG_ESCAPE_ALL = FLAG_ESCAPE_SOLIDUS;

public:

  /**
   * ERROR_CODE_INVALID_ESCAPED_CHAR
   */
  static constexpr v_int64 ERROR_CODE_INVALID_ESCAPED_CHAR = 1;

  /**
   * ERROR_CODE_INVALID_SURROGATE_PAIR
   */
  static constexpr v_int64 ERROR_CODE_INVALID_SURROGATE_PAIR = 2;

  /**
   * '\\' - EXPECTED"
   * ERROR_CODE_PARSER_QUOTE_EXPECTED
   */
  static constexpr v_int64 ERROR_CODE_PARSER_QUOTE_EXPECTED = 3;

public:
  typedef oatpp::String String;
  typedef oatpp::parser::Caret ParsingCaret;
private:
  static v_buff_size escapeUtf8Char(const char* sequence, p_char8 buffer);
  static v_buff_size calcEscapedStringSize(const char* data, v_buff_size size, v_buff_size& safeSize, v_uint32 flags);
  static v_buff_size calcUnescapedStringSize(const char* data, v_buff_size size, v_int64& errorCode, v_buff_size& errorPosition);
  static void unescapeStringToBuffer(const char* data, v_buff_size size, p_char8 resultData);
  static const char* preparseString(ParsingCaret& caret, v_buff_size& size);
public:

  /**
   * Escape string as for json standard. <br>
   * *Note:* if(copyAsOwnData == false && escapedString == initialString) then result string will point to initial data.
   * @param data - pointer to string to escape.
   * @param size - data size.
   * @param flags - escape flags.
   * @return - &id:oatpp::String;.
   */
  static String escapeString(const char* data, v_buff_size size, v_uint32 flags = FLAG_ESCAPE_ALL);

  /**
   * Unescape string as for json standard.
   * @param data - pointer to string to unescape.
   * @param size - data size.
   * @param errorCode - out parameter. Error code <br>
   * *One of:*<br>
   * <ul>
   *   <li>&l:Utils::ERROR_CODE_INVALID_ESCAPED_CHAR;</li>
   *   <li>&l:Utils::ERROR_CODE_INVALID_SURROGATE_PAIR;</li>
   *   <li>&l:Utils::ERROR_CODE_PARSER_QUOTE_EXPECTED;</li>
   * </ul>
   * @param errorPosition - out parameter. Error position in data.
   * @return - &id:oatpp::String;.
   */
  static String unescapeString(const char* data, v_buff_size size, v_int64& errorCode, v_buff_size& errorPosition);

  /**
   * Same as &l:Utils::unescapeString (); but return `std::string`.
   * @param data - pointer to string to unescape.
   * @param size - data size.
   * @param errorCode - out parameter. Error code <br>
   * *One of:*<br>
   * <ul>
   *   <li>&l:Utils::ERROR_CODE_INVALID_ESCAPED_CHAR;</li>
   *   <li>&l:Utils::ERROR_CODE_INVALID_SURROGATE_PAIR;</li>
   *   <li>&l:Utils::ERROR_CODE_PARSER_QUOTE_EXPECTED;</li>
   * </ul>
   * @param errorPosition - out parameter. Error position in data.
   * @return - &id:oatpp::String;.
   */
  static std::string unescapeStringToStdString(const char* data, v_buff_size size, v_int64& errorCode, v_buff_size& errorPosition);

  /**
   * Parse string enclosed in `"<string>"`.
   * @param caret - &id:oatpp::parser::Caret;.
   * @return - &id:oatpp::String;.
   */
  static String parseString(ParsingCaret& caret);

  /**
   * Parse string enclosed in `"<string>"`.
   * @param caret - &id:oatpp::parser::Caret;.
   * @return - `std::string`.
   */
  static std::string parseStringToStdString(ParsingCaret& caret);
  
};
  
}}}

#endif /* oatpp_parser_json_Utils_hpp */
