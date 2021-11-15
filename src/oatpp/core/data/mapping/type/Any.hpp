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

#ifndef oatpp_data_mapping_type_Any_hpp
#define oatpp_data_mapping_type_Any_hpp

#include "./Type.hpp"

#include "oatpp/core/base/Countable.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  /**
   * Any class.
   */
  class Any {
  public:

    /**
     * Class Id.
     */
    static const ClassId CLASS_ID;

    static Type *getType() {
      static Type type(CLASS_ID);
      return &type;
    }

  };

}

class AnyHandle : public base::Countable {
public:

  AnyHandle(const std::shared_ptr<void>& objPtr, const Type* const objType)
    : ptr(objPtr)
    , type(objType)
  {}

  std::shared_ptr<void> ptr;
  const Type* const type;

};

/**
 * Any - ObjectWrapper to hold Any oatpp mapping-enabled type.
 */
class Any : public ObjectWrapper<AnyHandle, __class::Any>{
public:

  /**
   * Default constructor.
   */
  Any();

  /**
   * Nullptr constructor.
   */
  Any(std::nullptr_t);

  /**
   * Copy constructor.
   * @param other - other Any.
   */
  Any(const Any& other);

  /**
   * Move constructor.
   * @param other
   */
  Any(Any&& other);

  Any(const std::shared_ptr<AnyHandle>& handle, const Type* const type);

  /**
   * Constructor.
   * @tparam T - Underlying type of ObjectWrapper.
   * @tparam C - __class of ObjectWrapper.
   * @param polymorph - any ObjectWrapper.
   */
  template<class T, class C>
  Any(const ObjectWrapper<T, C>& polymorph)
    : ObjectWrapper(std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.getValueType()), __class::Any::getType())
  {}

  /**
   * Store any ObjectWrapper in Any.
   * @tparam T
   * @tparam C
   * @param polymorph - ObjectWrapper. Ex.: `oatpp::String`, `oatpp::List<...>`, etc.
   */
  template<class T, class C>
  void store(const ObjectWrapper<T, C>& polymorph) {
    m_ptr = std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.getValueType());
  }

  /**
   * Get `Type` of the stored object.
   * @return - &id:oatpp::data::mapping::type::Type;.
   */
  const Type* getStoredType() const;

  /**
   * Retrieve stored object of type `type`.
   * @param type - &id:oatpp::Type;
   * @return - &id:oatpp::Void;.
   * @throws - `std::runtime_error` - if stored type and type requested do not match.
   */
  Void retrieve(const Type* type) const;

  /**
   * Retrieve stored object.
   * @tparam WrapperType - type of the object to retrieve.
   * @return - ObjectWrapper of type - `WrapperType`.
   * @throws - `std::runtime_error` - if stored type and type requested (`WrapperType`) do not match.
   */
  template<class WrapperType>
  WrapperType retrieve() const {
    const auto& v = retrieve(WrapperType::Class::getType());
    return WrapperType(std::static_pointer_cast<typename WrapperType::ObjectType>(v.getPtr()), WrapperType::Class::getType());
  }

  Any& operator=(std::nullptr_t);

  Any& operator=(const Any& other);
  Any& operator=(Any&& other);

  template<class T, class C>
  Any& operator=(const ObjectWrapper<T, C>& polymorph) {
    m_ptr = std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.getValueType());
    return *this;
  }

  bool operator == (std::nullptr_t) const;
  bool operator != (std::nullptr_t) const;

  bool operator == (const Any& other) const;
  bool operator != (const Any& other) const;

};

}}}}

#endif //oatpp_data_mapping_type_Any_hpp
