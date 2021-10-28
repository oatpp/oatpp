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
#include <initializer_list>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  /**
   * Abstract list class.
   */
  class AbstractList {
  public:
    /**
     * Class Id.
     */
    static const ClassId CLASS_ID;
  public:

    /**
     * Polymorphic Dispatcher
     */
    class PolymorphicDispatcher {
    public:

      virtual ~PolymorphicDispatcher() = default;

      virtual type::Void createObject() const = 0;

      /**
       * Add item.
       * @param object - List to add item to.
       * @param item - Item to add.
       */
      virtual void addPolymorphicItem(const type::Void& object, const type::Void& item) const = 0;
    };

  };

  template<class T>
  class List;

}

/**
 * `ObjectWrapper` over `std::list<T>`
 * @tparam T - Item `ObjectWrapper` type.
 * @tparam C - Class.
 */
template<class T, class C>
class ListObjectWrapper : public type::ObjectWrapper<std::list<T>, C> {
public:
  typedef std::list<T> TemplateObjectType;
  typedef C TemplateObjectClass;
public:

  OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(ListObjectWrapper, TemplateObjectType, TemplateObjectClass)

  ListObjectWrapper(std::initializer_list<T> ilist)
    : type::ObjectWrapper<TemplateObjectType, TemplateObjectClass>(std::make_shared<TemplateObjectType>(ilist))
  {}

  static ListObjectWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  ListObjectWrapper& operator = (std::initializer_list<T> ilist) {
    this->m_ptr = std::make_shared<TemplateObjectType>(ilist);
    return *this;
  }

  T& operator[] (v_buff_usize index) const {
    auto it = this->m_ptr->begin();
    std::advance(it, index);
    return *it;
  }

  TemplateObjectType& operator*() const {
    return this->m_ptr.operator*();
  }

};

/**
 * Mapping-Enabled List. See - &l:ListObjectWrapper;.
 */
template<class T>
using List = ListObjectWrapper<T, __class::List<T>>;

typedef ListObjectWrapper<type::Void, __class::AbstractList> AbstractList;

namespace __class {

  template<class T>
  class List : public AbstractList {
  public:

    class PolymorphicDispatcher : public AbstractList::PolymorphicDispatcher {
    public:

      type::Void createObject() const override {
        return type::Void(std::make_shared<std::list<T>>(), getType());
      }

      void addPolymorphicItem(const type::Void& object, const type::Void& item) const override {
        const auto& list = object.staticCast<type::List<T>>();
        const auto& listItem = item.staticCast<T>();
        list->push_back(listItem);
      }

    };

  private:

    static Type createType() {
      Type type(__class::AbstractList::CLASS_ID, nullptr, new PolymorphicDispatcher());
      type.params.push_back(T::Class::getType());
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

#endif // oatpp_data_mapping_type_List_hpp
