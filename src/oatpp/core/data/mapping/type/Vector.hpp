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

#ifndef oatpp_data_mapping_type_Vector_hpp
#define oatpp_data_mapping_type_Vector_hpp

#include "./Type.hpp"

#include <vector>
#include <initializer_list>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class AbstractVector {
  public:
    static const ClassId CLASS_ID;
  };

  template<class T>
  class Vector;

}

template<class T>
class VectorTemplate : public base::Countable {
  friend __class::Vector<T>;
public:
  typedef T TemplateParameter;
  typedef __class::Vector<T> TemplateClass;
public:

  class ObjectWrapper : public type::ObjectWrapper<VectorTemplate, TemplateClass> {
  public:

    OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(VectorTemplate, TemplateClass)

    ObjectWrapper(std::initializer_list<TemplateParameter> ilist)
      : type::ObjectWrapper<VectorTemplate, TemplateClass>(std::make_shared<VectorTemplate>(ilist))
    {}

    ObjectWrapper& operator = (std::initializer_list<TemplateParameter> ilist) {
      this->m_ptr = std::make_shared<VectorTemplate>(ilist);
      return *this;
    }

    std::vector<TemplateParameter>* operator->() const {
      return &this->m_ptr->m_vector;
    }

    std::vector<TemplateParameter>& collection() const {
      return this->m_ptr->m_vector;
    }

    TemplateParameter& operator[] (v_buff_usize index) const {
      return this->m_ptr->m_vector.operator [] (index);
    }

  };

public:
  typedef ObjectWrapper __Wrapper;
public:

  static AbstractObjectWrapper Z__CLASS_OBJECT_CREATOR(){
    return AbstractObjectWrapper(std::make_shared<VectorTemplate>(), Z__CLASS_GET_TYPE());
  }

  static Type* Z__CLASS_GET_TYPE(){
    static Type type(__class::AbstractVector::CLASS_ID, nullptr, &Z__CLASS_OBJECT_CREATOR);
    if(type.params.empty()){
      type.params.push_back(TemplateParameter::Class::getType());
    }
    return &type;
  }

private:
  std::vector<TemplateParameter> m_vector;
public:

  VectorTemplate() = default;

  VectorTemplate(std::initializer_list<TemplateParameter> ilist)
    : m_vector(ilist)
  {}

  virtual void addPolymorphicItem(const AbstractObjectWrapper& item){
    auto ptr = std::static_pointer_cast<typename TemplateParameter::ObjectType>(item.getPtr());
    m_vector.push_back(TemplateParameter(ptr, item.valueType));
  }

};

template<class T>
using Vector = VectorTemplate<typename T::__Wrapper>;

namespace __class {

template<class T>
class Vector : public AbstractVector {
public:

  static Type* getType(){
    static Type* type = static_cast<Type*>(type::Vector<T>::Z__CLASS_GET_TYPE());
    return type;
  }

};

}

}}}}

#endif // oatpp_data_mapping_type_Vector_hpp
