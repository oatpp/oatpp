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

#ifndef oatpp_data_type_Primitive_hpp
#define oatpp_data_type_Primitive_hpp

#include "./Type.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Controllable.hpp"
#include "oatpp/core/base/StrBuffer.hpp"

#include "oatpp/core/base/PtrWrapper.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {
  
namespace __class {
  
  class String; // FWD
  class Int32; // FWD
  class Int64; // FWD
  class Float32; // FWD
  class Float64; // FWD
  class Boolean; // FWD
  
}

class String : public oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String> {
public:
  String(const std::shared_ptr<oatpp::base::StrBuffer>& ptr, const type::Type* const valueType);
public:
  
  String() {}
  
  String(v_int32 size)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createShared(size))
  {}
  
  String(const void* data, v_int32 size, bool copyAsOwnData = true)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createShared(data, size, copyAsOwnData))
  {}
  
  String(const void* data1, v_int32 size1, const void* data2, v_int32 size2)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createSharedConcatenated(data1, size1, data2, size2))
  {}
  
  String(const char* data, bool copyAsOwnData = true)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(oatpp::base::StrBuffer::createFromCString(data, copyAsOwnData))
  {}
  
  String(const std::shared_ptr<oatpp::base::StrBuffer>& ptr)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(ptr)
  {}
  
  String(std::shared_ptr<oatpp::base::StrBuffer>&& ptr)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(std::move(ptr))
  {}
  
  String(const String& other)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(other)
  {}
  
  String(String&& other)
    : oatpp::data::mapping::type::PtrWrapper<oatpp::base::StrBuffer, __class::String>(std::move(other))
  {}
  
  String& operator = (const char* str) {
    m_ptr = oatpp::base::StrBuffer::createFromCString(str);
    return *this;
  }
  
  String& operator = (const String& other){
    oatpp::base::PtrWrapper<oatpp::base::StrBuffer>::operator=(other);
    return *this;
  }
  
  String& operator = (String&& other){
    oatpp::base::PtrWrapper<oatpp::base::StrBuffer>::operator=(std::move(other));
    return *this;
  }
  
  String operator + (const char* str) const{
    return oatpp::base::StrBuffer::createSharedConcatenated(m_ptr.get()->getData(), m_ptr.get()->getSize(), str, (v_int32) std::strlen(str));
  }
  
  String operator + (const String& other) const{
    return oatpp::base::StrBuffer::createSharedConcatenated(m_ptr.get()->getData(), m_ptr.get()->getSize(), other.get()->getData(), other.get()->getSize());
  }
  
  static const String& empty(){
    static String empty;
    return empty;
  }
  
  bool equals(const String& other) const {
    return m_ptr->equals(other.get());
  }
  
  explicit operator bool() const {
    return m_ptr.operator bool();
  }
  
};

template<typename ValueType, class Clazz>
class Primitive : public oatpp::base::Controllable {
public:
  OBJECT_POOL(Primitive_Type_Pool, Primitive, 32)
  SHARED_OBJECT_POOL(Shared_Primitive_Type_Pool, Primitive, 32)
public:
  
  class PtrWrapper : public oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz> {
  public:
    PtrWrapper(const std::shared_ptr<Primitive>& ptr, const type::Type* const valueType)
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>(ptr)
    {
      if(Clazz::getType() != valueType){
        throw std::runtime_error("Value type does not match");
      }
    }
  public:
    
    PtrWrapper()
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>()
    {}
    
    PtrWrapper(const std::shared_ptr<Primitive>& ptr)
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>(ptr)
    {}
    
    PtrWrapper(std::shared_ptr<Primitive>&& ptr)
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>(std::move(ptr))
    {}
    
    PtrWrapper(const PtrWrapper& other)
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>(other)
    {}
    
    PtrWrapper(PtrWrapper&& other)
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>(std::move(other))
    {}
    
    PtrWrapper(const ValueType& value)
      : oatpp::data::mapping::type::PtrWrapper<Primitive, Clazz>(Primitive::createShared(value))
    {}
    
    PtrWrapper& operator = (const ValueType& value){
      if(this->isNull()){
        this->m_ptr = Primitive::createShared(value);
      } else {
        this->m_ptr.get()->setValue(value);
      }
      return *this;
    }
    
    inline operator ValueType() const {
      return this->get()->getValue();
    }
    
    static const PtrWrapper& empty(){
      static PtrWrapper result;
      return result;
    }
    
  };
  
private:
  
  ValueType m_value;
  
public:
  Primitive(const ValueType& value)
    : m_value(value)
  {}
public:
  
  static std::shared_ptr<Primitive> createShared(const ValueType& value){
    return Shared_Primitive_Type_Pool::allocateShared(value);
  }
  
  static std::shared_ptr<Controllable> createAbstract(const ValueType& value){
    return std::static_pointer_cast<Controllable>(Shared_Primitive_Type_Pool::allocateShared(value));
  }
  
  void setValue(const ValueType& value) {
    m_value = value;
  }
  
  ValueType getValue() {
    return m_value;
  }
  
};


typedef Primitive<v_int32, __class::Int32> Int32;
typedef Primitive<v_int64, __class::Int64> Int64;
typedef Primitive<v_float32, __class::Float32> Float32;
typedef Primitive<v_float64, __class::Float64> Float64;
typedef Primitive<bool, __class::Boolean> Boolean;
  
namespace __class {
  
  class String {
  public:
    constexpr static const char* const CLASS_NAME = "String";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static oatpp::data::mapping::type::String parseFromString(const oatpp::data::mapping::type::String& str, bool& success){
      success = true;
      return str;
    }
    
  };
  
  class Int32 {
  public:
    constexpr static const char* const CLASS_NAME = "Int32";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Int32::PtrWrapper parseFromString(const oatpp::data::mapping::type::String& str, bool& success);
    
  };
  
  class Int64 {
  public:
    constexpr static const char* const CLASS_NAME = "Int64";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Int64::PtrWrapper parseFromString(const oatpp::data::mapping::type::String& str, bool& success);
    
  };
  
  class Float32 {
  public:
    constexpr static const char* const CLASS_NAME = "Float32";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Float32::PtrWrapper parseFromString(const oatpp::data::mapping::type::String& str, bool& success);
    
  };
  
  class Float64 {
  public:
    constexpr static const char* const CLASS_NAME = "Float64";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Float64::PtrWrapper parseFromString(const oatpp::data::mapping::type::String& str, bool& success);
    
  };
  
  class Boolean {
  public:
    constexpr static const char* const CLASS_NAME = "Boolean";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Boolean::PtrWrapper parseFromString(const oatpp::data::mapping::type::String& str, bool& success);
    
  };
  
}
  
}}}}

#endif /* oatpp_base_controllable_PrimitiveDataTypes_hpp */
