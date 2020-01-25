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

#ifndef oatpp_data_mapping_type_List_hpp
#define oatpp_data_mapping_type_List_hpp

#include "./Type.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {
  
namespace __class {
  
  class AbstractList {
  public:
    static const ClassId CLASS_ID;
  };
  
  template<class T>
  class List; // FWD
  
}
  
template<class T, class Class>
class ListTypeTemplate : public oatpp::collection::LinkedList<T> {
  friend Class;
public:
  typedef oatpp::data::mapping::type::ObjectWrapper<ListTypeTemplate, Class> ObjectWrapper;
public:
  OBJECT_POOL(DTO_LIST_POOL, ListTypeTemplate, 32)
  SHARED_OBJECT_POOL(SHARED_DTO_LIST_POOL, ListTypeTemplate, 32)
protected:
  
  static AbstractObjectWrapper Z__CLASS_OBJECT_CREATOR(){
    return AbstractObjectWrapper(SHARED_DTO_LIST_POOL::allocateShared(), Z__CLASS_GET_TYPE());
  }
  
  static Type* Z__CLASS_GET_TYPE(){
    static Type type(Class::CLASS_ID, nullptr, &Z__CLASS_OBJECT_CREATOR);
    if(type.params.empty()){
      type.params.push_back(T::Class::getType());
    }
    return &type;
  }
  
public:
  ListTypeTemplate()
  {}
public:
  
  static ObjectWrapper createShared(){
    return ObjectWrapper(SHARED_DTO_LIST_POOL::allocateShared());
  }
  
  virtual void addPolymorphicItem(const AbstractObjectWrapper& item){
    auto ptr = std::static_pointer_cast<typename T::ObjectType>(item.getPtr());
    this->pushBack(T(ptr, item.valueType));
  }
  
};

template<class T>
using List = ListTypeTemplate<T, __class::List<T>>;
  
namespace __class {
  
  template<class T>
  class List : public AbstractList {
  public:
    
    static Type* getType(){
      static Type* type = static_cast<Type*>(oatpp::data::mapping::type::List<T>::Z__CLASS_GET_TYPE());
      return type;
    }
    
  };
  
}
  
}}}}

#endif /* oatpp_data_mapping_type_List_hpp */
