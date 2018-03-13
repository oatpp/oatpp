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

#include "../../../utils/ConversionUtils.hpp"

#include "../../../base/memory/ObjectPool.hpp"
#include "../../../base/Controllable.hpp"
#include "../../../base/String.hpp"

#include "../../../base/SharedWrapper.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {
  
namespace __class {
  
  class String; // FWD
  class Int32; // FWD
  class Int64; // FWD
  class Float32; // FWD
  class Float64; // FWD
  class Boolean; // FWD
  
}
  
class StringSharedWrapper : public oatpp::data::mapping::type::SharedWrapper<oatpp::base::String, __class::String> {
public:
  StringSharedWrapper(const std::shared_ptr<oatpp::base::String>& ptr, const type::Type* const valueType);
public:
  
  StringSharedWrapper() {}
  
  StringSharedWrapper(const std::shared_ptr<oatpp::base::String>& ptr)
    : type::SharedWrapper<oatpp::base::String, __class::String>(ptr)
  {}
  
  StringSharedWrapper(std::shared_ptr<oatpp::base::String>&& ptr)
    : type::SharedWrapper<oatpp::base::String, __class::String>(std::move(ptr))
  {}
  
  StringSharedWrapper(const char* str)
    : type::SharedWrapper<oatpp::base::String, __class::String>(oatpp::base::String::createFromCString(str))
  {}
  
  StringSharedWrapper(const std::string& str)
    : type::SharedWrapper<oatpp::base::String, __class::String>
    (oatpp::base::String::createShared(str.data(), str.size()))
  {}
  
  StringSharedWrapper(const oatpp::base::SharedWrapper<oatpp::base::String>& other)
    : type::SharedWrapper<oatpp::base::String, __class::String>(other)
  {}
  
  StringSharedWrapper(oatpp::base::SharedWrapper<oatpp::base::String>&& other)
    : type::SharedWrapper<oatpp::base::String, __class::String>(std::move(other))
  {}
  
  StringSharedWrapper& operator = (const char* str) {
    m_ptr = oatpp::base::String::createFromCString(str);
    return *this;
  }
  
  StringSharedWrapper& operator = (const std::string& str) {
    m_ptr = oatpp::base::String::createShared(str.data(), str.size());
    return *this;
  }
  
  StringSharedWrapper& operator = (const oatpp::base::SharedWrapper<oatpp::base::String>& other){
    oatpp::base::SharedWrapper<oatpp::base::String>::operator=(other);
    return *this;
  }
  
  StringSharedWrapper& operator = (oatpp::base::SharedWrapper<oatpp::base::String>&& other){
    oatpp::base::SharedWrapper<oatpp::base::String>::operator=(std::move(other));
    return *this;
  }
  
  StringSharedWrapper operator + (const char* str) const{
    return oatpp::base::String::createSharedConcatenated(m_ptr.get()->getData(), m_ptr.get()->getSize(), str, (v_int32) std::strlen(str));
  }
  
  StringSharedWrapper operator + (const oatpp::base::SharedWrapper<oatpp::base::String>& other) const{
    return oatpp::base::String::createSharedConcatenated(m_ptr.get()->getData(), m_ptr.get()->getSize(), other.get()->getData(), other.get()->getSize());
  }
  
  operator AbstractSharedWrapper() const;
  operator std::string() const;
  
  static const StringSharedWrapper& empty(){
    static StringSharedWrapper empty;
    return empty;
  }
  
};

template<typename ValueType, class Clazz>
class Primitive : public oatpp::base::Controllable {
public:
  OBJECT_POOL(Primitive_Type_Pool, Primitive, 32)
  SHARED_OBJECT_POOL(Shared_Primitive_Type_Pool, Primitive, 32)
public:
  
  class SharedWrapper : public oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz> {
  public:
    SharedWrapper(const std::shared_ptr<Primitive>& ptr, const type::Type* const valueType)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>(ptr)
    {
      if(Clazz::getType() != valueType){
        throw std::runtime_error("Value type does not match");
      }
    }
  public:
    
    SharedWrapper()
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>()
    {}
    
    SharedWrapper(const std::shared_ptr<Primitive>& ptr)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>(ptr)
    {}
    
    SharedWrapper(std::shared_ptr<Primitive>&& ptr)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>(std::move(ptr))
    {}
    
    SharedWrapper(const SharedWrapper& other)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>(other)
    {}
    
    SharedWrapper(SharedWrapper&& other)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>(std::move(other))
    {}
    
    SharedWrapper(const VariantWrapper& variant)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>()
    {
      *this = variant;
    }
    
    SharedWrapper(const ValueType& value)
      : oatpp::data::mapping::type::SharedWrapper<Primitive, Clazz>(Primitive::createShared(value))
    {}
    
    SharedWrapper& operator = (const ValueType& value){
      if(this->isNull()){
        this->m_ptr = Primitive::createShared(value);
      } else {
        this->m_ptr.get()->setValue(value);
      }
      return *this;
    }
    
    SharedWrapper& operator=(const VariantWrapper& variant) {
      if(this->valueType != variant.getValueType()) {
        OATPP_LOGE("[oatpp::data::mapping::type::Primitive::SharedWrapper]", "Invalid class cast");
        throw std::runtime_error("[oatpp::data::mapping::type::Primitive::SharedWrapper]: Invalid class cast");
      }
      oatpp::base::SharedWrapper<oatpp::base::Controllable>::operator=(variant);
      return *this;
    }
    
    inline operator ValueType() const {
      return this->get()->getValue();
    }
    
    inline operator AbstractSharedWrapper() const {
      return AbstractSharedWrapper(this->m_ptr, Clazz::getType());
    }
    
    static const SharedWrapper& empty(){
      static SharedWrapper result;
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
    
    static type::StringSharedWrapper parseFromString(const oatpp::base::String::SharedWrapper& str, bool& success){
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
    
    static type::Int32::SharedWrapper parseFromString(const oatpp::base::String::SharedWrapper& str, bool& success);
    
  };
  
  class Int64 {
  public:
    constexpr static const char* const CLASS_NAME = "Int64";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Int64::SharedWrapper parseFromString(const oatpp::base::String::SharedWrapper& str, bool& success);
    
  };
  
  class Float32 {
  public:
    constexpr static const char* const CLASS_NAME = "Float32";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Float32::SharedWrapper parseFromString(const oatpp::base::String::SharedWrapper& str, bool& success);
    
  };
  
  class Float64 {
  public:
    constexpr static const char* const CLASS_NAME = "Float64";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Float64::SharedWrapper parseFromString(const oatpp::base::String::SharedWrapper& str, bool& success);
    
  };
  
  class Boolean {
  public:
    constexpr static const char* const CLASS_NAME = "Boolean";
    
    static Type* getType(){
      static Type type(CLASS_NAME);
      return &type;
    }
    
    static type::Boolean::SharedWrapper parseFromString(const oatpp::base::String::SharedWrapper& str, bool& success);
    
  };
  
}
  
template<class T>
base::SharedWrapper<base::String>
primitiveToStr(const oatpp::data::mapping::type::PolymorphicWrapper<T>& primitive) {
  auto type = primitive.valueType;
  if(type == oatpp::data::mapping::type::__class::String::getType()) {
    return std::static_pointer_cast<base::String>(primitive.getPtr());
  } else if(type == oatpp::data::mapping::type::__class::Int32::getType()) {
    return utils::conversion::int32ToStr(static_cast<oatpp::data::mapping::type::Int32*>(primitive.get())->getValue());
  } else if(type == oatpp::data::mapping::type::__class::Int64::getType()) {
    return utils::conversion::int64ToStr(static_cast<oatpp::data::mapping::type::Int64*>(primitive.get())->getValue());
  } else if(type == oatpp::data::mapping::type::__class::Float32::getType()) {
    return utils::conversion::float32ToStr(static_cast<oatpp::data::mapping::type::Float32*>(primitive.get())->getValue());
  } else if(type == oatpp::data::mapping::type::__class::Float64::getType()) {
    return utils::conversion::float64ToStr(static_cast<oatpp::data::mapping::type::Float64*>(primitive.get())->getValue());
  } else if(type == oatpp::data::mapping::type::__class::Boolean::getType()) {
    return utils::conversion::boolToStr(static_cast<oatpp::data::mapping::type::Boolean*>(primitive.get())->getValue());
  }
  throw std::runtime_error("[oatpp::data::mapping::type::primitiveToStr]: Invalid primitive type");
}
  
}}}}

#endif /* oatpp_base_controllable_PrimitiveDataTypes_hpp */
