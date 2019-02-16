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

#ifndef oatpp_base_StrBuffer_hpp
#define oatpp_base_StrBuffer_hpp

#include "memory/ObjectPool.hpp"
#include "./Controllable.hpp"

#include <cstring> // c

namespace oatpp { namespace base {

class StrBuffer : public oatpp::base::Controllable {  
private:

  static constexpr v_int32 SM_STRING_POOL_ENTRY_SIZE = 256;
  
  static oatpp::base::memory::ThreadDistributedMemoryPool* getSmallStringPool() {
    static oatpp::base::memory::ThreadDistributedMemoryPool pool("Small_String_Pool", SM_STRING_POOL_ENTRY_SIZE, 16);
    return &pool;
  }
  
  static v_int32 getSmStringBaseSize() {
    memory::AllocationExtras extras(0);
    auto ptr = memory::customPoolAllocateSharedWithExtras<StrBuffer>(extras, getSmallStringPool());
    return extras.baseSize;
  }
  
  static v_int32 getSmStringSize() {
    static v_int32 size = SM_STRING_POOL_ENTRY_SIZE - getSmStringBaseSize();
    return size;
  }

private:
  p_char8 m_data;
  v_int32 m_size;
  bool m_hasOwnData;
private:
  
  void set(const void* data, v_int32 size, bool hasOwnData);
  void setAndCopy(const void* data, const void* originData, v_int32 size);
  static std::shared_ptr<StrBuffer> allocShared(const void* data, v_int32 size, bool copyAsOwnData);

  /**
   *  Allocate memory for string or use originData
   *  if copyAsOwnData == false return originData
   */
  static p_char8 allocStrBuffer(const void* originData, v_int32 size, bool copyAsOwnData);
  
public:
  StrBuffer()
    : m_data((p_char8)"[<nullptr>]")
    , m_size(11)
    , m_hasOwnData(false)
  {}
  
  StrBuffer(const void* data, v_int32 size, bool copyAsOwnData)
    : m_data(allocStrBuffer(data, size, copyAsOwnData))
    , m_size(size)
    , m_hasOwnData(copyAsOwnData)
  {}
public:
  
  virtual ~StrBuffer();
  
  static std::shared_ptr<StrBuffer> createShared(v_int32 size);
  static std::shared_ptr<StrBuffer> createShared(const void* data, v_int32 size, bool copyAsOwnData = true);
  static std::shared_ptr<StrBuffer> createShared(const char* data, bool copyAsOwnData = true);
  static std::shared_ptr<StrBuffer> createShared(StrBuffer* other, bool copyAsOwnData = true);
  
  static std::shared_ptr<StrBuffer> createSharedConcatenated(const void* data1, v_int32 size1, const void* data2, v_int32 size2);
  
  static std::shared_ptr<StrBuffer> createFromCString(const char* data, bool copyAsOwnData = true) {
    if(data != nullptr) {
      return allocShared(data, (v_int32) std::strlen(data), copyAsOwnData);
    }
    return nullptr;
  }
  
  /**
   * Load data from file and store in StrBuffer.
   * If file not found return nullptr
   */
  static std::shared_ptr<StrBuffer> loadFromFile(const char* filename);
  
  void saveToFile(const char* filename);
  
  p_char8 getData() const;
  v_int32 getSize() const;
  
  const char* c_str() const;
  std::string std_str() const;
  
  bool hasOwnData() const;
  
  /**
   *  (correct for ACII only)
   */
  std::shared_ptr<StrBuffer> toLowerCase() const;
  
  /**
   *  (correct for ACII only)
   */
  std::shared_ptr<StrBuffer> toUpperCase() const;
  
  bool equals(const void* data, v_int32 size) const;
  bool equals(const char* data) const;
  bool equals(StrBuffer* other) const;
  
  bool startsWith(const void* data, v_int32 size) const;
  bool startsWith(const char* data) const;
  bool startsWith(StrBuffer* data) const;
  
public:
  
  static v_int32 compare(const void* data1, const void* data2, v_int32 size);
  static v_int32 compare(StrBuffer* str1, StrBuffer* str2);
  
  static bool equals(const void* data1, const void* data2, v_int32 size);
  static bool equals(const char* data1, const char* data2);
  static bool equals(StrBuffer* str1, StrBuffer* str2);
  
  // Case Insensitive (correct for ASCII only)
  
  static bool equalsCI(const void* data1, const void* data2, v_int32 size);
  static bool equalsCI(const char* data1, const char* data2);
  static bool equalsCI(StrBuffer* str1, StrBuffer* str2);
  
  // Case Insensitive Fast (ASCII only, correct compare if one of strings contains letters only)
  
  static bool equalsCI_FAST(const void* data1, const void* data2, v_int32 size);
  static bool equalsCI_FAST(const char* data1, const char* data2);
  static bool equalsCI_FAST(StrBuffer* str1, StrBuffer* str2);
  static bool equalsCI_FAST(StrBuffer* str1, const char* str2);
  
  /**
   *  lower case chars in the buffer @data (correct for ACII only)
   */
  static void lowerCase(const void* data, v_int32 size);
  
  /**
   *  upper case chars in the buffer @data (correct for ACII only)
   */
  static void upperCase(const void* data, v_int32 size);
  
};
  
}}
  
#endif /* oatpp_base_StrBuffer_hpp */
