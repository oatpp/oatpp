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

#ifndef oatpp_data_mapping_type_PairList_hpp
#define oatpp_data_mapping_type_PairList_hpp

#include "./Type.hpp"

#include <list>
#include <initializer_list>
#include <utility>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

class AbstractPairList {
public:
  static const ClassId CLASS_ID;
public:

  class AbstractPolymorphicDispatcher {
  public:
    virtual void addPolymorphicItem(const type::Void& object, const type::Void& key, const type::Void& value) const = 0;
  };

};

template<class Key, class Value>
class PairList;

}

template<class Key, class Value, class C>
class PairListObjectWrapper : public type::ObjectWrapper<std::list<std::pair<Key, Value>>, C> {
public:
  typedef std::list<std::pair<Key, Value>> TemplateObjectType;
  typedef C TemplateObjectClass;
public:

OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(PairListObjectWrapper, TemplateObjectType, TemplateObjectClass)

  PairListObjectWrapper(std::initializer_list<std::pair<Key, Value>> ilist)
    : type::ObjectWrapper<TemplateObjectType, TemplateObjectClass>(std::make_shared<TemplateObjectType>(ilist))
  {}

  static PairListObjectWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  PairListObjectWrapper& operator = (std::initializer_list<std::pair<Key, Value>> ilist) {
    this->m_ptr = std::make_shared<TemplateObjectType>(ilist);
    return *this;
  }
  
  Value& operator[] (const Key& key) const {
    auto& list = *(this->m_ptr.get());
    auto it = list.begin();
    while(it != list.end()) {
      if(it->first == key) {
        return it->second;
      }
      it ++;
    }
    list.push_back({key, nullptr});
    return list.back().second;
  }

  TemplateObjectType& operator*() const {
    return this->m_ptr.operator*();
  }

};

template<class Key, class Value>
using PairList = PairListObjectWrapper<
  typename Key::__Wrapper,
  typename Value::__Wrapper,
  __class::PairList<
    typename Key::__Wrapper,
    typename Value::__Wrapper
  >
>;

namespace __class {

template<class Key, class Value>
class PairList : public AbstractPairList {
private:

  class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
  public:

    void addPolymorphicItem(const type::Void& object, const type::Void& key, const type::Void& value) const override {
      const auto& map = object.staticCast<type::PairList<Key, Value>>();
      const auto& k = key.staticCast<Key>();
      const auto& v = value.staticCast<Value>();
      map->push_back({k, v});
    }

  };

private:

  static type::Void creator() {
    return type::Void(std::make_shared<std::list<std::pair<Key, Value>>>(), getType());
  }

  static Type createType() {
    Type type(__class::AbstractPairList::CLASS_ID, nullptr, &creator, nullptr, new PolymorphicDispatcher());
    type.params.push_back(Key::Class::getType());
    type.params.push_back(Value::Class::getType());
    return type;
  }

public:

  static Type* getType() {
    static Type type = createType();
    return &type;
  }

};

}

}}}}

#endif // oatpp_data_mapping_type_PairList_hpp
