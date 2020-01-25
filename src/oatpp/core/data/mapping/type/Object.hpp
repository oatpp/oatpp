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

#include "./Primitive.hpp"
#include "./ListMap.hpp"
#include "./List.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

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
  public:

    /**
     * Get type describing this class.
     * @return - &id:oatpp::data::mapping::type::Type;
     */
    static Type* getType(){
      static Type* type = static_cast<Type*>(T::Z__CLASS_GET_TYPE());
      return type;
    }
    
  };
  
}

/**
 * Base class for all DTO objects.
 * For more info about Data Transfer Object (DTO) see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/).
 */
class Object : public oatpp::base::Countable {
public:
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
  using List = oatpp::data::mapping::type::List<T>;
  template <class Value>
  using Fields = oatpp::data::mapping::type::ListMap<String, Value>;
protected:
  
  static Type::Properties* Z__CLASS_EXTEND(Type::Properties* properties, Type::Properties* extensionProperties) {
    properties->pushFrontAll(extensionProperties);
    return properties;
  }
  
public:
  
  static oatpp::data::mapping::type::Type::Properties* Z__CLASS_GET_FIELDS_MAP(){
    static oatpp::data::mapping::type::Type::Properties map;
    return &map;
  }
  
};
  
}}}}

#endif /* oatpp_data_type_Object_hpp */
