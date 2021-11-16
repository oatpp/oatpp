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

#ifndef oatpp_data_share_MemoryLabel_hpp
#define oatpp_data_share_MemoryLabel_hpp

#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/utils/String.hpp"

#include <cstring>
#include <memory>

namespace oatpp { namespace data { namespace share {
  
/**
 * MemoryLabel represent a part of the whole memory buffer refered by handle.
 * Advantage of MemoryLabel use is that you may just "label" some data instead of allocating buffer for it's copy.
 * You may allocate separate buffer for data copy later once you need it.
 */
class MemoryLabel {
public:
  typedef oatpp::data::mapping::type::String String;
protected:
  mutable std::shared_ptr<std::string> m_memoryHandle;
  mutable const void* m_data;
  v_buff_size m_size;
public:

  /**
   * Default constructor. Null MemoryLabel.
   */
  MemoryLabel()
    : m_memoryHandle(nullptr)
    , m_data(nullptr)
    , m_size(0)
  {}

  /**
   * nullptr constructor.
   */
  MemoryLabel(std::nullptr_t)
    : m_memoryHandle(nullptr)
    , m_data(nullptr)
    , m_size(0)
  {}

  /**
   * Constructor.
   * @param ptr
   */
  MemoryLabel(const std::shared_ptr<std::string>& ptr) :
    MemoryLabel(
      ptr,
      ptr ? ptr->data() : nullptr,
      ptr ? (v_buff_size) ptr->size() :  0
    )
  {}

  /**
   * Constructor.
   * @param memHandle - memory handle. `std::shared_ptr` to buffer pointed by a memory label.
   * @param data - pointer to data.
   * @param size - size of the data in bytes.
   */
  MemoryLabel(const std::shared_ptr<std::string>& memHandle, const void* data, v_buff_size size);

  /**
   * Get pointer to labeled data.
   * @return - pointer to data.
   */
  const void* getData() const {
    return m_data;
  }

  /**
   * Get data size.
   * @return - size of the data.
   */
  v_buff_size getSize() const {
    return m_size;
  }

  /**
   * Get memory handle which this memory label holds.
   * @return - `std::shared_ptr` to `std::string`.
   */
  std::shared_ptr<std::string> getMemoryHandle() const {
    return m_memoryHandle;
  }

  /**
   * Capture data referenced by memory label to its own memory.
   */
  void captureToOwnMemory() const {
    if(!m_memoryHandle || m_memoryHandle->data() != (const char*)m_data || m_memoryHandle->size() != m_size) {
      m_memoryHandle = std::make_shared<std::string>((const char*) m_data, m_size);
      m_data = (p_char8) m_memoryHandle->data();
    }
  }

  /**
   * Check if labeled data equals to data specified.
   * Data is compared using &id:oatpp::urils::String::compare;.
   * @param data - data to compare with labeled data.
   * @return - `true` if equals.
   */
  bool equals(const char* data) const {
    auto len = data != nullptr ? std::strlen(data) : 0;
    return utils::String::compare(m_data, m_size, data, len) == 0;
  }

  /**
   * Check if labeled data equals to data specified.
   * Data is compared using &id:oatpp::urils::String::compare;.
   * @param data - data to compare with labeled data.
   * @param size - data size.
   * @return - `true` if equals.
   */
  bool equals(const void* data, v_buff_size size) const {
    return utils::String::compare(m_data, m_size, data, size) == 0;
  }

  /**
   * Create oatpp::String from memory label
   * @return oatpp::String(data, size)
   */
  String toString() const {
    return String((const char*) m_data, m_size);
  }

  /**
   * Create std::string from memory label
   * @return std::string(data, size)
   */
  std::string std_str() const {
    return std::string((const char*) m_data, m_size);
  }

  inline bool operator==(std::nullptr_t) const {
    return m_data == nullptr;
  }

  inline bool operator!=(std::nullptr_t) const {
    return m_data != nullptr;
  }

  inline explicit operator bool() const {
    return m_data != nullptr;
  }
  
};

/**
 * MemoryLabel which can be used as a key in unordered_map
 */
class StringKeyLabel : public MemoryLabel {
public:
  
  StringKeyLabel() : MemoryLabel() {};

  StringKeyLabel(std::nullptr_t) : MemoryLabel() {}

  /**
   * Constructor.
   * @param ptr
   */
  StringKeyLabel(const std::shared_ptr<std::string>& ptr) : MemoryLabel(ptr) {}

  StringKeyLabel(const std::shared_ptr<std::string>& memoryHandle, const char* data, v_buff_size size);
  StringKeyLabel(const char* constText);
  StringKeyLabel(const String& str);

  inline bool operator==(std::nullptr_t) const {
    return m_data == nullptr;
  }

  inline bool operator!=(std::nullptr_t) const {
    return m_data != nullptr;
  }

  inline bool operator==(const char* str) const {
    return equals(str);
  }

  inline bool operator!=(const char* str) const {
    return !operator==(str);
  }

  inline bool operator==(const String& str) const {
    if(m_data == nullptr) return str == nullptr;
    if(str == nullptr) return false;
    return equals(str->data(), str->size());
  }

  inline bool operator!=(const String& str) const {
    return !operator==(str);
  }

  inline bool operator==(const StringKeyLabel &other) const {
    return utils::String::compare(m_data, m_size, other.m_data, other.m_size) == 0;
  }

  inline bool operator!=(const StringKeyLabel &other) const {
    return !operator==(other);
  }

  inline bool operator < (const StringKeyLabel &other) const {
    return utils::String::compare(m_data, m_size, other.m_data, other.m_size) < 0;
  }

  inline bool operator > (const StringKeyLabel &other) const {
    return utils::String::compare(m_data, m_size, other.m_data, other.m_size) > 0;
  }

};

/**
 * MemoryLabel which can be used as a case-insensitive key in unordered_map
 */
class StringKeyLabelCI : public MemoryLabel {
public:
  
  StringKeyLabelCI() : MemoryLabel() {};

  StringKeyLabelCI(std::nullptr_t) : MemoryLabel() {}

  StringKeyLabelCI(const std::shared_ptr<std::string>& ptr) : MemoryLabel(ptr) {}

  StringKeyLabelCI(const std::shared_ptr<std::string>& memoryHandle, const char* data, v_buff_size size);
  StringKeyLabelCI(const char* constText);
  StringKeyLabelCI(const String& str);

  inline bool operator==(std::nullptr_t) const {
    return m_data == nullptr;
  }

  inline bool operator!=(std::nullptr_t) const {
    return m_data != nullptr;
  }

  inline bool operator==(const char* str) const {
    auto len = str != nullptr ? std::strlen(str) : 0;
    return utils::String::compareCI_ASCII(m_data, m_size, str, len) == 0;
  }

  inline bool operator!=(const char* str) const {
    return !operator==(str);
  }

  inline bool operator==(const String& str) const {
    if(m_data == nullptr) return str == nullptr;
    if(str == nullptr) return false;
    return utils::String::compareCI_ASCII(m_data, m_size, str->data(), str->size()) == 0;
  }

  inline bool operator!=(const String& str) const {
    return !operator==(str);
  }

  inline bool operator==(const StringKeyLabelCI &other) const {
    return utils::String::compareCI_ASCII(m_data, m_size, other.m_data, other.m_size) == 0;
  }

  inline bool operator!=(const StringKeyLabelCI &other) const {
    return !operator==(other);
  }

  inline bool operator < (const StringKeyLabelCI &other) const {
    return utils::String::compareCI_ASCII(m_data, m_size, other.m_data, other.m_size) < 0;
  }

  inline bool operator > (const StringKeyLabelCI &other) const {
    return utils::String::compareCI_ASCII(m_data, m_size, other.m_data, other.m_size) > 0;
  }

};
  
}}}

namespace std {
  
  template<>
  struct hash<oatpp::data::share::StringKeyLabel> {
    
    typedef oatpp::data::share::StringKeyLabel argument_type;
    typedef v_uint64 result_type;
    
    result_type operator()(oatpp::data::share::StringKeyLabel const& s) const noexcept {

      auto data = (p_char8) s.getData();
      result_type result = 0;
      for(v_buff_size i = 0; i < s.getSize(); i++) {
        v_char8 c = data[i];
        result = (31 * result) + c;
      }

      return result;

    }
  };
  
  template<>
  struct hash<oatpp::data::share::StringKeyLabelCI> {
    
    typedef oatpp::data::share::StringKeyLabelCI argument_type;
    typedef v_uint64 result_type;
    
    result_type operator()(oatpp::data::share::StringKeyLabelCI const& s) const noexcept {

      auto data = (p_char8) s.getData();
      result_type result = 0;
      for(v_buff_size i = 0; i < s.getSize(); i++) {
        v_char8 c = data[i] | 32;
        result = (31 * result) + c;
      }

      return result;

    }
  };
  
}

#endif /* oatpp_data_share_MemoryLabel_hpp */
