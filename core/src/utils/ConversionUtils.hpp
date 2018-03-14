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

#ifndef oatpp_utils_ConversionUtils_hpp
#define oatpp_utils_ConversionUtils_hpp

#include "../base/String.hpp"

#include "../base/PtrWrapper.hpp"
#include "../base/Controllable.hpp"
#include "../base/Environment.hpp"

#include <string>

namespace oatpp { namespace utils { namespace conversion {
  
  v_int32 strToInt32(const char* str);
  v_int32 strToInt32(const base::PtrWrapper<base::String>& str, bool& success);
  v_int64 strToInt64(const char* str);
  v_int64 strToInt64(const base::PtrWrapper<base::String>& str, bool& success);
  
  v_int32 int32ToCharSequence(v_int32 value, p_char8 data);
  v_int32 int64ToCharSequence(v_int64 value, p_char8 data);
  
  base::PtrWrapper<base::String> int32ToStr(v_int32 value);
  base::PtrWrapper<base::String> int64ToStr(v_int64 value);
  
  std::string int32ToStdStr(v_int32 value);
  std::string int64ToStdStr(v_int64 value);
  
  template<typename T>
  v_int32 primitiveToCharSequence(T value, p_char8 data, const char* pattern){
    return sprintf((char*)data, pattern, value);
  }
  
  template<typename T>
  base::PtrWrapper<base::String> primitiveToStr(T value, const char* pattern){
    v_char8 buff [100];
    v_int32 size = primitiveToCharSequence(value, &buff[0], pattern);
    if(size > 0){
      return base::String::createShared(&buff[0], size, true);
    }
    return base::PtrWrapper<base::String>::empty();
  }
  
  v_float32 strToFloat32(const char* str);
  v_float32 strToFloat32(const base::PtrWrapper<base::String>& str, bool& success);
  v_float64 strToFloat64(const char* str);
  v_float64 strToFloat64(const base::PtrWrapper<base::String>& str, bool& success);
  
  v_int32 float32ToCharSequence(v_float32 value, p_char8 data);
  v_int32 float64ToCharSequence(v_float64 value, p_char8 data);
  
  base::PtrWrapper<base::String> float32ToStr(v_float32 value);
  base::PtrWrapper<base::String> float64ToStr(v_float64 value);
  
  base::PtrWrapper<base::String> boolToStr(bool value);
  bool strToBool(const base::PtrWrapper<base::String>& str, bool& success);
  
  
}}}

#endif /* oatpp_utils_ConversionUtils_hpp */
