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

#ifndef oatpp_data_mapping_type_ListMap_hpp
#define oatpp_data_mapping_type_ListMap_hpp

#include "./Type.hpp"
#include "oatpp/core/collection/ListMap.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {
  
namespace __class {
  
  class AbstractListMap {
  public:
    static const ClassId CLASS_ID;
  };
  
  template<class Key, class Value>
  class ListMap; // FWD
  
}
  
template<class Key, class Value>
class ListMap : public oatpp::collection::ListMap<Key, Value> {
  friend __class::ListMap<Key, Value>;
public:
  typedef oatpp::data::mapping::type::ObjectWrapper<ListMap, __class::ListMap<Key, Value>> ObjectWrapper;
public:
  OBJECT_POOL(DTO_LISTMAP_POOL, ListMap, 32)
  SHARED_OBJECT_POOL(SHARED_DTO_LISTMAP_POOL, ListMap, 32)
protected:
  
  static AbstractObjectWrapper Z__CLASS_OBJECT_CREATOR(){
    return AbstractObjectWrapper(SHARED_DTO_LISTMAP_POOL::allocateShared(), Z__CLASS_GET_TYPE());
  }
  
  static Type* Z__CLASS_GET_TYPE(){
    static Type type(__class::AbstractListMap::CLASS_ID, nullptr, &Z__CLASS_OBJECT_CREATOR);
    if(type.params.empty()){
      type.params.push_back(Key::Class::getType());
      type.params.push_back(Value::Class::getType());
    }
    return &type;
  }
  
public:
  ListMap()
  {}
public:
  
  static ObjectWrapper createShared(){
    return ObjectWrapper(SHARED_DTO_LISTMAP_POOL::allocateShared());
  }
  
  virtual void putPolymorphicItem(const AbstractObjectWrapper& key, const AbstractObjectWrapper& value){
    auto keyPtr = std::static_pointer_cast<typename Key::ObjectType>(key.getPtr());
    auto valuePtr = std::static_pointer_cast<typename Value::ObjectType>(value.getPtr());
    this->put(Key(keyPtr, key.valueType), Value(valuePtr, value.valueType));
  }
  
};

namespace __class {
  
  template<class Key, class Value>
  class ListMap : public AbstractListMap{
  public:
    
    static Type* getType(){
      static Type* type = static_cast<Type*>(oatpp::data::mapping::type::ListMap<Key, Value>::Z__CLASS_GET_TYPE());
      return type;
    }
    
  };
  
}
  
}}}}

#endif /* oatpp_data_mapping_type_ListMap_hpp */
