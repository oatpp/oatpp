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

#include "Url.hpp"

#include <cstdlib>

namespace oatpp { namespace network {
  
oatpp::String Url::Parser::parseScheme(oatpp::parser::Caret& caret) {
  v_int32 pos0 = caret.getPosition();
  caret.findChar(':');
  v_int32 size = caret.getPosition() - pos0;
  if(size > 0) {
    v_char8 buff[size];
    std::memcpy(buff, &caret.getData()[pos0], size);
    oatpp::base::StrBuffer::lowerCase(buff, size);
    return oatpp::String((const char*)buff, size, true);
  }
  return nullptr;
}

Url::Authority Url::Parser::parseAuthority(oatpp::parser::Caret& caret) {
  
  p_char8 data = caret.getData();
  v_int32 pos0 = caret.getPosition();
  v_int32 pos = pos0;
  
  v_int32 hostPos = pos0;
  v_int32 atPos = -1;
  v_int32 portPos = -1;
  
  while (pos < caret.getDataSize()) {
    v_char8 a = data[pos];
    if(a == '@') {
      atPos = pos;
      pos ++;
      hostPos = pos;
    } else if(a == ':') {
      pos ++;
      portPos = pos; // last ':' in authority proceeds port in case it goes after '@'
    } else if(a == '/' || a == '?' || a == '#') {
      if(pos == pos0) {
        return Url::Authority();
      }
      break;
    } else {
      pos ++;
    }
  }
  
  caret.setPosition(pos);
  
  Url::Authority result;
  
  if(atPos > -1) {
    result.userInfo = oatpp::String((const char*)&data[pos0], atPos - pos0, true);
  }
  
  if(portPos > hostPos) {
    result.host = oatpp::String((const char*)&data[hostPos], portPos - 1 - hostPos, true);
    char* end;
    result.port = (v_int32) std::strtol((const char*)&data[portPos], &end, 10);
    bool success = (((v_int64)end - (v_int64)&data[portPos]) == pos - portPos);
    if(!success) {
      caret.setError("Invalid port string");
    }
  } else {
    result.host = oatpp::String((const char*)&data[hostPos], pos - pos0, true);
  }
  
  return result;
  
}

oatpp::String Url::Parser::parsePath(oatpp::parser::Caret& caret) {
  auto label = caret.putLabel();
  caret.findCharFromSet((p_char8)"?#", 2);
  if(label.getSize() > 0) {
    return label.toString(true);
  }
  return nullptr;
}

void Url::Parser::parseQueryParamsToMap(Url::Parameters& params, oatpp::parser::Caret& caret) {
  
  if(caret.findChar('?')) {
    
    do {
      caret.inc();
      auto nameLabel = caret.putLabel();
      v_int32 charFound = caret.findCharFromSet("=&");
      if(charFound == '=') {
        nameLabel.end();
        caret.inc();
        auto valueLabel = caret.putLabel();
        caret.findChar('&');
        params[nameLabel.toString()] = valueLabel.toString();
      } else {
        params[nameLabel.toString()] = oatpp::String("", false);
      }
    } while (caret.canContinueAtChar('&'));
    
  }
  
}

void Url::Parser::parseQueryParamsToMap(Url::Parameters& params, const oatpp::String& str) {
  oatpp::parser::Caret caret(str.getPtr());
  parseQueryParamsToMap(params, caret);
}

Url::Parameters Url::Parser::parseQueryParams(oatpp::parser::Caret& caret) {
  Url::Parameters params;
  parseQueryParamsToMap(params, caret);
  return params;
}

Url::Parameters Url::Parser::parseQueryParams(const oatpp::String& str) {
  Url::Parameters params;
  parseQueryParamsToMap(params, str);
  return params;
}

Url::ParametersAsLabels Url::Parser::labelQueryParams(const oatpp::String& str) {

  Url::ParametersAsLabels params;
  oatpp::parser::Caret caret(str);

  if(caret.findChar('?')) {

    do {
      caret.inc();
      auto nameLabel = caret.putLabel();
      v_int32 charFound = caret.findCharFromSet("=&");
      if(charFound == '=') {
        nameLabel.end();
        caret.inc();
        auto valueLabel = caret.putLabel();
        caret.findChar('&');
        params[StringKeyLabel(str.getPtr(), nameLabel.getData(), nameLabel.getSize())] =
               StringKeyLabel(str.getPtr(), valueLabel.getData(), valueLabel.getSize());
      } else {
        params[StringKeyLabel(str.getPtr(), nameLabel.getData(), nameLabel.getSize())] = "";
      }
    } while (caret.canContinueAtChar('&'));

  }

  return params;

}

Url Url::Parser::parseUrl(oatpp::parser::Caret& caret) {

  Url result;

  if(caret.findChar(':')) {
    caret.setPosition(0);
    result.scheme = parseScheme(caret);
    caret.canContinueAtChar(':', 1);
  } else {
    caret.setPosition(0);
  }

  caret.isAtText((p_char8)"//", 2, true);

  if(!caret.isAtChar('/')) {
    result.authority = parseAuthority(caret);
  }

  result.path = parsePath(caret);
  result.queryParams = parseQueryParams(caret);

  return result;
}

Url Url::Parser::parseUrl(const oatpp::String& str) {
  oatpp::parser::Caret caret(str);
  return parseUrl(caret);
}
  
}}
