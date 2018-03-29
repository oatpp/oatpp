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

#ifndef oatpp_parser_json_Utils_hpp
#define oatpp_parser_json_Utils_hpp

#include "oatpp/core/parser/ParsingCaret.hpp"
#include "oatpp/core/base/String.hpp"

#include <string>

namespace oatpp { namespace parser { namespace json{
  
class Utils {
public:
  static const char* const ERROR_INVALID_ESCAPED_CHAR;
  static const char* const ERROR_INVALID_SURROGATE_PAIR;
  
  static const char* const ERROR_PARSER_QUOTE_EXPECTED;
public:
  typedef oatpp::base::String String;
  typedef oatpp::parser::ParsingCaret ParsingCaret;
private:
  static v_int32 escapeUtf8Char(p_char8 sequence, p_char8 buffer);
  static v_int32 calcEscapedStringSize(p_char8 data, v_int32 size, v_int32& safeSize);
  static v_int32 calcUnescapedStringSize(p_char8 data, v_int32 size, const char* & error, v_int32& errorPosition);
  static void unescapeStringToBuffer(p_char8 data, v_int32 size, p_char8 resultData);
  static p_char8 preparseString(const std::shared_ptr<ParsingCaret>& caret, v_int32& size);
public:
  
  static std::shared_ptr<String> escapeString(p_char8 data, v_int32 size);
  static std::shared_ptr<String> unescapeString(p_char8 data, v_int32 size, const char* & error, v_int32& errorPosition);
  static std::string unescapeStringToStdString(p_char8 data, v_int32 size,
                                               const char* & error, v_int32& errorPosition);
  
  static std::shared_ptr<String> parseString(const std::shared_ptr<ParsingCaret>& caret);
  static std::string parseStringToStdString(const std::shared_ptr<ParsingCaret>& caret);
  
};
  
}}}

#endif /* oatpp_parser_json_Utils_hpp */
