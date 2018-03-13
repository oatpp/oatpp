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

#include "String.hpp"

namespace oatpp { namespace base {
  
void String::setAndCopy(const void* data, const void* originData, v_int32 size){
  m_data = (p_char8) data;
  m_size = size;
  //m_hasOwnData = false;
  if(originData != nullptr) {
    std::memcpy(m_data, originData, size);
  }
  m_data[size] = 0;
}

std::shared_ptr<String> String::allocShared(const void* data, v_int32 size, bool copyAsOwnData) {
  if(copyAsOwnData) {
    memory::AllocationExtras extras(size + 1);
    const auto& ptr = memory::allocateSharedWithExtras<String>(extras);
    ptr->setAndCopy(extras.extraPtr, data, size);
    return ptr;
  }
  return std::make_shared<String>(data, size, copyAsOwnData);
}
  
p_char8 String::allocString(const void* originData, v_int32 size, bool copyAsOwnData) {
  if(copyAsOwnData) {
    p_char8 data = new v_char8[size + 1];
    data[size] = 0;
    if(originData != nullptr) {
      std::memcpy(data, originData, size);
    }
    return data;
  }
  return (p_char8) originData;
}
  
String::~String() {
  if(m_hasOwnData) {
    delete [] m_data;
  }
  m_data = nullptr;
}
  
std::shared_ptr<String> String::createShared(const void* data, v_int32 size, bool copyAsOwnData) {
  return allocShared(data, size, copyAsOwnData);
}
  
std::shared_ptr<String> String::createShared(const char* data, bool copyAsOwnData) {
  return allocShared(data, (v_int32) std::strlen(data), copyAsOwnData);
}

std::shared_ptr<String> String::createShared(String* other, bool copyAsOwnData) {
  return allocShared(other->getData(), other->getSize(), copyAsOwnData);
}

std::shared_ptr<String> String::createShared(v_int32 size) {
  return allocShared(nullptr, size, true);
}

std::shared_ptr<String> String::createSharedConcatenated(const void* data1, v_int32 size1, const void* data2, v_int32 size2) {
  const auto& ptr = allocShared(nullptr, size1 + size2, true);
  std::memcpy(ptr->m_data, data1, size1);
  std::memcpy(ptr->m_data + size1, data2, size2);
  return ptr;
}
  
p_char8 String::getData() const {
  return m_data;
}

v_int32 String::getSize() const {
  return m_size;
}

const char* String::c_str() const {
  return (const char*) m_data;
}
  
bool String::hasOwnData() const {
  return m_hasOwnData;
}
  
std::shared_ptr<String> String::toLowerCase() const {
  return createShared(allocateAndLowerCase(m_data, m_size), m_size, true);
}

std::shared_ptr<String> String::toUpperCase() const {
  return createShared(allocateAndUpperCase(m_data, m_size), m_size, true);
}
  
bool String::equals(const void* data, v_int32 size) const {
  if(m_size == size) {
    return equals(m_data, data, size);
  }
  return false;
}

bool String::equals(const char* data) const {
  if(m_size == (v_int32) std::strlen(data)) {
    return equals(m_data, data, m_size);
  }
  return false;
}

bool String::equals(String* other) const {
  return equals((String*) this, other);
}
  
bool String::equals(const String::SharedWrapper& other) const {
  return equals((String*) this, other.get());
}

bool String::equals(const std::shared_ptr<String>& other) const {
  return equals((String*) this, other.get());
}

bool String::startsWith(const void* data, v_int32 size) const {
  if(m_size >= size) {
    return equals(m_data, data, size);
  }
  return false;
}

bool String::startsWith(const char* data) const {
  v_int32 length = (v_int32) std::strlen(data);
  if(m_size >= length) {
    return equals(m_data, data, length);
  }
  return false;
}

bool String::startsWith(String* data) const {
  if(m_size >= data->m_size) {
    return equals(m_data, data, data->m_size);
  }
  return false;
}
  
// static
  
v_int32 String::compare(const void* data1, const void* data2, v_int32 size) {
  return std::memcmp(data1, data2, size);
}

v_int32 String::compare(String* str1, String* str2) {
  if(str1 == str2) {
    return 0;
  }
  if(str1->m_size < str2->m_size) {
    return compare(str1->m_data, str2->m_data, str1->m_size);
  } else {
    return compare(str1->m_data, str2->m_data, str2->m_size);
  }
}

bool String::equals(const void* data1, const void* data2, v_int32 size) {
  return (data1 == data2) || (std::memcmp(data1, data2, size) == 0);
}
  
bool String::equals(const char* data1, const char* data2) {
  if(data1 == data2) return true;
  if(data1 == nullptr && data2 == nullptr) return false;
  v_int32 size = (v_int32) std::strlen(data1);
  return (size == (v_int32) std::strlen(data2) && std::memcmp(data1, data2, size) == 0);
}
  
bool String::equals(String* str1, String* str2) {
  return  (str1 == str2) ||
          (str1 != nullptr && str2 != nullptr && str1->m_size == str2->m_size &&
            (str1->m_data == str2->m_data || std::memcmp(str1->m_data, str2->m_data, str1->m_size) == 0)
          );
}

bool String::equalsCI(const void* data1, const void* data2, v_int32 size) {
  for(v_int32 i = 0; i < size; i++) {
    v_char8 a = ((p_char8) data1) [i];
    v_char8 b = ((p_char8) data2) [i];
    if(a >= 'A' && a <= 'Z') a |= 32;
    if(b >= 'A' && b <= 'Z') b |= 32;
    if(a != b) {
      return false;
    }
  }
  return true;
}
  
bool String::equalsCI(const char* data1, const char* data2) {
  if(data1 == data2) return true;
  if(data1 == nullptr && data2 == nullptr) return false;
  v_int32 size = (v_int32) std::strlen(data1);
  return (size == (v_int32) std::strlen(data2) && equalsCI(data1, data2, size) == 0);
}
  
bool String::equalsCI(String* str1, String* str2) {
  return  (str1 == str2) ||
          (str1 != nullptr && str2 != nullptr && str1->m_size == str2->m_size &&
            (str1->m_data == str2->m_data || equalsCI(str1->m_data, str2->m_data, str1->m_size))
          );
}

bool String::equalsCI_FAST(const void* data1, const void* data2, v_int32 size) {
  for(v_int32 i = 0; i < size; i++) {
    if((((p_char8) data1) [i] | 32) != (((p_char8) data2) [i] | 32)) {
      return false;
    }
  }
  return true;
}
  
bool String::equalsCI_FAST(const char* data1, const char* data2) {
  if(data1 == data2) return true;
  if(data1 == nullptr && data2 == nullptr) return false;
  v_int32 size = (v_int32) std::strlen(data1);
  return (size == (v_int32) std::strlen(data2) && equalsCI_FAST(data1, data2, size) == 0);
}

bool String::equalsCI_FAST(String* str1, String* str2) {
  return  (str1 == str2) ||
  (str1 != nullptr && str2 != nullptr && str1->m_size == str2->m_size &&
   (str1->m_data == str2->m_data || equalsCI_FAST(str1->m_data, str2->m_data, str1->m_size))
   );
}
  
bool String::equalsCI_FAST(const String::SharedWrapper& str1, const char* str2) {
  v_int32 len = (v_int32) std::strlen(str2);
  return (str1->getSize() == len && equalsCI_FAST(str1->m_data, str2, str1->m_size));
}

p_char8 String::allocateAndLowerCase(const void* originData, v_int32 size) {
  p_char8 result = new v_char8[size + 1];
  for(v_int32 i = 0; i < size; i++) {
    v_char8 a = ((p_char8)originData)[i];
    if(a >= 'A' && a <= 'Z') a |= 32;
    result[i] = a;
  }
  result[size] = 0;
  return result;
}

p_char8 String::allocateAndUpperCase(const void* originData, v_int32 size) {
  p_char8 result = new v_char8[size + 1];
  for(v_int32 i = 0; i < size; i++) {
    v_char8 a = ((p_char8)originData)[i];
    if(a >= 'a' && a <= 'z') a &= 223;
    result[i] = a;
  }
  result[size] = 0;
  return result;
}
  
// Operator
  
oatpp::base::SharedWrapper<String> operator + (const char* a, const oatpp::base::SharedWrapper<String>& b){
  return String::createSharedConcatenated(a, (v_int32) std::strlen(a), b->getData(), b->getSize());
}
  
oatpp::base::SharedWrapper<String> operator + (const oatpp::base::SharedWrapper<String>& b, const char* a){
  return String::createSharedConcatenated(b->getData(), b->getSize(), a, (v_int32) std::strlen(a));
}
  
std::shared_ptr<String> operator + (const char* a, const std::shared_ptr<String>& b){
  return String::createSharedConcatenated(a, (v_int32) std::strlen(a), b->getData(), b->getSize());
}
  
std::shared_ptr<String> operator + (const std::shared_ptr<String>& b, const char* a){
  return String::createSharedConcatenated(b->getData(), b->getSize(), a, (v_int32) std::strlen(a));
}
  
}}
