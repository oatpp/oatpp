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

#include "Utils.hpp"

#include "oatpp/encoding/Unicode.hpp"
#include "oatpp/encoding/Hex.hpp"

namespace oatpp { namespace json{

v_buff_size Utils::calcEscapedStringSize(const char* data, v_buff_size size, v_buff_size& safeSize, v_uint32 flags) {
  v_buff_size result = 0;
  v_buff_size i = 0;
  safeSize = size;
  while (i < size) {
    v_char8 a = static_cast<v_char8>(data[i]);
    if(a < 32) {
      i ++;

      switch (a) {

        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t': result += 2; break; // '\n'

        default:
          result += 6; // '\uFFFF' - 6 chars
          break;

      }

    } else if(a < 128){
      i ++;

      switch (a) {
        case '\"':
        case '\\': result += 2; break; // '\/'

        case '/':
          result ++;
          if((flags & FLAG_ESCAPE_SOLIDUS) > 0) result ++;
          break;

        default:
          result ++;
          break;

      }

    } else {
      v_buff_size charSize = oatpp::encoding::Unicode::getUtf8CharSequenceLength(a);
      if(charSize != 0) {
        if(i + charSize > size) {
          safeSize = i;
        }
        i += charSize;
        if (!(flags & FLAG_ESCAPE_UTF8CHAR)) {
          result += charSize; // output as-is
        } else if(charSize < 4) {
          result += 6; // '\uFFFF' - 6 chars
        } else if(charSize == 4) {
          result += 12; // '\uFFFF\uFFFF' - 12 chars surrogate pair
        } else {
          result += 11; // '\u+FFFFFFFF' - 11 chars NOT JSON standard case
        }
      } else {
        // invalid char
        i ++;
        result ++;
      }
    }
  }
  return result;
}

v_buff_size Utils::calcUnescapedStringSize(const char* data, v_buff_size size, v_int64& errorCode, v_buff_size& errorPosition) {
  errorCode = 0;
  v_buff_size result = 0;
  v_buff_size i = 0;
  
  while (i < size) {
    v_char8 a = static_cast<v_char8>(data[i]);
    if(a == '\\'){
      
      if(i + 1 == size){
        errorCode = ERROR_CODE_INVALID_ESCAPED_CHAR;
        errorPosition = i;
        return 0;
      }
      
      v_char8 b = static_cast<v_char8>(data[i + 1]);
      
      if(b == '"' || b == '\\' || b == '/' || b == 'b' || b == 'f' || b == 'n' || b == 'r' || b == 't'){
        result += 1;
        i += 2;
      } else if(b == 'u'){
        
        if(i + 6 > size){
          errorCode = ERROR_CODE_INVALID_ESCAPED_CHAR;
          errorPosition = i;
          return 0;
        }
        
        if(data[i + 2] == '+') { // not JSON standard case
          if(i + 11 > size){
            errorCode = ERROR_CODE_INVALID_ESCAPED_CHAR;
            errorPosition = i;
            return 0;
          }
          v_uint32 code;
          errorCode = encoding::Hex::readUInt32(&data[i + 3], code);
          if(errorCode != 0){
            errorPosition = i + 3;
            return 0;
          }
          i += 11;
          result += encoding::Unicode::getUtf8CharSequenceLengthForCode(code);
        } else {
          v_uint16 code;
          errorCode = encoding::Hex::readUInt16(&data[i + 2], code);
          if(errorCode != 0){
            errorPosition = i + 2;
            return 0;
          }
          
          if(code >= 0xD800 && code <= 0xDBFF){
            if(i + 12 > size){
              errorCode = ERROR_CODE_INVALID_SURROGATE_PAIR;
              errorPosition = i;
              return 0;
            }
            v_uint16 low;
            errorCode = encoding::Hex::readUInt16(&data[i + 8], low);
            if(errorCode != 0){
              errorPosition = i + 8;
              return 0;
            }
            
            if(low >= 0xDC00 && low <= 0xDFFF){
              v_uint32 bigCode = static_cast<v_uint32>(encoding::Unicode::utf16SurrogatePairToCode(static_cast<v_int16>(code), static_cast<v_int16>(low)));
              i += 12;
              result += encoding::Unicode::getUtf8CharSequenceLengthForCode(bigCode);
            } else {
              errorCode = ERROR_CODE_INVALID_SURROGATE_PAIR;
              errorPosition = i;
              return 0;
            }
            
          } else {
            i += 6;
            result += encoding::Unicode::getUtf8CharSequenceLengthForCode(code);
          }
        }
        
      } else {
        errorCode = ERROR_CODE_INVALID_ESCAPED_CHAR;
        errorPosition = i;
        return 0;
      }
      
    } else {
      i ++;
      result ++;
    }
    
  }
  
  return result;
}
  
v_buff_size Utils::escapeUtf8Char(const char* sequence, p_char8 buffer){
  v_buff_size length;
  v_int32 code = oatpp::encoding::Unicode::encodeUtf8Char(sequence, length);
  if(code < 0x00010000) {
    buffer[0] = '\\';
    buffer[1] = 'u';
    oatpp::encoding::Hex::writeUInt16(v_uint16(code), &buffer[2]);
    return 6;
  } else if(code < 0x00200000) {
    v_int16 high;
    v_int16 low;
    oatpp::encoding::Unicode::codeToUtf16SurrogatePair(code, high, low);
    buffer[0] = '\\';
    buffer[1] = 'u';
    oatpp::encoding::Hex::writeUInt16(static_cast<v_uint16>(high), &buffer[2]);
    buffer[6] = '\\';
    buffer[7] = 'u';
    oatpp::encoding::Hex::writeUInt16(static_cast<v_uint16>(low), &buffer[8]);
    return 12;
  } else {
    buffer[0] = '\\';
    buffer[1] = 'u';
    buffer[2] = '+';
    oatpp::encoding::Hex::writeUInt32(static_cast<v_uint32>(code), &buffer[2]);
    return 11;
  }
}

oatpp::String Utils::escapeString(const char* data, v_buff_size size, v_uint32 flags) {
  v_buff_size safeSize;
  v_buff_size escapedSize = calcEscapedStringSize(data, size, safeSize, flags);
  if(escapedSize == size) {
    return String(data, size);
  }
  String result(escapedSize);
  auto resultData = reinterpret_cast<p_char8>(result->data());
  v_buff_size pos = 0;

  {
    v_buff_size i = 0;
    while (i < safeSize) {
      v_char8 a = static_cast<v_char8>(data[i]);
      if (a < 32) {

        switch (a) {

          case '\b': resultData[pos] = '\\'; resultData[pos + 1] = 'b'; pos += 2; break;
          case '\f': resultData[pos] = '\\'; resultData[pos + 1] = 'f'; pos += 2; break;
          case '\n': resultData[pos] = '\\'; resultData[pos + 1] = 'n'; pos += 2; break;
          case '\r': resultData[pos] = '\\'; resultData[pos + 1] = 'r'; pos += 2; break;
          case '\t': resultData[pos] = '\\'; resultData[pos + 1] = 't'; pos += 2; break;

          default:
            resultData[pos] = '\\';
            resultData[pos + 1] = 'u';
            oatpp::encoding::Hex::writeUInt16(a, &resultData[pos + 2]);
            pos += 6;
            break;

        }

        i++;

      }
      else if (a < 128) {

        switch (a) {
          case '\"': resultData[pos] = '\\'; resultData[pos + 1] = '"'; pos += 2; break;
          case '\\': resultData[pos] = '\\'; resultData[pos + 1] = '\\'; pos += 2; break;

          case '/':
            if((flags & FLAG_ESCAPE_SOLIDUS) > 0) {
              resultData[pos] = '\\';
              resultData[pos + 1] = '/';
              pos += 2;
            } else {
              resultData[pos] = static_cast<v_char8>(data[i]);
              pos++;
            }
            break;

          default:
            resultData[pos] = static_cast<v_char8>(data[i]);
            pos++;
            break;

        }

        i++;
      }
      else {
        v_buff_size charSize = oatpp::encoding::Unicode::getUtf8CharSequenceLength(a);
        if (charSize != 0) {
          if (!(flags & FLAG_ESCAPE_UTF8CHAR)) {
            std::memcpy(reinterpret_cast<void*>(&resultData[pos]), reinterpret_cast<void*>(const_cast<char*>(&data[i])), static_cast<size_t>(charSize));
            pos += charSize;
          }
          else {
            pos += escapeUtf8Char(&data[i], &resultData[pos]);
          }
          i += charSize;
        }
        else {
          // invalid char
          resultData[pos] = static_cast<v_char8>(data[i]);
          i++;
          pos++;
        }
      }
    }
  }
  
  if(size > safeSize){
    for(v_buff_size i = pos; static_cast<size_t>(i) < result->size(); i ++){
      resultData[i] = '?';
    }
  }
  
  return result;
}

void Utils::unescapeStringToBuffer(const char* data, v_buff_size size, p_char8 resultData){
  
  v_buff_size i = 0;
  v_buff_size pos = 0;
  
  while (i < size) {
    v_char8 a = static_cast<v_char8>(data[i]);
    
    if(a == '\\'){
      v_char8 b = static_cast<v_char8>(data[i + 1]);
      if(b != 'u'){
        switch (b) {
          case '"': resultData[pos] = '"'; pos ++; break;
          case '\\': resultData[pos] = '\\'; pos ++; break;
          case '/': resultData[pos] = '/'; pos ++; break;
          case 'b': resultData[pos] = '\b'; pos ++; break;
          case 'f': resultData[pos] = '\f'; pos ++; break;
          case 'n': resultData[pos] = '\n'; pos ++; break;
          case 'r': resultData[pos] = '\r'; pos ++; break;
          case 't': resultData[pos] = '\t'; pos ++; break;
          default: break;
        }
        i += 2;
      } else {
        if(data[i + 2] == '+'){ // Not JSON standard case
          v_uint32 code;
          encoding::Hex::readUInt32(&data[i + 3], code);
          i += 11;
          pos += encoding::Unicode::decodeUtf8Char(static_cast<v_int32>(code), &resultData[pos]);
        } else {
          
          v_uint16 code;
          encoding::Hex::readUInt16(&data[i + 2], code);
          
          if(code >= 0xD800 && code <= 0xDBFF){
            v_uint16 low;
            encoding::Hex::readUInt16(&data[i + 8], low);
            v_uint32 bigCode = static_cast<v_uint32>(encoding::Unicode::utf16SurrogatePairToCode(static_cast<v_int16>(code), static_cast<v_int16>(low)));
            pos += encoding::Unicode::decodeUtf8Char(static_cast<v_int32>(bigCode), &resultData[pos]);
            i += 12;
          } else {
            pos += encoding::Unicode::decodeUtf8Char(code, &resultData[pos]);
            i += 6;
          }
          
        }
      }
    } else {
      resultData[pos] = a;
      pos ++;
      i++;
    }
    
  }
  
}
  
oatpp::String Utils::unescapeString(const char* data, v_buff_size size, v_int64& errorCode, v_buff_size& errorPosition) {
  
  v_buff_size unescapedSize = calcUnescapedStringSize(data, size, errorCode, errorPosition);
  if(errorCode != 0){
    return nullptr;
  }
  String result(unescapedSize);
  if(unescapedSize == size) {
    std::memcpy(reinterpret_cast<void*>(result->data()), data, static_cast<size_t>(size));
  } else {
    unescapeStringToBuffer(data, size, reinterpret_cast<p_char8>(result->data()));
  }
  return result;
  
}
  
std::string Utils::unescapeStringToStdString(const char* data, v_buff_size size, v_int64& errorCode, v_buff_size& errorPosition){
  
  v_buff_size unescapedSize = calcUnescapedStringSize(data, size, errorCode, errorPosition);
  if(errorCode != 0){
    return "";
  }
  std::string result;
  result.resize(static_cast<size_t>(unescapedSize));
  if(unescapedSize == size) {
    std::memcpy(reinterpret_cast<void*>(result.data()), data, static_cast<size_t>(size));
  } else {
    unescapeStringToBuffer(data, size, reinterpret_cast<p_char8>(result.data()));
  }
  return result;
  
}
  
const char* Utils::preparseString(ParsingCaret& caret, v_buff_size& size){
  
  if(caret.canContinueAtChar('"', 1)){
    
    const char* data = caret.getData();
    v_buff_size pos = caret.getPosition();
    v_buff_size pos0 = pos;
    v_buff_size length = caret.getDataSize();
    
    while (pos < length) {
      v_char8 a = static_cast<v_char8>(data[pos]);
      if(a == '"'){
        size = pos - pos0;
        return &data[pos0];
      } else if(a == '\\') {
        pos += 2;
      } else {
        pos ++;
      }
    }
    caret.setPosition(caret.getDataSize());
    caret.setError("[oatpp::json::Utils::preparseString()]: Error. '\"' - expected", ERROR_CODE_PARSER_QUOTE_EXPECTED);
  } else {
    caret.setError("[oatpp::json::Utils::preparseString()]: Error. '\"' - expected", ERROR_CODE_PARSER_QUOTE_EXPECTED);
  }
  
  return nullptr;
  
}
  
oatpp::String Utils::parseString(ParsingCaret& caret) {
  
  v_buff_size size;
  const char* data = preparseString(caret, size);
  
  if(data != nullptr) {
  
    v_buff_size pos = caret.getPosition();
    
    v_int64 errorCode;
    v_buff_size errorPosition;
    auto result = unescapeString(data, size, errorCode, errorPosition);
    if(errorCode != 0){
      caret.setError("[oatpp::json::Utils::parseString()]: Error. Call to unescapeString() failed", errorCode);
      caret.setPosition(pos + errorPosition);
    } else {
      caret.setPosition(pos + size + 1);
    }
    
    return result;
    
  }
  
  return nullptr;
  
}
  
std::string Utils::parseStringToStdString(ParsingCaret& caret){
  
  v_buff_size size;
  auto data = preparseString(caret, size);
  
  if(data != nullptr) {
    
    v_buff_size pos = caret.getPosition();
    
    v_int64 errorCode;
    v_buff_size errorPosition;
    const std::string& result = unescapeStringToStdString(data, size, errorCode, errorPosition);
    if(errorCode != 0){
      caret.setError("[oatpp::json::Utils::parseStringToStdString()]: Error. Call to unescapeStringToStdString() failed", errorCode);
      caret.setPosition(pos + errorPosition);
    } else {
      caret.setPosition(pos + size + 1);
    }
    
    return result;
    
  }
  
  return "";
  
}

bool Utils::findDecimalSeparatorInCurrentNumber(ParsingCaret& caret) {
  utils::parser::Caret::StateSaveGuard stateGuard(caret);

  // search until a decimal separator is found or no more digits/sign are found or no more data available
  while(caret.canContinue()) {
    if (caret.isAtChar(JSON_DECIMAL_SEPARATOR)) {
      return true;
    }
    if (!caret.isAtDigitChar() && !caret.isAtChar('-')) {
      return false;
    }
    caret.inc();
  }
  return false;
}
  
}}
