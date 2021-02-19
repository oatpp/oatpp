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
#include "./Countable.hpp"

#include <cstring>

namespace oatpp { namespace base {

/**
 * String buffer class.
 */
class StrBuffer : public oatpp::base::Countable {  
private:

  static constexpr v_buff_size SM_STRING_POOL_ENTRY_SIZE = 256;
  
  static memory::ThreadDistributedMemoryPool& getSmallStringPool() {
    static auto pool = new memory::ThreadDistributedMemoryPool("Small_String_Pool", SM_STRING_POOL_ENTRY_SIZE, 16);
    return *pool;
  }
  
  static v_buff_size getSmStringBaseSize() {
    memory::AllocationExtras extras(0);
    auto ptr = memory::customPoolAllocateSharedWithExtras<StrBuffer>(extras, getSmallStringPool());
    return extras.baseSize;
  }
  
  static v_buff_size getSmStringSize() {
    static v_buff_size size = SM_STRING_POOL_ENTRY_SIZE - getSmStringBaseSize();
    return size;
  }

private:
  p_char8 m_data;
  v_buff_size m_size;
  bool m_hasOwnData;
private:
  
  void set(const void* data, v_buff_size size, bool hasOwnData);
  void setAndCopy(const void* data, const void* originData, v_buff_size size);
  static std::shared_ptr<StrBuffer> allocShared(const void* data, v_buff_size size, bool copyAsOwnData);

  /*
   * Allocate memory for string or use originData<br>
   * if copyAsOwnData == false return originData
   */
  static p_char8 allocStrBuffer(const void* originData, v_buff_size size, bool copyAsOwnData);
  
public:
  /**
   * Constructor. Default.
   */
  StrBuffer();

  /**
   * Constructor.
   * @param data - pointer to data.
   * @param size - size of the data.
   * @param copyAsOwnData - if true then allocate own buffer and copy data to that buffer.
   */
  StrBuffer(const void* data, v_buff_size size, bool copyAsOwnData);
public:

  /**
   * virtual Destructor.
   */
  virtual ~StrBuffer();

  /**
   * Create shared StrBuffer of specified size.
   * @param size - size of the buffer.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> createShared(v_buff_size size);

  /**
   * Create shared StrBuffer with data, size, and copyAsOwnData parameters.
   * @param data - buffer data.
   * @param size - size of the data.
   * @param copyAsOwnData - if true then allocate own buffer and copy data to that buffer.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> createShared(const void* data, v_buff_size size, bool copyAsOwnData = true);

  /**
   * Create shared StrBuffer with data, and copyAsOwnData parameters.
   * @param data - buffer data.
   * @param copyAsOwnData - if true then allocate own buffer and copy data to that buffer.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> createShared(const char* data, bool copyAsOwnData = true);

  /**
   * Create shared StrBuffer from other StrBuffer.
   * @param other - other StrBuffer.
   * @param copyAsOwnData - if true then allocate own buffer and copy data to that buffer.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> createShared(StrBuffer* other, bool copyAsOwnData = true);

  /**
   * Create shared StrBuffer of size=size1 + size2 and data=data1 + data2.
   * @param data1 - pointer to data1.
   * @param size1 - size of the data1.
   * @param data2 - pointer to data2.
   * @param size2 - size of the data2.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> createSharedConcatenated(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2);

  /**
   * Create shared StrBuffer from c-string.
   * @param data - data.
   * @param copyAsOwnData - if true then allocate own buffer and copy data to that buffer.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> createFromCString(const char* data, bool copyAsOwnData = true) {
    if(data != nullptr) {
      return allocShared(data, std::strlen(data), copyAsOwnData);
    }
    return nullptr;
  }

  /**
   * Load data from file and store in StrBuffer.
   * @param filename - name of the file.
   * @return - shared_ptr to StrBuffer.
   */
  static std::shared_ptr<StrBuffer> loadFromFile(const char* filename);

  /**
   * Save content of the buffer to file.
   * @param filename - name of the file.
   */
  void saveToFile(const char* filename);

  /**
   * Get pointer to data of the buffer.
   * @return - pointer to data of the buffer.
   */
  p_char8 getData() const;

  /**
   * Get buffer size.
   * @return - buffer size.
   */
  v_buff_size getSize() const;

  /**
   * Get pointer to data of the buffer as `const* char`.
   * @return - pointer to data of the buffer.
   */
  const char* c_str() const;

  /**
   * Get copy of the buffer data as `std::string`.
   * @return - copy of the buffer data as `std::string`.
   */
  std::string std_str() const;

  /**
   * Is this object is responsible for freeing buffer data.
   * @return - true if this object is responsible for freeing buffer data.
   */
  bool hasOwnData() const;

  /**
   * Create lowercase copy of the buffer.<br>
   * (correct for ASCII only)
   * @return - copy of the buffer containing lowercase variants of ascii symbols.
   */
  std::shared_ptr<StrBuffer> toLowerCase() const;

  /**
   * Create uppercase copy of the buffer.<br>
   * (correct for ASCII only)
   * @return - copy of the buffer containing uppercase variants of ascii symbols.
   */
  std::shared_ptr<StrBuffer> toUpperCase() const;

  /**
   * Check string equality of the buffer to data of specified size.
   * @param data - pointer to data to be compared with the buffer data.
   * @param size - size of the data.
   * @return - true if all chars of buffer are same as in data, and size == this.getSize().
   */
  bool equals(const void* data, v_buff_size size) const;

  /**
   * Check string equality of the buffer to data of specified size.
   * @param data - pointer to data to be compared with the buffer data.
   * @return - true if all chars of buffer are same as in data, and std::strlen(data) == this.getSize().
   */
  bool equals(const char* data) const;

  /**
   * Check string equality to other buffer.
   * @param other - pointer to other StrBuffer to be compared with the buffer data.
   * @return - true if all chars of one buffer are same as in other, and other.getSize() == this.getSize().
   */
  bool equals(StrBuffer* other) const;

  /**
   * Check if buffer starts with specified data, size.
   * @param data - data as `const void*`.
   * @param size - size of the data.
   * @return - true if buffer starts with specified data.
   */
  bool startsWith(const void* data, v_buff_size size) const;

  /**
   * Check if buffer starts with specified data.
   * @param data - data as `const char*`.
   * @return - true if buffer starts with specified data.
   */
  bool startsWith(const char* data) const;

  /**
   * Check if buffer starts with specified data.
   * @param data - data as `StrBuffer`.
   * @return - true if buffer starts with specified data.
   */
  bool startsWith(StrBuffer* data) const;


public:

  /**
   * Compare data1, data2 using `std::memcmp`.
   * @param data1 - pointer to data1.
   * @param size1 - size of data1.
   * @param data2 - pointer to data2.
   * @param size2 - size of data2.
   * @return - Negative value if the first differing byte (reinterpreted as unsigned char) in data1 is less than the corresponding byte in data2.<br>
   * 0 if all count bytes of data1 and data2 are equal.<br>
   * Positive value if the first differing byte in data1 is greater than the corresponding byte in data2.
   */
  static v_buff_size compare(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2);

  /**
   * Compare data1, data2 - case insensitive.
   * @param data1 - pointer to data1.
   * @param size1 - size of data1.
   * @param data2 - pointer to data2.
   * @param size2 - size of data2.
   * @return - Negative value if the first differing byte (reinterpreted as unsigned char) in data1 is less than the corresponding byte in data2.<br>
   * 0 if all count bytes of data1 and data2 are equal.<br>
   * Positive value if the first differing byte in data1 is greater than the corresponding byte in data2.
   */
  static v_buff_size compareCI(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2);

  /**
   * Compare data1, data2 - case insensitive (ASCII only, correct compare if one of strings contains letters only).
   * @param data1 - pointer to data1.
   * @param size1 - size of data1.
   * @param data2 - pointer to data2.
   * @param size2 - size of data2.s
   * @return - Negative value if the first differing byte (reinterpreted as unsigned char) in data1 is less than the corresponding byte in data2.<br>
   * 0 if all count bytes of data1 and data2 are equal.<br>
   * Positive value if the first differing byte in data1 is greater than the corresponding byte in data2.
   */
  static v_buff_size compareCI_FAST(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2);

  /**
   * Check string equality of data1 to data2.
   * @param data1 - pointer to data1.
   * @param data2 - pointer to data2.
   * @param size - number of characters to compare.
   * @return - `true` if equals.
   */
  static bool equals(const void* data1, const void* data2, v_buff_size size);

  /**
   * Check string equality of data1 to data2.
   * @param data1 - pointer to data1.
   * @param data2 - pointer to data2.
   * @return - `true` if equals.
   */
  static bool equals(const char* data1, const char* data2);

  /**
   * Check string equality of str1 to str2.
   * @param str1 - pointer to str1.
   * @param str2 - pointer to str2.
   * @return - `true` if equals.
   */
  static bool equals(StrBuffer* str1, StrBuffer* str2);

  /**
   * Check Case Insensitive string equality of data1 to data2.
   * @param data1 - pointer to data1.
   * @param data2 - pointer to data2.
   * @param size - number of characters to compare.
   * @return - `true` if equals.
   */
  static bool equalsCI(const void* data1, const void* data2, v_buff_size size);

  /**
   * Check Case Insensitive string equality of data1 to data2.
   * @param data1 - pointer to data1.
   * @param data2 - pointer to data2.
   * @return - `true` if equals.
   */
  static bool equalsCI(const char* data1, const char* data2);

  /**
   * Check Case Insensitive string equality of str1 to str2.
   * @param str1 - pointer to str1.
   * @param str2 - pointer to str2.
   * @return - `true` if equals.
   */
  static bool equalsCI(StrBuffer* str1, StrBuffer* str2);

  /**
   * Check Case Insensitive string equality of data1 to data2. (ASCII only, correct compare if one of strings contains letters only)
   * @param data1 - pointer to data1.
   * @param data2 - pointer to data2.
   * @param size - number of characters to compare.
   * @return - `true` if equals.
   */
  static bool equalsCI_FAST(const void* data1, const void* data2, v_buff_size size);

  /**
   * Check Case Insensitive string equality of data1 to data2. (ASCII only, correct compare if one of strings contains letters only)
   * @param data1 - pointer to data1.
   * @param data2 - pointer to data2.
   * @return - `true` if equals.
   */
  static bool equalsCI_FAST(const char* data1, const char* data2);

  /**
   * Check Case Insensitive string equality of str1 to str2. (ASCII only, correct compare if one of strings contains letters only)
   * @param str1 - pointer to str1.
   * @param str2 - pointer to str2.
   * @return - `true` if equals.
   */
  static bool equalsCI_FAST(StrBuffer* str1, StrBuffer* str2);

  /**
   * Check Case Insensitive string equality of str1 to str2. (ASCII only, correct compare if one of strings contains letters only)
   * @param str1 - pointer to str1 as `StrBuffer`.
   * @param str2 - pointer to str2 as `const char*`
   * @return - `true` if equals.
   */
  static bool equalsCI_FAST(StrBuffer* str1, const char* str2);

  /**
   * Change characters in data to lowercase.
   * @param data - pointer to data.
   * @param size - size of the data.
   */
  static void lowerCase(const void* data, v_buff_size size);

  /**
   * Change characters in data to uppercase.
   * @param data - pointer to data.
   * @param size - size of the data.
   */
  static void upperCase(const void* data, v_buff_size size);
  
};
  
}}
  
#endif /* oatpp_base_StrBuffer_hpp */
