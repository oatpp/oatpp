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

#ifndef oatpp_Types_hpp
#define oatpp_Types_hpp

#include "oatpp/data/type/Object.hpp"
#include "oatpp/data/mapping/Tree.hpp"

namespace oatpp {

  /**
   * &id:oatpp::data::type::Type;.
   */
  typedef oatpp::data::type::Type Type;

  /**
   * &id:oatpp::data::type::ClassId;.
   */
  typedef oatpp::data::type::ClassId ClassId;

  /**
   * ObjectWrapper.
   */
  template <class T, class Clazz = oatpp::data::type::__class::Void>
  using ObjectWrapper = oatpp::data::type::ObjectWrapper<T, Clazz>;

  /**
   * Mapping enabled &id:oatpp::data::mapping::Tree;
   * `mapping::Tree` and `type::Tree` are inter-mappable;
   */
  typedef oatpp::data::type::Tree Tree;

  /**
   * ObjectWrapper over the `void*`.
   */
  typedef oatpp::data::type::Void Void;

  /**
   * `Any` - container for mapping-enabled types.
   * &id:oatpp::data::type::Any;
   */
  typedef oatpp::data::type::Any Any;

  /**
   * Mapping-Enabled String type. &id:oatpp::data::type::String; <br>
   * For `oatpp::String` methods see `std::string`
   */
  typedef oatpp::data::type::String String;

  /**
   * Mapping-Enabled 8-bits int. Can hold nullptr value. &id:oatpp::data::type::Int8;
   */
  typedef oatpp::data::type::Int8 Int8;

  /**
   * Mapping-Enabled 8-bits unsigned int. Can hold nullptr value. &id:oatpp::data::type::UInt8;
   */
  typedef oatpp::data::type::UInt8 UInt8;

  /**
   * Mapping-Enabled 16-bits int. Can hold nullptr value. &id:oatpp::data::type::Int16;
   */
  typedef oatpp::data::type::Int16 Int16;

  /**
   * Mapping-Enabled 16-bits unsigned int. Can hold nullptr value. &id:oatpp::data::type::UInt16;
   */
  typedef oatpp::data::type::UInt16 UInt16;

  /**
   * Mapping-Enabled 32-bits int. Can hold nullptr value. &id:oatpp::data::type::Int32;
   */
  typedef oatpp::data::type::Int32 Int32;

  /**
   * Mapping-Enabled 32-bits unsigned int. Can hold nullptr value. &id:oatpp::data::type::UInt32;
   */
  typedef oatpp::data::type::UInt32 UInt32;

  /**
   * Mapping-Enabled 64-bits int. Can hold nullptr value. &id:oatpp::data::type::Int64;
   */
  typedef oatpp::data::type::Int64 Int64;

  /**
   * Mapping-Enabled 64-bits unsigned int. Can hold nullptr value. &id:oatpp::data::type::UInt64;
   */
  typedef oatpp::data::type::UInt64 UInt64;

  /**
   * Mapping-Enabled 32-bits float. Can hold nullptr value. &id:oatpp::data::type::Float32;
   */
  typedef oatpp::data::type::Float32 Float32;

  /**
   * Mapping-Enabled 64-bits float (double). Can hold nullptr value. &id:oatpp::data::type::Float64;
   */
  typedef oatpp::data::type::Float64 Float64;

  /**
   * Mapping-Enabled Boolean. Can hold nullptr value. &id:oatpp::data::type::Boolean;
   */
  typedef oatpp::data::type::Boolean Boolean;

  /**
   * Base class for all Object-like Mapping-enabled structures. &id:oatpp::data::type::BaseObject;
   */
  typedef oatpp::data::type::BaseObject BaseObject;

  /**
   * Base class for all DTO objects. &id:oatpp::data::type::DTO;
   */
  typedef oatpp::data::type::DTO DTO;

  /**
   * Mapping-Enabled DTO Object. &id:oatpp::data::type::DTOWrapper;
   */
  template <class T>
  using Object = oatpp::data::type::DTOWrapper<T>;

  /**
   * Mapping-Enabled Enum. &id:oatpp::data::type::Enum;
   */
  template <class T>
  using Enum = oatpp::data::type::Enum<T>;

  /**
   * Mapping-Enabled Vector. &id:oatpp::data::type::Vector;
   */
  template <class T>
  using Vector = oatpp::data::type::Vector<T>;

  /**
   * Abstract Vector.
   */
  typedef oatpp::data::type::AbstractVector AbstractVector;

  /**
   * Mapping-Enabled List. &id:oatpp::data::type::List;
   */
  template <class T>
  using List = oatpp::data::type::List<T>;

  /**
   * Abstract List.
   */
  typedef oatpp::data::type::AbstractList AbstractList;

  /**
   * Mapping-Enabled UnorderedSet. &id:oatpp::data::type::UnorderedSet;
   */
  template <class T>
  using UnorderedSet = oatpp::data::type::UnorderedSet<T>;

  /**
   * Abstract UnorderedSet.
   */
  typedef oatpp::data::type::AbstractUnorderedSet AbstractUnorderedSet;

  /**
   * Mapping-Enabled PairList<Key, Value>. &id:oatpp::data::type::PairList;
   */
  template <class Key, class Value>
  using PairList = oatpp::data::type::PairList<Key, Value>;

  /**
   * Mapping-Enabled PairList<String, Value>. &id:oatpp::data::type::PairList;
   */
  template <class Value>
  using Fields = oatpp::PairList<String, Value>;

  /**
   * Abstract Fields
   */
  typedef Fields<oatpp::Void> AbstractFields;

  /**
   * Mapping-Enabled UnorderedMap<Key, Value>. &id:oatpp::data::type::UnorderedMap;.
   */
  template <class Key, class Value>
  using UnorderedMap = oatpp::data::type::UnorderedMap<Key, Value>;

  /**
   * Mapping-Enabled UnorderedMap<String, Value>. &id:oatpp::data::type::UnorderedMap;.
   */
  template <class Value>
  using UnorderedFields = oatpp::UnorderedMap<String, Value>;

  /**
   * Abstract UnorderedFields
   */
  typedef UnorderedFields<oatpp::Void> AbstractUnorderedFields;
  
}

#endif /* oatpp_Types_hpp */
