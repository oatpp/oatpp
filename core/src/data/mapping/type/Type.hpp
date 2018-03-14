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

#include "../../../base/PtrWrapper.hpp"

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
class PolymorphicWrapper : public oatpp::base::PtrWrapper<T> {
public:
  typedef T ObjectType;
public:
  typedef __class::Void Class;
public:
  PolymorphicWrapper(const oatpp::base::PtrWrapper<T>& other, const Type* const type)
    : oatpp::base::PtrWrapper<T>(other)
    , valueType(type)
  {}
  
  PolymorphicWrapper(oatpp::base::PtrWrapper<T>&& other, const Type* const type)
    : oatpp::base::PtrWrapper<T>(std::move(other))
    , valueType(type)
  {}
public:
  
  PolymorphicWrapper()
    : oatpp::base::PtrWrapper<T>() 
    , valueType(Class::getType())
  {}
  
  PolymorphicWrapper(const Type* const type)
    : oatpp::base::PtrWrapper<T>()
    , valueType(type)
  {}
  
  PolymorphicWrapper(const std::shared_ptr<T>& ptr, const Type* const type)
    : oatpp::base::PtrWrapper<T>(ptr)
    , valueType(type)
  {}
  
  PolymorphicWrapper(const PolymorphicWrapper& other)
    : oatpp::base::PtrWrapper<T>(other)
    , valueType(other.valueType)
  {}
  
  PolymorphicWrapper(PolymorphicWrapper&& other)
    : oatpp::base::PtrWrapper<T>(std::move(other))
    , valueType(other.valueType)
  {}
  
  static PolymorphicWrapper empty(){
    return PolymorphicWrapper();
  }
  
  PolymorphicWrapper& operator=(const oatpp::base::PtrWrapper<T>& other){
    oatpp::base::PtrWrapper<T>::operator = (other);
    return *this;
  }
  
  PolymorphicWrapper& operator=(const oatpp::base::PtrWrapper<T>&& other){
    oatpp::base::PtrWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>& other){
    oatpp::base::PtrWrapper<T>::operator = (other);
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>&& other){
    oatpp::base::PtrWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
  inline operator PolymorphicWrapper<oatpp::base::Controllable>() const {
    return PolymorphicWrapper<oatpp::base::Controllable>(this->m_ptr, valueType);
  }
  
  const Type* const valueType;
  
};
  
template <class T, class Clazz>
class PtrWrapper : public PolymorphicWrapper<T>{
public:
  typedef T ObjectType;
public:
  typedef Clazz Class;
public:
  PtrWrapper(const std::shared_ptr<T>& ptr, const type::Type* const valueType)
    : PolymorphicWrapper<T>(ptr, Class::getType())
  {
    if(Class::getType() != valueType){
      throw std::runtime_error("Value type does not match");
    }
  }
public:
  
  PtrWrapper()
    : PolymorphicWrapper<T>(Class::getType())
  {}
  
  PtrWrapper(const std::shared_ptr<T>& ptr)
    : PolymorphicWrapper<T>(ptr, Class::getType())
  {}
  
  PtrWrapper(const oatpp::base::PtrWrapper<T>& other)
    : PolymorphicWrapper<T>(other, Class::getType())
  {}
  
  PtrWrapper(oatpp::base::PtrWrapper<T>&& other)
    : PolymorphicWrapper<T>(std::move(other), Class::getType())
  {}
  
  static PtrWrapper empty(){
    return PtrWrapper();
  }
  
  PtrWrapper& operator=(const oatpp::base::PtrWrapper<T>& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("PtrWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::PtrWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (other);
    return *this;
  }
  
  PtrWrapper& operator=(const oatpp::base::PtrWrapper<T>&& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("PtrWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::PtrWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
};
  
typedef PolymorphicWrapper<oatpp::base::Controllable> AbstractPtrWrapper;
  
class Type {
public:
  typedef AbstractPtrWrapper (*Creator)();
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
    
    void set(void* object, const oatpp::base::PtrWrapper<oatpp::base::Controllable>& value) {
      oatpp::base::PtrWrapper<oatpp::base::Controllable>* property =
      (oatpp::base::PtrWrapper<oatpp::base::Controllable>*)(((v_int64) object) + offset);
      *property = value;
    }
    
    oatpp::base::PtrWrapper<oatpp::base::Controllable> get(void* object) {
      oatpp::base::PtrWrapper<oatpp::base::Controllable>* property =
      (oatpp::base::PtrWrapper<oatpp::base::Controllable>*)(((v_int64) object) + offset);
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
