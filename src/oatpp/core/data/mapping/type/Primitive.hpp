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
class String : public type::ObjectWrapper<oatpp::base::StrBuffer, __class::String> {
public:
  String(const std::shared_ptr<oatpp::base::StrBuffer>& ptr, const type::Type* const valueType);
public:
  
  String() {}
  
  String(v_buff_size size)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createShared(size))
  {}
  
  String(const char* data, v_buff_size size, bool copyAsOwnData = true)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createShared(data, size, copyAsOwnData))
  {}
  
  String(const char* data1, v_buff_size size1, const char* data2, v_buff_size size2)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createSharedConcatenated(data1, size1, data2, size2))
  {}
  
  String(const char* data, bool copyAsOwnData = true)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createFromCString(data, copyAsOwnData))
  {}
  
  String(const std::shared_ptr<oatpp::base::StrBuffer>& ptr)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(ptr)
  {}
  
  String(std::shared_ptr<oatpp::base::StrBuffer>&& ptr)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(std::forward<std::shared_ptr<oatpp::base::StrBuffer>>(ptr))
  {}
  
  String(const String& other)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(other)
  {}
  
  String(String&& other)
    : type::ObjectWrapper<oatpp::base::StrBuffer, __class::String>(std::forward<String>(other))
  {}
  
  String& operator = (const char* str) {
    m_ptr = oatpp::base::StrBuffer::createFromCString(str);
    return *this;
  }
  
  String& operator = (const String& other){
    m_ptr = other.m_ptr;
    return *this;
  }
  
  String& operator = (String&& other){
    m_ptr = std::forward<std::shared_ptr<oatpp::base::StrBuffer>>(other.m_ptr);
    return *this;
  }
  
  static const String& empty(){
    static String empty;
    return empty;
  }
  
  bool operator==(const String &other) const {
    return m_ptr->equals(other.get());
  }
  
  bool operator!=(const String &other) const {
    return !m_ptr->equals(other.get());
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
class Primitive : public oatpp::base::Countable {
public:
  typedef TValueType ValueType;
public:
  OBJECT_POOL(Primitive_Type_Pool, Primitive, 32)
  SHARED_OBJECT_POOL(Shared_Primitive_Type_Pool, Primitive, 32)
public:

  /**
   * ObjectWrapper template for &l:Primitive;.
   */
  class ObjectWrapper : public type::ObjectWrapper<Primitive, Clazz> {
  public:
    ObjectWrapper(const std::shared_ptr<Primitive>& ptr, const type::Type* const valueType)
      : type::ObjectWrapper<Primitive, Clazz>(ptr)
    {
      if(Clazz::getType() != valueType){
        throw std::runtime_error("Value type does not match");
      }
    }
  public:
    
    ObjectWrapper()
      : type::ObjectWrapper<Primitive, Clazz>()
    {}
    
    ObjectWrapper(const std::shared_ptr<Primitive>& ptr)
      : type::ObjectWrapper<Primitive, Clazz>(ptr)
    {}
    
    ObjectWrapper(std::shared_ptr<Primitive>&& ptr)
      : type::ObjectWrapper<Primitive, Clazz>(std::move(ptr))
    {}
    
    ObjectWrapper(const ObjectWrapper& other)
      : type::ObjectWrapper<Primitive, Clazz>(other)
    {}
    
    ObjectWrapper(ObjectWrapper&& other)
      : type::ObjectWrapper<Primitive, Clazz>(std::move(other))
    {}
    
    ObjectWrapper(const ValueType& value)
      : type::ObjectWrapper<Primitive, Clazz>(Primitive::createShared(value))
    {}
    
    ObjectWrapper& operator = (const ValueType& value){
      if(!this->m_ptr){
        this->m_ptr = Primitive::createShared(value);
      } else {
        this->m_ptr.get()->setValue(value);
      }
      return *this;
    }
    
    ObjectWrapper& operator = (const ObjectWrapper &other){
      this->m_ptr = other.m_ptr;
      return *this;
    }
    
    bool operator==(const ObjectWrapper &other) const {
      return this->m_ptr->getValue() == other->getValue();
    }
    
    bool operator!=(const ObjectWrapper &other) const {
      return this->m_ptr->getValue() != other->getValue();
    }
    
    inline operator ValueType() const {
      return this->get()->getValue();
    }
    
    static const ObjectWrapper& empty(){
      static ObjectWrapper result;
      return result;
    }
    
  };
  
private:
  
  ValueType m_value;
  
public:
  /**
   * Constructor.
   * @param value - initial value.
   */
  Primitive(const ValueType& value)
    : m_value(value)
  {}
public:

  /**
   * Create shared primitive.
   * @param value - initial value.
   * @return - `std::shared_ptr` to Primitive.
   */
  static std::shared_ptr<Primitive> createShared(const ValueType& value){
    return Shared_Primitive_Type_Pool::allocateShared(value);
  }

  /**
   * Create shared primitive upcasted to &id:oatpp::base::Countable;.
   * @param value - initial value.
   * @return - `std::shared_ptr` to primitive upcasted to &id:oatpp::base::Countable;.
   */
  static std::shared_ptr<Countable> createAbstract(const ValueType& value){
    return std::static_pointer_cast<Countable>(Shared_Primitive_Type_Pool::allocateShared(value));
  }

  /**
   * Set value.
   * @param value.
   */
  void setValue(const ValueType& value) {
    m_value = value;
  }

  /**
   * Get value.
   * @return - value.
   */
  ValueType getValue() {
    return m_value;
  }
  
};

/**
 * Int8 is an ObjectWrapper over &l:Primitive; and __class::Int8.
 */
typedef Primitive<v_int8, __class::Int8>::ObjectWrapper Int8;

/**
 * UInt8 is an ObjectWrapper over &l:Primitive; and __class::UInt8.
 */
typedef Primitive<v_uint8, __class::UInt8>::ObjectWrapper UInt8;

/**
 * Int16 is an ObjectWrapper over &l:Primitive; and __class::Int16.
 */
typedef Primitive<v_int16, __class::Int16>::ObjectWrapper Int16;

/**
 * UInt16 is an ObjectWrapper over &l:Primitive; and __class::UInt16.
 */
typedef Primitive<v_uint16, __class::UInt16>::ObjectWrapper UInt16;

/**
 * Int32 is an ObjectWrapper over &l:Primitive; and __class::Int32.
 */
typedef Primitive<v_int32, __class::Int32>::ObjectWrapper Int32;

/**
 * UInt32 is an ObjectWrapper over &l:Primitive; and __class::UInt32.
 */
typedef Primitive<v_uint32, __class::UInt32>::ObjectWrapper UInt32;

/**
 * Int64 is an ObjectWrapper over &l:Primitive; and __class::Int64.
 */
typedef Primitive<v_int64, __class::Int64>::ObjectWrapper Int64;

/**
 * UInt64 is an ObjectWrapper over &l:Primitive; and __class::UInt64.
 */
typedef Primitive<v_uint64, __class::UInt64>::ObjectWrapper UInt64;

/**
 * Float32 is an ObjectWrapper over &l:Primitive; and __class::Float32.
 */
typedef Primitive<v_float32, __class::Float32>::ObjectWrapper Float32;

/**
 * Float64 is an ObjectWrapper over &l:Primitive; and __class::Float64.
 */
typedef Primitive<v_float64, __class::Float64>::ObjectWrapper Float64;

/**
 * Boolean is an ObjectWrapper over &l:Primitive; and __class::Boolean.
 */
typedef Primitive<bool, __class::Boolean>::ObjectWrapper Boolean;
  
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
