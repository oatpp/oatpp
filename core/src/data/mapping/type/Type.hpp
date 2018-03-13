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

#ifndef oatpp_data_type_Type_hpp
#define oatpp_data_type_Type_hpp

#include "../../../base/SharedWrapper.hpp"

#include <list>
#include <unordered_map>
#include <string>

namespace oatpp { namespace data { namespace mapping { namespace type {
  
class Type; // FWD
  
namespace __class {
  class Void {
  public:
    constexpr static const char* const CLASS_NAME = "Void";
    static Type* getType();
  };
}
  
template <class T>
class PolymorphicWrapper : public oatpp::base::SharedWrapper<T> {
public:
  typedef T ObjectType;
public:
  typedef __class::Void Class;
public:
  PolymorphicWrapper(const oatpp::base::SharedWrapper<T>& other, const Type* const type)
    : oatpp::base::SharedWrapper<T>(other)
    , valueType(type)
  {}
  
  PolymorphicWrapper(oatpp::base::SharedWrapper<T>&& other, const Type* const type)
    : oatpp::base::SharedWrapper<T>(std::move(other))
    , valueType(type)
  {}
public:
  
  PolymorphicWrapper()
    : oatpp::base::SharedWrapper<T>() 
    , valueType(Class::getType())
  {}
  
  PolymorphicWrapper(const Type* const type)
    : oatpp::base::SharedWrapper<T>()
    , valueType(type)
  {}
  
  PolymorphicWrapper(const std::shared_ptr<T>& ptr, const Type* const type)
    : oatpp::base::SharedWrapper<T>(ptr)
    , valueType(type)
  {}
  
  PolymorphicWrapper(const PolymorphicWrapper& other)
    : oatpp::base::SharedWrapper<T>(other)
    , valueType(other.valueType)
  {}
  
  PolymorphicWrapper(PolymorphicWrapper&& other)
    : oatpp::base::SharedWrapper<T>(std::move(other))
    , valueType(other.valueType)
  {}
  
  static PolymorphicWrapper empty(){
    return PolymorphicWrapper();
  }
  
  PolymorphicWrapper& operator=(const oatpp::base::SharedWrapper<T>& other){
    oatpp::base::SharedWrapper<T>::operator = (other);
    return *this;
  }
  
  PolymorphicWrapper& operator=(const oatpp::base::SharedWrapper<T>&& other){
    oatpp::base::SharedWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>& other){
    oatpp::base::SharedWrapper<T>::operator = (other);
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>&& other){
    oatpp::base::SharedWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
  const Type* const valueType;
  
};
  
template <class T, class Clazz>
class SharedWrapper : public PolymorphicWrapper<T>{
public:
  typedef T ObjectType;
public:
  typedef Clazz Class;
public:
  SharedWrapper(const std::shared_ptr<T>& ptr, const type::Type* const valueType)
    : PolymorphicWrapper<T>(ptr, Class::getType())
  {
    if(Class::getType() != valueType){
      throw std::runtime_error("Value type does not match");
    }
  }
public:
  
  SharedWrapper()
    : PolymorphicWrapper<T>(Class::getType())
  {}
  
  SharedWrapper(const std::shared_ptr<T>& ptr)
    : PolymorphicWrapper<T>(ptr, Class::getType())
  {}
  
  SharedWrapper(const oatpp::base::SharedWrapper<T>& other)
    : PolymorphicWrapper<T>(other, Class::getType())
  {}
  
  SharedWrapper(oatpp::base::SharedWrapper<T>&& other)
    : PolymorphicWrapper<T>(std::move(other), Class::getType())
  {}
  
  static SharedWrapper empty(){
    return SharedWrapper();
  }
  
  SharedWrapper& operator=(const oatpp::base::SharedWrapper<T>& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("SharedWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::SharedWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (other);
    return *this;
  }
  
  SharedWrapper& operator=(const oatpp::base::SharedWrapper<T>&& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("SharedWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::SharedWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
};

class VariantWrapper : public oatpp::base::SharedWrapper<oatpp::base::Controllable> {
protected:
  const Type* m_valueType;
public:
  
  explicit VariantWrapper()
    : oatpp::base::SharedWrapper<oatpp::base::Controllable>()
    , m_valueType(nullptr)
  {}
  
  VariantWrapper(const VariantWrapper& other)
    : oatpp::base::SharedWrapper<oatpp::base::Controllable>(other)
    , m_valueType(other.m_valueType)
  {}
  
  template <class T, class Clazz>
  VariantWrapper(const oatpp::data::mapping::type::SharedWrapper<T, Clazz>& other)
    : oatpp::base::SharedWrapper<oatpp::base::Controllable>(std::static_pointer_cast<oatpp::base::Controllable>(other.getPtr()))
    , m_valueType(other.valueType)
  {}
  
  static VariantWrapper empty(){
    return VariantWrapper();
  }
  
  const Type* const getValueType() const {
    return m_valueType;
  }
  
  template <class T, class Clazz>
  VariantWrapper& operator=(const oatpp::data::mapping::type::SharedWrapper<T, Clazz>& other){
    oatpp::base::SharedWrapper<oatpp::base::Controllable>::operator = (other);
    this->m_valueType = other.valueType;
    return *this;
  }
  
  template <class T, class Clazz>
  inline operator oatpp::data::mapping::type::SharedWrapper<T, Clazz>(){
    if(Clazz::getType() != m_valueType){
      OATPP_LOGE("VariantWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::VariantWrapper]: Invalid class cast");
    }
    return oatpp::data::mapping::type::SharedWrapper<T, Clazz>(m_ptr);
  }
  
};
  
typedef PolymorphicWrapper<oatpp::base::Controllable> AbstractSharedWrapper;
  
class Type {
public:
  typedef AbstractSharedWrapper (*Creator)();
public:
  class Property; // FWD
  typedef std::unordered_map<std::string, Property*> Properties;
public:
  
  class Property {
  private:
    const v_int64 offset;
  public:
    
    Property(std::unordered_map<std::string, Property*>* pMap,
             v_int64 pOffset,
             const char* pName,
             Type* pType)
      : offset(pOffset)
      , name(pName)
      , type(pType)
    {
      pMap->insert({name, this});
    }
    
    const char* const name;
    const Type* const type;
    
    void set(void* object, const oatpp::base::SharedWrapper<oatpp::base::Controllable>& value) {
      oatpp::base::SharedWrapper<oatpp::base::Controllable>* property =
      (oatpp::base::SharedWrapper<oatpp::base::Controllable>*)(((v_int64) object) + offset);
      *property = value;
    }
    
    oatpp::base::SharedWrapper<oatpp::base::Controllable> get(void* object) {
      oatpp::base::SharedWrapper<oatpp::base::Controllable>* property =
      (oatpp::base::SharedWrapper<oatpp::base::Controllable>*)(((v_int64) object) + offset);
      return *property;
    }
    
  };
  
public:
  
  Type(const char* pName)
    : name(pName)
    , creator(nullptr)
    , properties(nullptr)
  {}
  
  Type(const char* pName, Creator pCreator)
    : name(pName)
    , creator(pCreator)
    , properties(nullptr)
  {}
  
  Type(const char* pName, Creator pCreator, Properties* pProperties)
    : name(pName)
    , creator(pCreator)
    , properties(pProperties)
  {}
  
  const char* const name;
  std::list<Type*> params;
  
  const Creator creator;
  
  const Properties* const properties;
  
};
  
}}}}
  
#endif /* oatpp_data_type_Type_hpp */
