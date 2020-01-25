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

/**[info]
 * This file contains "defines" for DTO code generating macro. <br>
 * Usage:<br>
 *
 * ```cpp
 * #include OATPP_CODEGEN_BEGIN(DTO)
 * ...
 * // Generated Endpoints.
 * ...
 * #include OATPP_CODEGEN_END(DTO)
 * ```
 *
 *
 * *For details see:*
 * <ul>
 *   <li>[Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/)</li>
 *   <li>&id:oatpp::data::mapping::type::Object;</li>
 * </ul>
 */

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

// Defaults

/**
 * Codegen macoro to be used in classes extending &id:oatpp::data::mapping::type::Object; to generate required fields/methods/constructors for DTO object.
 * @param TYPE_NAME - name of the DTO class.
 * @param TYPE_EXTEND - name of the parent DTO class. If DTO extends &id:oatpp::data::mapping::type::Object; TYPE_EXETENDS should be `Object`.
 */
#define DTO_INIT(TYPE_NAME, TYPE_EXTEND) \
public: \
  typedef TYPE_NAME Z__CLASS; \
  typedef TYPE_EXTEND Z__CLASS_EXTENDED; \
  typedef oatpp::data::mapping::type::ObjectWrapper<Z__CLASS, oatpp::data::mapping::type::__class::Object<Z__CLASS>> ObjectWrapper; \
public: \
  OBJECT_POOL(DTO_OBJECT_POOL_##TYPE_NAME, TYPE_NAME, 32) \
  SHARED_OBJECT_POOL(SHARED_DTO_OBJECT_POOL_##TYPE_NAME, TYPE_NAME, 32) \
protected: \
  oatpp::data::mapping::type::Type::Properties* Z__CLASS_INIT_FIELDS(oatpp::data::mapping::type::Type::Properties* properties, \
                                                                     oatpp::data::mapping::type::Type::Properties* extensionProperties) { \
    static oatpp::data::mapping::type::Type::Properties* ptr = Z__CLASS_EXTEND(properties, extensionProperties); \
    return ptr; \
  } \
public: \
  TYPE_NAME() \
  { \
    Z__CLASS_INIT_FIELDS(Z__CLASS::Z__CLASS_GET_FIELDS_MAP(), TYPE_EXTEND::Z__CLASS_GET_FIELDS_MAP()); \
  } \
public: \
\
  static ObjectWrapper createShared(){ \
    return ObjectWrapper(SHARED_DTO_OBJECT_POOL_##TYPE_NAME::allocateShared()); \
  } \
\
  static oatpp::data::mapping::type::Type::Properties* Z__CLASS_GET_FIELDS_MAP(){ \
    static oatpp::data::mapping::type::Type::Properties map = oatpp::data::mapping::type::Type::Properties(); \
    return &map; \
  } \
\
  static oatpp::data::mapping::type::AbstractObjectWrapper Z__CLASS_OBJECT_CREATOR(){ \
    return oatpp::data::mapping::type::AbstractObjectWrapper(SHARED_DTO_OBJECT_POOL_##TYPE_NAME::allocateShared(), Z__CLASS_GET_TYPE()); \
  } \
\
  static oatpp::data::mapping::type::Type* Z__CLASS_GET_TYPE(){ \
    static oatpp::data::mapping::type::Type type(oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID, \
                                                 #TYPE_NAME, \
                                                 &Z__CLASS_OBJECT_CREATOR, \
                                                 Z__CLASS_GET_FIELDS_MAP()); \
    return &type; \
  }

// Fields

#define OATPP_MACRO_DTO_FIELD_1(TYPE, NAME) \
\
oatpp::data::mapping::type::Type::Property* Z__CLASS_FIELD_##NAME = \
  Z__CLASS_GET_FIELD_##NAME(static_cast<oatpp::base::Countable*>(this), \
                            (oatpp::data::mapping::type::AbstractObjectWrapper*)(&NAME)); \
\
static oatpp::data::mapping::type::Type::Property* \
Z__CLASS_GET_FIELD_##NAME(oatpp::base::Countable* _this, \
                          oatpp::data::mapping::type::AbstractObjectWrapper* _reg) { \
  static oatpp::data::mapping::type::Type::Property* field = \
      new oatpp::data::mapping::type::Type::Property(Z__CLASS_GET_FIELDS_MAP(), \
                                                     (v_int64) _reg - (v_int64) _this, \
                                                     #NAME, \
                                                     TYPE::Class::getType()); \
  return field; \
} \
\
TYPE NAME

#define OATPP_MACRO_DTO_FIELD_2(TYPE, NAME, QUALIFIER) \
\
oatpp::data::mapping::type::Type::Property* Z__CLASS_FIELD_##NAME = \
  Z__CLASS_GET_FIELD_##NAME(static_cast<oatpp::base::Countable*>(this), \
                            (oatpp::data::mapping::type::AbstractObjectWrapper*)(&NAME)); \
\
static oatpp::data::mapping::type::Type::Property* \
Z__CLASS_GET_FIELD_##NAME(oatpp::base::Countable* _this, \
                          oatpp::data::mapping::type::AbstractObjectWrapper* _reg) { \
  static oatpp::data::mapping::type::Type::Property* field = \
      new oatpp::data::mapping::type::Type::Property(Z__CLASS_GET_FIELDS_MAP(), \
                                                     (v_int64) _reg - (v_int64) _this, \
                                                     QUALIFIER, \
                                                     TYPE::Class::getType()); \
  return field; \
} \
\
TYPE NAME

/**
 * Codegen macro to generate fields of DTO object.
 * @param TYPE - type of the field.
 * @param NAME - name of the field.
 * @param QUALIFIER_NAME - additional (optional) field to specify serialized name of the field. If not specified it will be same as NAME.
 */
#define DTO_FIELD(TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(OATPP_MACRO_DTO_FIELD_, (__VA_ARGS__)) (TYPE, __VA_ARGS__))
