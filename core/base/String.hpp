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

#ifndef oatpp_base_String_hpp
#define oatpp_base_String_hpp

#include "memory/ObjectPool.hpp"
#include "./Controllable.hpp"
#include "./PtrWrapper.hpp"

#include <cstring> // c

namespace oatpp { namespace base {

class String : public oatpp::base::Controllable {
public:
  
  class PtrWrapper : public oatpp::base::PtrWrapper<String> {
  public:
    
    PtrWrapper() {}
    
    PtrWrapper(const std::shared_ptr<String>& ptr)
      : oatpp::base::PtrWrapper<String>(ptr)
    {}
    
    PtrWrapper(std::shared_ptr<String>&& ptr)
      : oatpp::base::PtrWrapper<String>(std::move(ptr))
    {}
    
    PtrWrapper(const char* str)
      : oatpp::base::PtrWrapper<String>(createFromCString(str))
    {}
    
    PtrWrapper(const oatpp::base::PtrWrapper<String>& other)
      : oatpp::base::PtrWrapper<String>(other)
    {}
    
    PtrWrapper(oatpp::base::PtrWrapper<String>&& other)
      : oatpp::base::PtrWrapper<String>(std::move(other))
    {}
    
    PtrWrapper& operator = (const char* str) {
      m_ptr = String::createFromCString(str);
      return *this;
    }
    
    PtrWrapper& operator = (const oatpp::base::PtrWrapper<String>& other){
      oatpp::base::PtrWrapper<String>::operator=(other);
      return *this;
    }

    PtrWrapper& operator = (oatpp::base::PtrWrapper<String>&& other){
      oatpp::base::PtrWrapper<String>::operator=(std::move(other));
      return *this;
    }
    
    PtrWrapper operator + (const char* str) const{
      return String::createSharedConcatenated(m_ptr.get()->m_data, m_ptr.get()->m_size, str, (v_int32) std::strlen(str));
    }
    
    PtrWrapper operator + (const oatpp::base::PtrWrapper<String>& other) const{
      return String::createSharedConcatenated(m_ptr.get()->m_data, m_ptr.get()->m_size, other.get()->m_data, other.get()->m_size);
    }
    
    static const PtrWrapper& empty(){
      static PtrWrapper empty;
      return empty;
    }
    
  };
  
public:
  OBJECT_POOL_THREAD_LOCAL(String_Pool, String, 32)
private:
  p_char8 m_data;
  v_int32 m_size;
  bool m_hasOwnData;
private:
  
  void set(const void* data, v_int32 size, bool hasOwnData);
  void setAndCopy(const void* data, const void* originData, v_int32 size);
  static std::shared_ptr<String> allocShared(const void* data, v_int32 size, bool copyAsOwnData);

  /**
   *  Allocate memory for string or use originData
   *  if copyAsOwnData == false return originData
   */
  static p_char8 allocString(const void* originData, v_int32 size, bool copyAsOwnData);
  
public:
  String()
    : m_data((p_char8)"[<nullptr>]")
    , m_size(11)
    , m_hasOwnData(false)
  {}
  
  String(const void* data, v_int32 size, bool copyAsOwnData)
    : m_data(allocString(data, size, copyAsOwnData))
    , m_size(size)
    , m_hasOwnData(copyAsOwnData)
  {}
public:
  
  virtual ~String();
  
  static std::shared_ptr<String> createShared(v_int32 size);
  static std::shared_ptr<String> createShared(const void* data, v_int32 size, bool copyAsOwnData = true);
  static std::shared_ptr<String> createShared(const char* data, bool copyAsOwnData = true);
  static std::shared_ptr<String> createShared(String* other, bool copyAsOwnData = true);
  
  static std::shared_ptr<String> createSharedConcatenated(const void* data1, v_int32 size1, const void* data2, v_int32 size2);
  
  static std::shared_ptr<String> createFromCString(const char* data, bool copyAsOwnData = true) {
    if(data != nullptr) {
      return allocShared(data, (v_int32) std::strlen(data), copyAsOwnData);
    }
    return nullptr;
  }
  
  p_char8 getData() const;
  v_int32 getSize() const;
  
  const char* c_str() const;
  std::string std_str() const;
  
  bool hasOwnData() const;
  
  /**
   *  (correct for ACII only)
   */
  std::shared_ptr<String> toLowerCase() const;
  
  /**
   *  (correct for ACII only)
   */
  std::shared_ptr<String> toUpperCase() const;
  
  bool equals(const void* data, v_int32 size) const;
  bool equals(const char* data) const;
  bool equals(String* other) const;
  bool equals(const String::PtrWrapper& other) const;
  bool equals(const std::shared_ptr<String>& other) const;
  
  bool startsWith(const void* data, v_int32 size) const;
  bool startsWith(const char* data) const;
  bool startsWith(String* data) const;
  
public:
  
  static v_int32 compare(const void* data1, const void* data2, v_int32 size);
  static v_int32 compare(String* str1, String* str2);
  
  static bool equals(const void* data1, const void* data2, v_int32 size);
  static bool equals(const char* data1, const char* data2);
  static bool equals(String* str1, String* str2);
  
  // Case Insensitive (correct for ASCII only)
  
  static bool equalsCI(const void* data1, const void* data2, v_int32 size);
  static bool equalsCI(const char* data1, const char* data2);
  static bool equalsCI(String* str1, String* str2);
  
  // Case Insensitive Fast (ASCII only, correct compare if one of strings contains letters only)
  
  static bool equalsCI_FAST(const void* data1, const void* data2, v_int32 size);
  static bool equalsCI_FAST(const char* data1, const char* data2);
  static bool equalsCI_FAST(String* str1, String* str2);
  static bool equalsCI_FAST(const String::PtrWrapper& str1, const char* str2);
  
  /**
   *  lower case chars in the buffer @data (correct for ACII only)
   */
  static void lowerCase(const void* data, v_int32 size);
  
  /**
   *  upper case chars in the buffer @data (correct for ACII only)
   */
  static void upperCase(const void* data, v_int32 size);
  
};
  
oatpp::base::PtrWrapper<String> operator + (const char* a, const oatpp::base::PtrWrapper<String>& b);
oatpp::base::PtrWrapper<String> operator + (const oatpp::base::PtrWrapper<String>& b, const char* a);
oatpp::base::PtrWrapper<String> operator + (const oatpp::base::PtrWrapper<String>& a, const oatpp::base::PtrWrapper<String>& b);

std::shared_ptr<String> operator + (const char* a, const std::shared_ptr<String>& b);
std::shared_ptr<String> operator + (const std::shared_ptr<String>& b, const char* a);
  
}}
  
#endif /* oatpp_base_String_hpp */
