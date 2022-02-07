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

#ifndef oatpp_data_type_Primitive_hpp
#define oatpp_data_type_Primitive_hpp

#include "./Type.hpp"

#include "oatpp/core/base/Countable.hpp"

#include <algorithm>
#include <cctype>
#include <iterator>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class String; // FWD

  class Int8; // FWD
  class UInt8; // FWD

  class Int16; // FWD
  class UInt16; // FWD

  class Int32; // FWD
  class UInt32; // FWD

  class Int64; // FWD
  class UInt64; // FWD

  class Float32; // FWD
  class Float64; // FWD

  class Boolean; // FWD

}

/**
 * Mapping-enables String is &id:type::ObjectWrapper; over `std::string`;
 */
class String : public type::ObjectWrapper<std::string, __class::String> {
public:
  String(const std::shared_ptr<std::string>& ptr, const type::Type* const valueType);
public:

  String() {}

  explicit String(v_buff_size size)
    : type::ObjectWrapper<std::string, __class::String>(std::make_shared<std::string>(size, '\0'))
  {}

  String(const char* data, v_buff_size size)
    : type::ObjectWrapper<std::string, __class::String>(std::make_shared<std::string>(data, size))
  {}

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  String(T) {}

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, char>::value, void>::type
  >
  String(const T* data)
    : type::ObjectWrapper<std::string, __class::String>(
        data == nullptr ? nullptr : std::make_shared<std::string>(data)
      )
  {}

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::string>::value, void>::type
  >
  String(const T& str)
    : type::ObjectWrapper<std::string, __class::String>(std::make_shared<std::string>(str))
  {}

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::string>::value, void>::type
  >
  String(T&& str)
    : type::ObjectWrapper<std::string, __class::String>(
        std::make_shared<std::string>(std::forward<std::string>(str))
      )
  {}

  String(const std::shared_ptr<std::string>& ptr)
    : type::ObjectWrapper<std::string, __class::String>(ptr)
  {}

  String(std::shared_ptr<std::string>&& ptr)
    : type::ObjectWrapper<std::string, __class::String>(std::forward<std::shared_ptr<std::string>>(ptr))
  {}

  String(const String& other)
    : type::ObjectWrapper<std::string, __class::String>(other)
  {}

  String(String&& other)
    : type::ObjectWrapper<std::string, __class::String>(std::forward<String>(other))
  {}

  /**
   * Load data from file and store in &id:oatpp::String;.
   * @param filename - name of the file.
   * @return - &id:oatpp::String;.
   */
  static String loadFromFile(const char* filename);

  /**
   * Save content of the buffer to file.
   * @param filename - name of the file.
   */
  void saveToFile(const char* filename) const;

  const std::string& operator*() const;

  operator std::string() const {
    if (this->m_ptr == nullptr) {
      throw std::runtime_error("[oatpp::data::mapping::type::String::operator std::string() const]: "
                               "Error. Null pointer.");
    }
    return this->m_ptr.operator*();
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline String& operator = (std::nullptr_t) {
    m_ptr.reset();
    return *this;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, char>::value, void>::type
  >
  inline String& operator = (const T* str) {
    if (str) {
      m_ptr = std::make_shared<std::string>(str);
    } else {
      m_ptr.reset();
    }
    return *this;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::string>::value, void>::type
  >
  inline String& operator = (const T& str) {
    m_ptr = std::make_shared<std::string>(str);
    return *this;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::string>::value, void>::type
  >
  inline String& operator = (T&& str) {
    m_ptr = std::make_shared<std::string>(std::forward<std::string>(str));
    return *this;
  }

  inline String& operator = (const String& other){
    m_ptr = other.m_ptr;
    return *this;
  }

  inline String& operator = (String&& other) noexcept {
    m_ptr = std::move(other.m_ptr);
    return *this;
  }

  /**
   * Case insensitive compare (ASCII only).
   * @param other
   * @return
   */
  bool equalsCI_ASCII(const std::string& other);

  /**
   * Case insensitive compare (ASCII only).
   * @param other
   * @return
   */
  bool equalsCI_ASCII(const String& other);

  /**
   * Case insensitive compare (ASCII only).
   * @param other
   * @return
   */
  bool equalsCI_ASCII(const char* str);

  /**
   * Get underlying value.
   * @param defaultValue - value to return in case stored value is `nullptr`.
   * @return - value or `defaultValue` if stored value is `nullptr`.
   */
  std::string getValue(const std::string& defaultValue) const;

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (T) const {
    return m_ptr.get() == nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (T) const {
    return m_ptr.get() != nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, char>::value, void>::type
  >
  inline bool operator == (const T* str) const {
    if(!m_ptr) return str == nullptr;
    if(str == nullptr) return false;
    return *m_ptr == str;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, char>::value, void>::type
  >
  inline bool operator != (const T* str) const {
    return !operator == (str);
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::string>::value, void>::type
  >
  inline bool operator == (const T& str) const {
    if(!m_ptr) return false;
    return *m_ptr == str;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::string>::value, void>::type
  >
  inline bool operator != (const T& str) const {
    return !operator == (str);
  }

  inline bool operator == (const String &other) const {
    if(!m_ptr) return !other.m_ptr;
    if(!other.m_ptr) return false;
    return *m_ptr == *other.m_ptr;
  }

  inline bool operator != (const String &other) const {
    return !operator == (other);
  }

};

String operator + (const char* a, const String& b);
String operator + (const String& a, const char* b);
String operator + (const String& a, const String& b);

/**
 * Template for primitive mapping-enabled types.
 * @tparam TValueType - type of the value ex.: v_int64.
 * @tparam Clazz - Class holding static class information.
 */
template<typename TValueType, class Clazz>
class Primitive : public type::ObjectWrapper<TValueType, Clazz>  {
public:
  typedef TValueType UnderlyingType;
public:

  OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(Primitive, TValueType, Clazz)

  Primitive(TValueType value)
    : type::ObjectWrapper<TValueType, Clazz>(std::make_shared<TValueType>(value))
  {}

  Primitive& operator = (TValueType value) {
    this->m_ptr = std::make_shared<TValueType>(value);
    return *this;
  }

  TValueType operator*() const {
    return this->m_ptr.operator*();
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (T){
    return this->m_ptr.get() == nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (T){
    return this->m_ptr.get() != nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, TValueType>::value, void>::type
  >
  inline bool operator == (T value) const {
    if(!this->m_ptr) return false;
    return *this->m_ptr == value;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, TValueType>::value, void>::type
  >
  inline bool operator != (T value) const {
    if(!this->m_ptr) return true;
    return *this->m_ptr != value;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, Primitive>::value, void>::type
  >
  inline bool operator == (const T &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, Primitive>::value, void>::type
  >
  inline bool operator != (const T &other) const {
    return !operator == (other);
  }

  inline operator TValueType() const {
    return *this->m_ptr;
  }

  TValueType getValue(const TValueType& defaultValue) const {
    if(this->m_ptr) {
      return *this->m_ptr;
    }
    return defaultValue;
  }

};

/**
 * ObjectWrapper for Boolean.
 */
class Boolean : public type::ObjectWrapper<bool, __class::Boolean>  {
public:
  typedef bool UnderlyingType;
public:

  OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(Boolean, bool, __class::Boolean)

  Boolean(bool value)
    : type::ObjectWrapper<bool, __class::Boolean>(std::make_shared<bool>(value))
  {}

  Boolean& operator = (bool value) {
    this->m_ptr = std::make_shared<bool>(value);
    return *this;
  }

  inline bool operator*() const {
    return this->m_ptr.operator*();
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (T){
    return m_ptr.get() == nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (T){
    return m_ptr.get() != nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, bool>::value, void>::type
  >
  inline bool operator == (T value) const {
    if(!this->m_ptr) return false;
    return *this->m_ptr == value;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, bool>::value, void>::type
  >
  inline bool operator != (T value) const {
    if(!this->m_ptr) return true;
    return *this->m_ptr != value;
  }

  inline bool operator == (const Boolean &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  inline bool operator != (const Boolean &other) const {
    return !operator == (other);
  }

  inline operator bool() const {
    if(this->m_ptr) {
      return *(this->m_ptr);
    }
    return false;
  }

  bool getValue(bool defaultValue) const {
    if(this->m_ptr) {
      return *this->m_ptr;
    }
    return defaultValue;
  }

};

/**
 * Int8 is an ObjectWrapper over `v_int8` and __class::Int8.
 */
typedef Primitive<v_int8, __class::Int8> Int8;

/**
 * UInt8 is an ObjectWrapper over `v_uint8` and __class::UInt8.
 */
typedef Primitive<v_uint8, __class::UInt8> UInt8;

/**
 * Int16 is an ObjectWrapper over `v_int16` and __class::Int16.
 */
typedef Primitive<v_int16, __class::Int16> Int16;

/**
 * UInt16 is an ObjectWrapper over `v_uint16` and __class::UInt16.
 */
typedef Primitive<v_uint16, __class::UInt16> UInt16;

/**
 * Int32 is an ObjectWrapper over `v_int32` and __class::Int32.
 */
typedef Primitive<v_int32, __class::Int32> Int32;

/**
 * UInt32 is an ObjectWrapper over `v_uint32` and __class::UInt32.
 */
typedef Primitive<v_uint32, __class::UInt32> UInt32;

/**
 * Int64 is an ObjectWrapper over `v_int64` and __class::Int64.
 */
typedef Primitive<v_int64, __class::Int64> Int64;

/**
 * UInt64 is an ObjectWrapper over `v_uint64` and __class::UInt64.
 */
typedef Primitive<v_uint64, __class::UInt64> UInt64;

/**
 * Float32 is an ObjectWrapper over `v_float32` and __class::Float32.
 */
typedef Primitive<v_float32, __class::Float32> Float32;

/**
 * Float64 is an ObjectWrapper over `v_float64` and __class::Float64.
 */
typedef Primitive<v_float64, __class::Float64> Float64;


template<>
struct ObjectWrapperByUnderlyingType <v_int8> {
  typedef Int8 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_uint8> {
  typedef UInt8 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_int16> {
  typedef Int16 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_uint16> {
  typedef UInt16 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_int32> {
  typedef Int32 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_uint32> {
  typedef UInt32 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_int64> {
  typedef Int64 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <v_uint64> {
  typedef UInt64 ObjectWrapper;
};

template<>
struct ObjectWrapperByUnderlyingType <bool> {
  typedef Boolean ObjectWrapper;
};

namespace __class {

  class String {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Int8 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class UInt8 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Int16 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class UInt16 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Int32 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class UInt32 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Int64 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class UInt64 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Float32 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Float64 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

  class Boolean {
  public:
    static const ClassId CLASS_ID;

    static Type* getType();

  };

}

}}}}

namespace std {

  template<>
  struct hash<oatpp::data::mapping::type::String> {

    typedef oatpp::data::mapping::type::String argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& s) const noexcept {
      if(s.get() == nullptr) return 0;
      return hash<std::string> {} (*s);
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Boolean> {

    typedef oatpp::data::mapping::type::Boolean argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 2;
      return result_type(*v);
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Int8> {

    typedef oatpp::data::mapping::type::Int8 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::UInt8> {

    typedef oatpp::data::mapping::type::UInt8 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Int16> {

    typedef oatpp::data::mapping::type::Int16 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::UInt16> {

    typedef oatpp::data::mapping::type::UInt16 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Int32> {

    typedef oatpp::data::mapping::type::Int32 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::UInt32> {

    typedef oatpp::data::mapping::type::UInt32 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Int64> {

    typedef oatpp::data::mapping::type::Int64 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::UInt64> {

    typedef oatpp::data::mapping::type::UInt64 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return (result_type) *v;
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Float32> {

    typedef oatpp::data::mapping::type::Float32 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return *((v_uint32*) v.get());
    }

  };

  template<>
  struct hash<oatpp::data::mapping::type::Float64> {

    typedef oatpp::data::mapping::type::Float64 argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const& v) const noexcept {
      if(v.get() == nullptr) return 0;
      return *((result_type*) v.get());
    }

  };

}

#endif /* oatpp_base_Countable_PrimitiveDataTypes_hpp */
