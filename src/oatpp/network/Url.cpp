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
  v_buff_size pos0 = caret.getPosition();
  caret.findChar(':');
  v_buff_size size = caret.getPosition() - pos0;
  if(size > 0) {
    std::unique_ptr<v_char8[]> buff(new v_char8[size]);
    std::memcpy(buff.get(), &caret.getData()[pos0], size);
    utils::String::lowerCase_ASCII(buff.get(), size);
    return oatpp::String((const char*)buff.get(), size);
  }
  return nullptr;
}

Url::Authority Url::Parser::parseAuthority(oatpp::parser::Caret& caret) {
  
  const char* data = caret.getData();
  v_buff_size pos0 = caret.getPosition();
  v_buff_size pos = pos0;
  
  v_buff_size hostPos = pos0;
  v_buff_size atPos = -1;
  v_buff_size portPos = -1;
  
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
    result.userInfo = oatpp::String((const char*)&data[pos0], atPos - pos0);
  }
  
  if(portPos > hostPos) {
    result.host = oatpp::String((const char*)&data[hostPos], portPos - 1 - hostPos);
    char* end;
    result.port = std::strtol((const char*)&data[portPos], &end, 10);
    bool success = (((v_buff_size)end - (v_buff_size)&data[portPos]) == pos - portPos);
    if(!success) {
      caret.setError("Invalid port string");
    }
  } else {
    result.host = oatpp::String((const char*)&data[hostPos], pos - pos0);
  }
  
  return result;
  
}

oatpp::String Url::Parser::parsePath(oatpp::parser::Caret& caret) {
  auto label = caret.putLabel();
  caret.findCharFromSet("?#", 2);
  if(label.getSize() > 0) {
    return label.toString();
  }
  return nullptr;
}

void Url::Parser::parseQueryParams(Url::Parameters& params, oatpp::parser::Caret& caret) {

  if(caret.findChar('?')) {

    do {
      caret.inc();
      auto nameLabel = caret.putLabel();
      v_buff_size charFound = caret.findCharFromSet("=&");
      if(charFound == '=') {
        nameLabel.end();
        caret.inc();
        auto valueLabel = caret.putLabel();
        caret.findChar('&');
        params.put_LockFree(StringKeyLabel(caret.getDataMemoryHandle(), nameLabel.getData(), nameLabel.getSize()),
                            StringKeyLabel(caret.getDataMemoryHandle(), valueLabel.getData(), valueLabel.getSize()));
      } else {
        params.put_LockFree(StringKeyLabel(caret.getDataMemoryHandle(), nameLabel.getData(), nameLabel.getSize()), "");
      }
    } while (caret.canContinueAtChar('&'));

  }
  
}

void Url::Parser::parseQueryParams(Url::Parameters& params, const oatpp::String& str) {
  oatpp::parser::Caret caret(str.getPtr());
  parseQueryParams(params, caret);
}

Url::Parameters Url::Parser::parseQueryParams(oatpp::parser::Caret& caret) {
  Url::Parameters params;
  parseQueryParams(params, caret);
  return params;
}

Url::Parameters Url::Parser::parseQueryParams(const oatpp::String& str) {
  Url::Parameters params;
  parseQueryParams(params, str);
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

  caret.isAtText("//", 2, true);

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
