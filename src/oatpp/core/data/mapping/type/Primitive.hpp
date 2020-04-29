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

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/StrBuffer.hpp"

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
 * Mapping-enables String is &id:type::ObjectWrapper; over &id:oatpp::base::StrBuffer;
 */
class String : public type::ObjectWrapper<base::StrBuffer, __class::String> {
public:
  typedef String __Wrapper;
public:
  String(const std::shared_ptr<base::StrBuffer>& ptr, const type::Type* const valueType);
public:
  
  String() {}

  String(std::nullptr_t) {}

  explicit String(v_buff_size size)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(base::StrBuffer::createShared(size))
  {}
  
  String(const char* data, v_buff_size size, bool copyAsOwnData = true)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(base::StrBuffer::createShared(data, size, copyAsOwnData))
  {}
  
  String(const char* data1, v_buff_size size1, const char* data2, v_buff_size size2)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(base::StrBuffer::createSharedConcatenated(data1, size1, data2, size2))
  {}
  
  String(const char* data, bool copyAsOwnData = true)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(base::StrBuffer::createFromCString(data, copyAsOwnData))
  {}
  
  String(const std::shared_ptr<base::StrBuffer>& ptr)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(ptr)
  {}
  
  String(std::shared_ptr<base::StrBuffer>&& ptr)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(std::forward<std::shared_ptr<base::StrBuffer>>(ptr))
  {}
  
  String(const String& other)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(other)
  {}
  
  String(String&& other)
    : type::ObjectWrapper<base::StrBuffer, __class::String>(std::forward<String>(other))
  {}

  String& operator = (std::nullptr_t) {
    m_ptr.reset();
    return *this;
  }

  String& operator = (const char* str) {
    m_ptr = base::StrBuffer::createFromCString(str);
    return *this;
  }
  
  String& operator = (const String& other){
    m_ptr = other.m_ptr;
    return *this;
  }
  
  String& operator = (String&& other){
    m_ptr = std::forward<std::shared_ptr<base::StrBuffer>>(other.m_ptr);
    return *this;
  }

  bool operator == (std::nullptr_t) const {
    return m_ptr.get() == nullptr;
  }

  bool operator != (std::nullptr_t) const {
    return m_ptr.get() != nullptr;
  }

  bool operator == (const char* str) const {
    if(!m_ptr) return str == nullptr;
    if(str == nullptr) return false;
    if(m_ptr->getSize() != std::strlen(str)) return false;
    return base::StrBuffer::equals(m_ptr->getData(), str, m_ptr->getSize());
  }

  bool operator != (const char* str) const {
    return !operator == (str);
  }
  
  bool operator == (const String &other) const {
    return base::StrBuffer::equals(m_ptr.get(), other.m_ptr.get());
  }
  
  bool operator != (const String &other) const {
    return !operator == (other);
  }
  
  explicit operator bool() const {
    return m_ptr.operator bool();
  }
  
};
  
String operator + (const char* a, const String& b);
String operator + (const String& b, const char* a);
String operator + (const String& a, const String& b);

/**
 * Template for primitive mapping-enabled types.
 * @tparam TValueType - type of the value ex.: v_int64.
 * @tparam Clazz - Class holding static class information.
 */
template<typename TValueType, class Clazz>
class Primitive : public type::ObjectWrapper<TValueType, Clazz>  {
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

  bool operator == (TValueType value) const {
    if(!this->m_ptr) return false;
    return *this->m_ptr == value;
  }

  bool operator != (TValueType value) const {
    if(!this->m_ptr) return true;
    return *this->m_ptr != value;
  }

  bool operator == (const Primitive &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  bool operator != (const Primitive &other) const {
    return !operator == (other);
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

/**
 * Boolean is an ObjectWrapper over `bool` and __class::Boolean.
 */
typedef Primitive<bool, __class::Boolean> Boolean;
  
namespace __class {
  
  class String {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };
  
  class Int8 {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };

  class UInt8 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }

  };

  class Int16 {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };

  class UInt16 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }

  };
  
  class Int32 {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };

  class UInt32 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }

  };
  
  class Int64 {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };

  class UInt64 {
  public:
    static const ClassId CLASS_ID;

    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }

  };

  class Float32 {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };
  
  class Float64 {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };
  
  class Boolean {
  public:
    static const ClassId CLASS_ID;
    
    static Type* getType(){
      static Type type(CLASS_ID, nullptr);
      return &type;
    }
    
  };
  
}
  
}}}}

namespace std {
  
  template<>
  struct hash<oatpp::data::mapping::type::String> {
    
    typedef oatpp::data::mapping::type::String argument_type;
    typedef v_uint32 result_type;
    
    result_type operator()(argument_type const& s) const noexcept {

      p_char8 data = s->getData();
      result_type result = 0;
      for(v_buff_size i = 0; i < s->getSize(); i++) {
        v_char8 c = data[i] | 32;
        result = (31 * result) + c;
      }

      return result;

    }
    
  };
}

#endif /* oatpp_base_Countable_PrimitiveDataTypes_hpp */
