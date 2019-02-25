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

#include "ConversionUtils.hpp"

#include <cstdlib>

namespace oatpp { namespace utils { namespace conversion {
  
  v_int32 strToInt32(const char* str){
    char* end;
    return (v_int32) std::strtol(str, &end, 10);
  }
  
  v_int32 strToInt32(const oatpp::String& str, bool& success){
    char* end;
    v_int32 result = (v_int32) std::strtol((const char*)str->getData(), &end, 10);
    success = (((v_int64)end - (v_int64)str->getData()) == str->getSize());
    return result;
  }
  
  v_int64 strToInt64(const char* str){
    char* end;
    return std::strtoll(str, &end, 10);
  }
  
  v_int64 strToInt64(const oatpp::String& str, bool& success){
    char* end;
    v_int64 result = std::strtoll((const char*)str->getData(), &end, 10);
    success = (((v_int64)end - (v_int64)str->getData()) == str->getSize());
    return result;
  }
  
  v_int32 int32ToCharSequence(v_int32 value, p_char8 data){
    return sprintf((char*)data, "%d", value);
  }
  
  v_int32 int64ToCharSequence(v_int64 value, p_char8 data){
    return sprintf((char*)data, "%lld", value);
  }
  
  oatpp::String int32ToStr(v_int32 value){
    v_char8 buff [100];
    v_int32 size = int32ToCharSequence(value, &buff[0]);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size, true);
    }
    return oatpp::String::empty();
  }
  
  oatpp::String int64ToStr(v_int64 value){
    v_char8 buff [100];
    v_int32 size = int64ToCharSequence(value, &buff[0]);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size, true);
    }
    return oatpp::String::empty();
  }
  
  std::string int32ToStdStr(v_int32 value){
    v_char8 buff [100];
    v_int32 size = int32ToCharSequence(value, &buff[0]);
    if(size > 0){
      return std::string((const char*)buff, size);
    }
    return nullptr;
  }
  
  std::string int64ToStdStr(v_int64 value){
    v_char8 buff [100];
    v_int32 size = int64ToCharSequence(value, &buff[0]);
    if(size > 0){
      return std::string((const char*)buff, size);
    }
    return nullptr;
  }
  
  v_float32 strToFloat32(const char* str){
    char* end;
    return std::strtof(str, &end);
  }
  
  v_float32 strToFloat32(const oatpp::String& str, bool& success) {
    char* end;
    v_float32 result = std::strtof((const char*)str->getData(), &end);
    success = (((v_int64)end - (v_int64)str->getData()) == str->getSize());
    return result;
  }
  
  v_float64 strToFloat64(const char* str){
    char* end;
    return std::strtod(str, &end);
  }
  
  v_float64 strToFloat64(const oatpp::String& str, bool& success) {
    char* end;
    v_float64 result = std::strtod((const char*)str->getData(), &end);
    success = (((v_int64)end - (v_int64)str->getData()) == str->getSize());
    return result;
  }
  
  v_int32 float32ToCharSequence(v_float32 value, p_char8 data){
    return sprintf((char*)data, "%f", value);
  }
  
  v_int32 float64ToCharSequence(v_float64 value, p_char8 data){
    return sprintf((char*)data, "%f", value);
  }
  
  oatpp::String float32ToStr(v_float32 value){
    v_char8 buff [100];
    v_int32 size = float32ToCharSequence(value, &buff[0]);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size, true);
    }
    return oatpp::String::empty();
  }
  
  oatpp::String float64ToStr(v_float64 value){
    v_char8 buff [100];
    v_int32 size = float64ToCharSequence(value, &buff[0]);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size, true);
    }
    return oatpp::String::empty();
  }
  
  oatpp::String boolToStr(bool value) {
    if(value){
      return oatpp::String("true", 4, false);
    } else {
      return oatpp::String("false", 5, false);
    }
  }
  
  bool strToBool(const oatpp::String& str, bool& success) {
    if(str->equals((p_char8)"true", 4)){
      success = true;
      return true;
    } else if(str->equals((p_char8)"false", 5)){
      success = true;
      return false;
    }
    success = false;
    return false;
  }
  
}}}
