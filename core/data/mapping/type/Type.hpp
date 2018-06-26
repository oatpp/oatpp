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

#include "oatpp/core/base/Controllable.hpp"

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
  
template<class T>
class BasicPtrWrapper {
protected:
  std::shared_ptr<T> m_ptr;
public:
  
  BasicPtrWrapper() {}
  
  BasicPtrWrapper(const std::shared_ptr<T>& ptr)
    : m_ptr(ptr)
  {}
  
  BasicPtrWrapper(std::shared_ptr<T>&& ptr)
    : m_ptr(std::move(ptr))
  {}
  
  BasicPtrWrapper(const BasicPtrWrapper& other)
    : m_ptr(other.m_ptr)
  {}
  
  BasicPtrWrapper(BasicPtrWrapper&& other)
  : m_ptr(std::move(other.m_ptr))
  {}
  
  BasicPtrWrapper& operator = (const BasicPtrWrapper& other){
    m_ptr = other.m_ptr;
    return *this;
  }
  
  BasicPtrWrapper& operator = (BasicPtrWrapper&& other){
    m_ptr = std::move(other.m_ptr);
    return *this;
  }
  
  T* operator->() const {
    return m_ptr.operator->();
  }
  
  T* get() const {
    return m_ptr.get();
  }
  
  std::shared_ptr<T> getPtr() const {
    return m_ptr;
  }
  
  bool isNull() const {
    return m_ptr.get() == nullptr;
  }
  
  static const BasicPtrWrapper& empty(){
    static BasicPtrWrapper empty;
    return empty;
  }
  
  inline bool operator == (const BasicPtrWrapper& other){
    return m_ptr.get() == other.m_ptr.get();
  }
  
  inline bool operator != (const BasicPtrWrapper& other){
    return m_ptr.get() != other.m_ptr.get();
  }
  
};

template<class T, class F>
inline BasicPtrWrapper<T> static_wrapper_cast(const F& from){
  return BasicPtrWrapper<T>(std::static_pointer_cast<T>(from.getPtr()));
}
  
template <class T>
class PolymorphicWrapper : public BasicPtrWrapper<T> {
public:
  typedef T ObjectType;
public:
  typedef __class::Void Class;
public:
  PolymorphicWrapper(const BasicPtrWrapper<T>& other, const Type* const type)
    : BasicPtrWrapper<T>(other)
    , valueType(type)
  {}
  
  PolymorphicWrapper(BasicPtrWrapper<T>&& other, const Type* const type)
    : BasicPtrWrapper<T>(std::move(other))
    , valueType(type)
  {}
public:
  
  PolymorphicWrapper()
    : BasicPtrWrapper<T>()
    , valueType(Class::getType())
  {}
  
  PolymorphicWrapper(const Type* const type)
    : BasicPtrWrapper<T>()
    , valueType(type)
  {}
  
  PolymorphicWrapper(const std::shared_ptr<T>& ptr, const Type* const type)
    : BasicPtrWrapper<T>(ptr)
    , valueType(type)
  {}
  
  PolymorphicWrapper(const PolymorphicWrapper& other)
    : BasicPtrWrapper<T>(other)
    , valueType(other.valueType)
  {}
  
  PolymorphicWrapper(PolymorphicWrapper&& other)
    : BasicPtrWrapper<T>(std::move(other))
    , valueType(other.valueType)
  {}
  
  static PolymorphicWrapper empty(){
    return PolymorphicWrapper();
  }
  
  PolymorphicWrapper& operator=(const BasicPtrWrapper<T>& other){
    BasicPtrWrapper<T>::operator = (other);
    return *this;
  }
  
  PolymorphicWrapper& operator=(const BasicPtrWrapper<T>&& other){
    BasicPtrWrapper<T>::operator = (std::move(other));
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>& other){
    BasicPtrWrapper<T>::operator = (other);
    return *this;
  }
  
  PolymorphicWrapper& operator=(const PolymorphicWrapper<T>&& other){
    BasicPtrWrapper<T>::operator = (std::move(other));
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
  
  PtrWrapper(const BasicPtrWrapper<T>& other)
    : PolymorphicWrapper<T>(other, Class::getType())
  {}
  
  PtrWrapper(BasicPtrWrapper<T>&& other)
    : PolymorphicWrapper<T>(std::move(other), Class::getType())
  {}
  
  static PtrWrapper empty(){
    return PtrWrapper();
  }
  
  PtrWrapper& operator=(const BasicPtrWrapper<T>& other){
    if(this->valueType != other.valueType){
      OATPP_LOGE("PtrWrapper", "Invalid class cast");
      throw std::runtime_error("[oatpp::data::mapping::type::PtrWrapper]: Invalid class cast");
    }
    PolymorphicWrapper<T>::operator = (other);
    return *this;
  }
  
  PtrWrapper& operator=(const BasicPtrWrapper<T>&& other){
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
    
    void set(void* object, const BasicPtrWrapper<oatpp::base::Controllable>& value) {
      BasicPtrWrapper<oatpp::base::Controllable>* property =
      (BasicPtrWrapper<oatpp::base::Controllable>*)(((v_int64) object) + offset);
      *property = value;
    }
    
    BasicPtrWrapper<oatpp::base::Controllable> get(void* object) {
      BasicPtrWrapper<oatpp::base::Controllable>* property =
      (BasicPtrWrapper<oatpp::base::Controllable>*)(((v_int64) object) + offset);
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
