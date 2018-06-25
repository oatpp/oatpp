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

#include "Parser.hpp"

namespace oatpp { namespace web { namespace url {
  
void Parser::parseQueryParamsToMap(Parameters& params, oatpp::parser::ParsingCaret& caret) {
  
  if(caret.findChar('?')) {
    
    do {
      caret.inc();
      oatpp::parser::ParsingCaret::Label nameLabel(caret);
      if(caret.findChar('=')) {
        nameLabel.end();
        caret.inc();
        oatpp::parser::ParsingCaret::Label valueLabel(caret);
        caret.findChar('&');
        params.put(nameLabel.toString(), valueLabel.toString());
      }
    } while (caret.canContinueAtChar('&'));
    
  }
  
}

void Parser::parseQueryParamsToMap(Parameters& params, const oatpp::String& str) {
  oatpp::parser::ParsingCaret caret(str.getPtr());
  parseQueryParamsToMap(params, caret);
}

std::shared_ptr<Parser::Parameters> Parser::parseQueryParams(oatpp::parser::ParsingCaret& caret) {
  auto params = Parameters::createShared();
  parseQueryParamsToMap(*params, caret);
  return params;
}

std::shared_ptr<Parser::Parameters> Parser::parseQueryParams(const oatpp::String& str) {
  auto params = Parameters::createShared();
  parseQueryParamsToMap(*params, str);
  return params;
}
  
}}}
