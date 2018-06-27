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

#ifndef oatpp_web_url_Parser_hpp
#define oatpp_web_url_Parser_hpp

#include "oatpp/core/collection/ListMap.hpp"
#include "oatpp/core/parser/ParsingCaret.hpp"

namespace oatpp { namespace web { namespace url {
  
class Parser {
public:
  typedef oatpp::collection::ListMap<oatpp::String, oatpp::String> Parameters;
public:
  
  /**
   *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by ParsingCaret
   *  and put that params to Parameters map
   */
  static void parseQueryParamsToMap(Parameters& params, oatpp::parser::ParsingCaret& caret);
  
  /**
   *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by str
   *  and put that params to Parameters map
   */
  static void parseQueryParamsToMap(Parameters& params, const oatpp::String& str);
  
  /**
   *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by ParsingCaret
   */
  static std::shared_ptr<Parameters> parseQueryParams(oatpp::parser::ParsingCaret& caret);
  
  /**
   *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by str
   */
  static std::shared_ptr<Parameters> parseQueryParams(const oatpp::String& str);
  
};
  
}}}

#endif /* oatpp_web_url_Parser_hpp */
