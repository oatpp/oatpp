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
    v_int32 result = (v_int32) std::strtol(str->data(), &end, 10);
    success = (((v_buff_size)end - (v_buff_size)str->data()) == str->size());
    return result;
  }

  v_uint32 strToUInt32(const char* str){
    char* end;
    return (v_uint32) std::strtoul(str, &end, 10);
  }

  v_uint32 strToUInt32(const oatpp::String& str, bool& success){
    char* end;
    v_uint32 result = (v_uint32) std::strtoul(str->data(), &end, 10);
    success = (((v_buff_size)end - (v_buff_size)str->data()) == str->size());
    return result;
  }
  
  v_int64 strToInt64(const char* str){
    char* end;
    return std::strtoll(str, &end, 10);
  }
  
  v_int64 strToInt64(const oatpp::String& str, bool& success){
    char* end;
    v_int64 result = std::strtoll(str->data(), &end, 10);
    success = (((v_buff_size)end - (v_buff_size)str->data()) == str->size());
    return result;
  }

  v_uint64 strToUInt64(const char* str){
    char* end;
    return std::strtoull(str, &end, 10);
  }

  v_uint64 strToUInt64(const oatpp::String& str, bool& success){
    char* end;
    v_uint64 result = std::strtoull(str->data(), &end, 10);
    success = (((v_buff_size)end - (v_buff_size)str->data()) == str->size());
    return result;
  }

  v_buff_size int32ToCharSequence(v_int32 value, p_char8 data, v_buff_size n) {
    return snprintf((char*)data, n, "%ld", (long) value);
  }

  v_buff_size uint32ToCharSequence(v_uint32 value, p_char8 data, v_buff_size n) {
    return snprintf((char*)data, n, "%lu", (unsigned long) value);
  }

  v_buff_size int64ToCharSequence(v_int64 value, p_char8 data, v_buff_size n) {
    return snprintf((char*)data, n, "%lld", (long long int) value);
  }

  v_buff_size uint64ToCharSequence(v_uint64 value, p_char8 data, v_buff_size n) {
    return snprintf((char*)data, n, "%llu", (long long unsigned int) value);
  }

  oatpp::String int32ToStr(v_int32 value){
    v_char8 buff [16]; // Max 10 digits with 1 sign. 16 is plenty enough.
    auto size = int32ToCharSequence(value, &buff[0], 16);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size);
    }
    return nullptr;
  }

  oatpp::String uint32ToStr(v_uint32 value){
    v_char8 buff [16]; // Max 10 digits. 16 is plenty enough.
    auto size = uint32ToCharSequence(value, &buff[0], 16);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size);
    }
    return nullptr;
  }
  
  oatpp::String int64ToStr(v_int64 value){
    v_char8 buff [32]; // Max 20 digits unsigned, 19 digits +1 sign signed.
    auto size = int64ToCharSequence(value, &buff[0], 32);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size);
    }
    return nullptr;
  }

  oatpp::String uint64ToStr(v_uint64 value){
    v_char8 buff [32]; // Max 20 digits.
    auto size = uint64ToCharSequence(value, &buff[0], 32);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size);
    }
    return nullptr;
  }
  
  std::string int32ToStdStr(v_int32 value){
    v_char8 buff [16];
    auto size = int32ToCharSequence(value, &buff[0], 16);
    if(size > 0){
      return std::string((const char*)buff, size);
    }
    return nullptr;
  }

  std::string uint32ToStdStr(v_uint32 value){
    v_char8 buff [16];
    auto size = uint32ToCharSequence(value, &buff[0], 16);
    if(size > 0){
      return std::string((const char*)buff, size);
    }
    return nullptr;
  }
  
  std::string int64ToStdStr(v_int64 value){
    v_char8 buff [32];
    v_int32 size = v_int32(int64ToCharSequence(value, &buff[0], 32));
    if(size > 0){
      return std::string((const char*)buff, size);
    }
    return nullptr;
  }

  std::string uint64ToStdStr(v_uint64 value){
    v_char8 buff [32];
    auto size = uint64ToCharSequence(value, &buff[0], 32);
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
    v_float32 result = std::strtof(str->data(), &end);
    success = (((v_buff_size)end - (v_buff_size)str->data()) == str->size());
    return result;
  }
  
  v_float64 strToFloat64(const char* str){
    char* end;
    return std::strtod(str, &end);
  }
  
  v_float64 strToFloat64(const oatpp::String& str, bool& success) {
    char* end;
    v_float64 result = std::strtod(str->data(), &end);
    success = (((v_buff_size)end - (v_buff_size)str->data()) == str->size());
    return result;
  }

  v_buff_size float32ToCharSequence(v_float32 value, p_char8 data, v_buff_size n, const char* format) {
    return snprintf((char*)data, n, format, value);
  }

v_buff_size float64ToCharSequence(v_float64 value, p_char8 data, v_buff_size n, const char* format) {
    return snprintf((char*)data, n, format, value);
  }
  
  oatpp::String float32ToStr(v_float32 value, const char* format) {
    v_char8 buff [100];
    auto size = float32ToCharSequence(value, &buff[0], 100, format);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size);
    }
    return nullptr;
  }
  
  oatpp::String float64ToStr(v_float64 value, const char* format) {
    v_char8 buff [100];
    auto size = float64ToCharSequence(value, &buff[0], 100, format);
    if(size > 0){
      return oatpp::String((const char*)&buff[0], size);
    }
    return nullptr;
  }
  
  oatpp::String boolToStr(bool value) {
    if(value){
      return oatpp::String("true", 4);
    } else {
      return oatpp::String("false", 5);
    }
  }
  
  bool strToBool(const oatpp::String& str, bool& success) {
    if(str == "true"){
      success = true;
      return true;
    } else if(str == "false"){
      success = true;
      return false;
    }
    success = false;
    return false;
  }
  
}}}
