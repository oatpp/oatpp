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

#ifndef oatpp_data_type_Object_hpp
#define oatpp_data_type_Object_hpp

#include "./Type.hpp"

#include "./Any.hpp"
#include "./Primitive.hpp"
#include "./Enum.hpp"
#include "./UnorderedMap.hpp"
#include "./PairList.hpp"
#include "./List.hpp"
#include "./Vector.hpp"
#include "./UnorderedSet.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

#include <type_traits>

namespace oatpp { namespace data { namespace mapping { namespace type {
  
namespace __class {

  /**
   * AbstractObject class.
   */
  class AbstractObject {
  public:
    static const ClassId CLASS_ID;
  };

  /**
   * Template for Object class of type T.
   * @tparam T - object type.
   */
  template<class T>
  class Object : public AbstractObject {
  private:

    static type::Void creator() {
      return type::Void(std::make_shared<T>(), getType());
    }

    static type::Type::Properties* initProperties() {
      T obj; // initializer;
      T::Z__CLASS_EXTEND(T::Z__CLASS::Z__CLASS_GET_FIELDS_MAP(), T::Z__CLASS_EXTENDED::Z__CLASS_GET_FIELDS_MAP());
      return T::Z__CLASS::Z__CLASS_GET_FIELDS_MAP();
    }

    static const Type::Properties* propertiesGetter() {
      static type::Type::Properties* properties = initProperties();
      return properties;
    }

  public:

    /**
     * Get type describing this class.
     * @return - &id:oatpp::data::mapping::type::Type;
     */
    static Type* getType() {
      static Type* type = new Type(CLASS_ID, T::Z__CLASS_TYPE_NAME(), creator, propertiesGetter);
      return type;
    }
    
  };
  
}

/**
 * ObjectWrapper for &l:DTO;. AKA `oatpp::Object<T>`.
 * @tparam ObjT - class extended from &l:DTO;.
 */
template<class ObjT>
class DTOWrapper : public ObjectWrapper<ObjT, __class::Object<ObjT>> {
public:
  typedef ObjT TemplateObjectType;
  typedef __class::Object<ObjT> TemplateObjectClass;
public:

  OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(DTOWrapper, TemplateObjectType, TemplateObjectClass)

  static DTOWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (T){
    return this->m_ptr.get() == nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (T){
    return this->m_ptr.get() != nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, DTOWrapper>::value, void>::type
  >
  inline bool operator == (const T &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, DTOWrapper>::value, void>::type
  >
  inline bool operator != (const T &other) const {
    return !operator == (other);
  }

};

/**
 * Base class for all DTO objects.
 * For more info about Data Transfer Object (DTO) see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/).
 */
class DTO : public oatpp::base::Countable {
  template<class T>
  friend class __class::Object;
public:
  typedef oatpp::data::mapping::type::Void Void;
  typedef oatpp::data::mapping::type::Any Any;
  typedef oatpp::data::mapping::type::String String;
  typedef oatpp::data::mapping::type::Int8 Int8;
  typedef oatpp::data::mapping::type::UInt8 UInt8;
  typedef oatpp::data::mapping::type::Int16 Int16;
  typedef oatpp::data::mapping::type::UInt16 UInt16;
  typedef oatpp::data::mapping::type::Int32 Int32;
  typedef oatpp::data::mapping::type::UInt32 UInt32;
  typedef oatpp::data::mapping::type::Int64 Int64;
  typedef oatpp::data::mapping::type::UInt64 UInt64;
  typedef oatpp::data::mapping::type::Float32 Float32;
  typedef oatpp::data::mapping::type::Float64 Float64;
  typedef oatpp::data::mapping::type::Boolean Boolean;

  template <class T>
  using Object = DTOWrapper<T>;

  template <class T>
  using Enum = oatpp::data::mapping::type::Enum<T>;

  template <class T>
  using Vector = oatpp::data::mapping::type::Vector<T>;

  template <class T>
  using UnorderedSet = oatpp::data::mapping::type::UnorderedSet<T>;

  template <class T>
  using List = oatpp::data::mapping::type::List<T>;

  template <class Value>
  using Fields = oatpp::data::mapping::type::PairList<String, Value>;

  template <class Value>
  using UnorderedFields = oatpp::data::mapping::type::UnorderedMap<String, Value>;

private:
  
  static Type::Properties* Z__CLASS_EXTEND(Type::Properties* properties, Type::Properties* extensionProperties) {
    properties->pushFrontAll(extensionProperties);
    return properties;
  }

  static oatpp::data::mapping::type::Type::Properties* Z__CLASS_GET_FIELDS_MAP(){
    static oatpp::data::mapping::type::Type::Properties map;
    return &map;
  }
  
public:

  virtual v_uint64 defaultHashCode() const {
    return (v_uint64) reinterpret_cast<v_buff_usize>(this);
  }

  virtual bool defaultEquals(const DTO& other) const {
    return this == &other;
  }

  v_uint64 hashCode() const {
    return defaultHashCode();
  }

  bool operator==(const DTO& other) const {
    return defaultEquals(other);
  }

};
  
}}}}

namespace std {

  template<class T>
  struct hash<oatpp::data::mapping::type::DTOWrapper<T>> {

    typedef oatpp::data::mapping::type::DTOWrapper<T> argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const &ow) const noexcept {
      if(ow) {
        return ow->hashCode();
      }
      return 0;
    }

  };

}

#endif /* oatpp_data_type_Object_hpp */
