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

#ifndef oatpp_parser_ParsingError_hpp
#define oatpp_parser_ParsingError_hpp

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser {

/**
 * Thrown when parsing error occurred and ParsingCaret object is not accessible for user.
 * If parsing was made via oatpp::parser::ParsingCaret and ParsingCaret is accessible for user
 * then do not throw this error.- User should read error from ParsingCaret::getError()
 */
class ParsingError : public std::runtime_error {
private:
  oatpp::String m_message;
  v_int64 m_code;
  v_buff_size m_position;
public:

  /**
   * Constructor
   * @param message
   * @param position
   */
  ParsingError(const oatpp::String &message, v_int64 code, v_buff_size position);

  /**
   * get error message
   * @return
   */
  oatpp::String getMessage() const;

  /**
   * get error code
   * @return
   */
  v_int64 getCode() const;

  /**
   * get parsing position of the error
   * @return
   */
  v_buff_size getPosition() const;

};

}}

#endif //oatpp_parser_ParsingError_hpp
