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

#include "StrBuffer.hpp"

#include <fstream>

namespace oatpp { namespace base {

void StrBuffer::set(const void* data, v_buff_size size, bool hasOwnData) {
  m_data = (p_char8) data;
  m_size = size;
  m_hasOwnData = hasOwnData;
}

void StrBuffer::setAndCopy(const void* data, const void* originData, v_buff_size size){
  m_data = (p_char8) data;
  m_size = size;
  //m_hasOwnData = false;
  if(originData != nullptr) {
    std::memcpy(m_data, originData, size);
  }
  m_data[size] = 0;
}

std::shared_ptr<StrBuffer> StrBuffer::allocShared(const void* data, v_buff_size size, bool copyAsOwnData) {
  if(copyAsOwnData) {
    memory::AllocationExtras extras(size + 1);
    std::shared_ptr<StrBuffer> ptr;
    if(size + 1 > getSmStringSize()) {
      ptr = memory::allocateSharedWithExtras<StrBuffer>(extras);
    } else {
      ptr = memory::customPoolAllocateSharedWithExtras<StrBuffer>(extras, getSmallStringPool());
    }
    ptr->setAndCopy(extras.extraPtr, data, size);
    return ptr;
  }
  return std::make_shared<StrBuffer>(data, size, copyAsOwnData);
}

p_char8 StrBuffer::allocStrBuffer(const void* originData, v_buff_size size, bool copyAsOwnData) {
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

StrBuffer::StrBuffer()
  : m_data((p_char8)"[<nullptr>]")
  , m_size(11)
  , m_hasOwnData(false)
{}

StrBuffer::StrBuffer(const void* data, v_buff_size size, bool copyAsOwnData)
  : m_data(allocStrBuffer(data, size, copyAsOwnData))
  , m_size(size)
  , m_hasOwnData(copyAsOwnData)
{}

StrBuffer::~StrBuffer() {
  if(m_hasOwnData) {
    delete [] m_data;
  }
  m_data = nullptr;
}

std::shared_ptr<StrBuffer> StrBuffer::createShared(const void* data, v_buff_size size, bool copyAsOwnData) {
  return allocShared(data, size, copyAsOwnData);
}

std::shared_ptr<StrBuffer> StrBuffer::createShared(const char* data, bool copyAsOwnData) {
  return allocShared(data, std::strlen(data), copyAsOwnData);
}

std::shared_ptr<StrBuffer> StrBuffer::createShared(StrBuffer* other, bool copyAsOwnData) {
  return allocShared(other->getData(), other->getSize(), copyAsOwnData);
}

std::shared_ptr<StrBuffer> StrBuffer::createShared(v_buff_size size) {
  return allocShared(nullptr, size, true);
}

std::shared_ptr<StrBuffer> StrBuffer::createSharedConcatenated(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2) {
  const auto& ptr = allocShared(nullptr, size1 + size2, true);
  std::memcpy(ptr->m_data, data1, size1);
  std::memcpy(ptr->m_data + size1, data2, size2);
  return ptr;
}

std::shared_ptr<StrBuffer> StrBuffer::loadFromFile(const char* filename) {
  std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
  if (file.is_open()) {
    auto result = createShared(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char*)result->getData(), result->getSize());
    file.close();
    return result;

  }
  return nullptr;
}

void StrBuffer::saveToFile(const char* filename) {
  std::ofstream fs(filename, std::ios::out | std::ios::binary);
  fs.write((const char*)m_data, m_size);
  fs.close();
}

p_char8 StrBuffer::getData() const {
  return m_data;
}

v_buff_size StrBuffer::getSize() const {
  return m_size;
}

const char* StrBuffer::c_str() const {
  return (const char*) m_data;
}

std::string StrBuffer::std_str() const {
  return std::string((const char*) m_data, m_size);
}

bool StrBuffer::hasOwnData() const {
  return m_hasOwnData;
}

std::shared_ptr<StrBuffer> StrBuffer::toLowerCase() const {
  const auto& ptr = allocShared(m_data, m_size, true);
  lowerCase(ptr->m_data, ptr->m_size);
  return ptr;
}

std::shared_ptr<StrBuffer> StrBuffer::toUpperCase() const {
  const auto& ptr = allocShared(m_data, m_size, true);
  upperCase(ptr->m_data, ptr->m_size);
  return ptr;
}

bool StrBuffer::equals(const void* data, v_buff_size size) const {
  if(m_size == size) {
    return equals(m_data, data, size);
  }
  return false;
}

bool StrBuffer::equals(const char* data) const {
  if(data == nullptr) {
    return m_data == nullptr;
  }
  if(m_size == (v_buff_size) std::strlen(data)) {
    return equals(m_data, data, m_size);
  }
  return false;
}

bool StrBuffer::equals(StrBuffer* other) const {
  return equals((StrBuffer*)this, other);
}

bool StrBuffer::startsWith(const void* data, v_buff_size size) const {
  if(m_size >= size) {
    return equals(m_data, data, size);
  }
  return false;
}

bool StrBuffer::startsWith(const char* data) const {
  if(data == nullptr) return false;
  v_buff_size length = std::strlen(data);
  if(m_size >= length) {
    return equals(m_data, data, length);
  }
  return false;
}

bool StrBuffer::startsWith(StrBuffer* data) const {
  if(data == nullptr) return false;
  if(m_size >= data->m_size) {
    return equals(m_data, data, data->m_size);
  }
  return false;
}

// static

v_buff_size StrBuffer::compare(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2) {

  if(data1 == data2) return 0;
  if(data1 == nullptr) return -1;
  if(data2 == nullptr) return 1;

  if(size1 < size2) {
    auto res = std::memcmp(data1, data2, size1);
    if(res == 0) return -1;
    return res;
  }

  if(size1 > size2) {
    auto res = std::memcmp(data1, data2, size2);
    if(res == 0) return 1;
    return res;
  }

  return std::memcmp(data1, data2, size1);

}

v_buff_size StrBuffer::compareCI(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2) {

  if(data1 == data2) return 0;
  if(data1 == nullptr) return -1;
  if(data2 == nullptr) return 1;

  auto d1 = (p_char8) data1;
  auto d2 = (p_char8) data2;

  v_buff_size size = size1;
  if(size2 < size1) size = size2;

  for(v_buff_size i = 0; i < size; i ++) {

    v_char8 a = d1[i];
    v_char8 b = d2[i];

    if(a >= 'A' && a <= 'Z') a |= 32;
    if(b >= 'A' && b <= 'Z') b |= 32;

    if(a != b) {
      return (int) a - (int) b;
    }

  }

  if(size1 < size2) return -1;
  if(size1 > size2) return  1;

  return 0;

}

v_buff_size StrBuffer::compareCI_FAST(const void* data1, v_buff_size size1, const void* data2, v_buff_size size2) {

  if(data1 == data2) return 0;
  if(data1 == nullptr) return -1;
  if(data2 == nullptr) return 1;

  auto d1 = (p_char8) data1;
  auto d2 = (p_char8) data2;

  v_buff_size size = size1;
  if(size2 < size1) size = size2;

  for(v_buff_size i = 0; i < size; i ++) {

    v_char8 a = d1[i] | 32;
    v_char8 b = d2[i] | 32;

    if(a != b) {
      return (int) a - (int) b;
    }

  }

  if(size1 < size2) return -1;
  if(size1 > size2) return  1;

  return 0;

}

bool StrBuffer::equals(const void* data1, const void* data2, v_buff_size size) {
  if(data1 == data2) return true;
  if(data1 == nullptr || data2 == nullptr) return false;
  return std::memcmp(data1, data2, size) == 0;
}

bool StrBuffer::equals(const char* data1, const char* data2) {
  if(data1 == data2) return true;
  if(data1 == nullptr || data2 == nullptr) return false;
  const auto size = std::strlen(data1);
  return (size == std::strlen(data2) && std::memcmp(data1, data2, size) == 0);
}

bool StrBuffer::equals(StrBuffer* str1, StrBuffer* str2) {
  if(str1 == str2) return true;
  if(str1 == nullptr || str2 == nullptr) return false;
  if(str1->m_size != str2->m_size) return false;
  return str1->m_data == str2->m_data || std::memcmp(str1->m_data, str2->m_data, str1->m_size) == 0;
}

bool StrBuffer::equalsCI(const void* data1, const void* data2, v_buff_size size) {
  for(v_buff_size i = 0; i < size; i++) {
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

bool StrBuffer::equalsCI(const char* data1, const char* data2) {
  if(data1 == data2) return true;
  if(data1 == nullptr || data2 == nullptr) return false;
  const auto size = std::strlen(data1);
  return (size == std::strlen(data2) && equalsCI(data1, data2, size) == 0);
}

bool StrBuffer::equalsCI(StrBuffer* str1, StrBuffer* str2) {
  if(str1 == str2) return true;
  if(str1 == nullptr || str2 == nullptr) return false;
  if(str1->m_size != str2->m_size) return false;
  return (str1->m_data == str2->m_data || equalsCI(str1->m_data, str2->m_data, str1->m_size));
}

bool StrBuffer::equalsCI_FAST(const void* data1, const void* data2, v_buff_size size) {
  for(v_buff_size i = 0; i < size; i++) {
    if((((p_char8) data1) [i] | 32) != (((p_char8) data2) [i] | 32)) {
      return false;
    }
  }
  return true;
}

bool StrBuffer::equalsCI_FAST(const char* data1, const char* data2) {
  if(data1 == data2) return true;
  if(data1 == nullptr || data2 == nullptr) return false;
  const auto size = std::strlen(data1);
  return (size == std::strlen(data2) && equalsCI_FAST(data1, data2, size) == 0);
}

bool StrBuffer::equalsCI_FAST(StrBuffer* str1, StrBuffer* str2) {
  if(str1 == str2) return true;
  if(str1 == nullptr || str2 == nullptr) return false;
  if(str1->m_size != str2->m_size) return false;
  return (str1->m_data == str2->m_data || equalsCI_FAST(str1->m_data, str2->m_data, str1->m_size));
}

bool StrBuffer::equalsCI_FAST(StrBuffer* str1, const char* str2) {
  v_buff_size len = std::strlen(str2);
  return (str1->getSize() == len && equalsCI_FAST(str1->m_data, str2, str1->m_size));
}

void StrBuffer::lowerCase(const void* data, v_buff_size size) {
  for(v_buff_size i = 0; i < size; i++) {
    v_char8 a = ((p_char8) data)[i];
    if(a >= 'A' && a <= 'Z') ((p_char8) data)[i] = a | 32;
  }
}

void StrBuffer::upperCase(const void* data, v_buff_size size) {
  for(v_buff_size i = 0; i < size; i++) {
    v_char8 a = ((p_char8) data)[i];
    if(a >= 'a' && a <= 'z') ((p_char8) data)[i] = a & 223;
  }
}

}}
