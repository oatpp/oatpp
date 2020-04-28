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

#include <list>
#include <iterator>
#include <initializer_list>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class AbstractList {
  public:
    static const ClassId CLASS_ID;
  };

  template<class T>
  class List;

}

template<class T>
class ListTemplate : public base::Countable {
  friend __class::List<T>;
public:
  typedef T TemplateParameter;
  typedef __class::List<T> TemplateClass;
public:

  class ObjectWrapper : public type::ObjectWrapper<ListTemplate, TemplateClass> {
  public:

    OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(ListTemplate, TemplateClass)

    ObjectWrapper(std::initializer_list<TemplateParameter> ilist)
      : type::ObjectWrapper<ListTemplate, TemplateClass>(std::make_shared<ListTemplate>(ilist))
    {}

    ObjectWrapper& operator = (std::initializer_list<TemplateParameter> ilist) {
      this->m_ptr = std::make_shared<ListTemplate>(ilist);
      return *this;
    }

    std::list<TemplateParameter>* operator->() const {
      return &this->m_ptr->m_list;
    }

    std::list<TemplateParameter>& collection() const {
      return this->m_ptr->m_list;
    }

  };

public:
  typedef ObjectWrapper __Wrapper;
public:

  static Void Z__CLASS_OBJECT_CREATOR(){
    return Void(std::make_shared<ListTemplate>(), Z__CLASS_GET_TYPE());
  }

  static Type* Z__CLASS_GET_TYPE(){
    static Type type(__class::AbstractList::CLASS_ID, nullptr, &Z__CLASS_OBJECT_CREATOR);
    if(type.params.empty()){
      type.params.push_back(TemplateParameter::Class::getType());
    }
    return &type;
  }

private:
  std::list<TemplateParameter> m_list;
public:

  ListTemplate() = default;

  ListTemplate(std::initializer_list<TemplateParameter> ilist)
    : m_list(ilist)
  {}

  static ObjectWrapper createShared() {
    return std::make_shared<ListTemplate>();
  }

  virtual void addPolymorphicItem(const Void& item){
    auto ptr = std::static_pointer_cast<typename TemplateParameter::ObjectType>(item.getPtr());
    m_list.push_back(TemplateParameter(ptr, item.valueType));
  }

  TemplateParameter getItemByIndex(v_uint32 index) {
    auto it = m_list.begin();
    std::advance(it, index);
    return *it;
  }

};

template<class T>
using List = ListTemplate<typename T::__Wrapper>;

namespace __class {

template<class T>
class List : public AbstractList {
public:

  static Type* getType(){
    static Type* type = static_cast<Type*>(type::List<T>::Z__CLASS_GET_TYPE());
    return type;
  }

};

}

}}}}

#endif // oatpp_data_mapping_type_List_hpp
