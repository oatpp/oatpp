/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

// Defaults

#define DTO_INIT(TYPE_NAME, TYPE_EXTEND) \
\
public: \
  typedef TYPE_NAME Z__CLASS; \
  typedef oatpp::data::mapping::type::PtrWrapper<Z__CLASS, oatpp::data::mapping::type::__class::Object<Z__CLASS>> PtrWrapper; \
public: \
  OBJECT_POOL(DTO_OBJECT_POOL_##TYPE_NAME, TYPE_NAME, 32) \
  SHARED_OBJECT_POOL(SHARED_DTO_OBJECT_POOL_##TYPE_NAME, TYPE_NAME, 32) \
public: \
  TYPE_NAME() \
  { \
    Z__CLASS_EXTEND(Z__CLASS::Z__CLASS_GET_FIELDS_MAP(), TYPE_EXTEND::Z__CLASS_GET_FIELDS_MAP()); \
  } \
public: \
\
  static PtrWrapper createShared(){ \
    return PtrWrapper(SHARED_DTO_OBJECT_POOL_##TYPE_NAME::allocateShared()); \
  } \
\
  static oatpp::data::mapping::type::Type::Properties* Z__CLASS_GET_FIELDS_MAP(){ \
    static oatpp::data::mapping::type::Type::Properties map = oatpp::data::mapping::type::Type::Properties(); \
    return &map; \
  } \
\
  static oatpp::data::mapping::type::AbstractPtrWrapper Z__CLASS_OBJECT_CREATOR(){ \
    return oatpp::data::mapping::type::AbstractPtrWrapper(SHARED_DTO_OBJECT_POOL_##TYPE_NAME::allocateShared(), Z__CLASS_GET_TYPE()); \
  } \
\
  static oatpp::data::mapping::type::Type* Z__CLASS_GET_TYPE(){ \
    static oatpp::data::mapping::type::Type type(oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME, \
                                           &Z__CLASS_OBJECT_CREATOR, \
                                           Z__CLASS_GET_FIELDS_MAP()); \
    return &type; \
  }

// Fields

#define OATPP_MACRO_DTO_FIELD_0(TYPE, NAME, LIST) \
\
oatpp::data::mapping::type::Type::Property* Z__CLASS_FIELD_##NAME = \
  Z__CLASS_GET_FIELD_##NAME(static_cast<oatpp::base::Controllable*>(this), \
                            (oatpp::data::mapping::type::BasicPtrWrapper<oatpp::base::Controllable>*)(&NAME)); \
\
static oatpp::data::mapping::type::Type::Property* \
Z__CLASS_GET_FIELD_##NAME(oatpp::base::Controllable* _this, \
                          oatpp::data::mapping::type::BasicPtrWrapper<oatpp::base::Controllable>* _reg) { \
  static oatpp::data::mapping::type::Type::Property* field = \
      new oatpp::data::mapping::type::Type::Property(Z__CLASS_GET_FIELDS_MAP(), \
                                                     (v_int64) _reg - (v_int64) _this, \
                                                     #NAME, \
                                                     TYPE::Class::getType()); \
  return field; \
} \
\
TYPE NAME

#define OATPP_MACRO_DTO_FIELD_1(TYPE, NAME, LIST) \
\
oatpp::data::mapping::type::Type::Property* Z__CLASS_FIELD_##NAME = \
  Z__CLASS_GET_FIELD_##NAME(static_cast<oatpp::base::Controllable*>(this), \
                            (oatpp::data::mapping::type::BasicPtrWrapper<oatpp::base::Controllable>*)(&NAME)); \
\
static oatpp::data::mapping::type::Type::Property* \
Z__CLASS_GET_FIELD_##NAME(oatpp::base::Controllable* _this, \
                          oatpp::data::mapping::type::BasicPtrWrapper<oatpp::base::Controllable>* _reg) { \
  static oatpp::data::mapping::type::Type::Property* field = \
      new oatpp::data::mapping::type::Type::Property(Z__CLASS_GET_FIELDS_MAP(), \
                                                     (v_int64) _reg - (v_int64) _this, \
                                                     OATPP_MACRO_FIRSTARG LIST, \
                                                     TYPE::Class::getType()); \
  return field; \
} \
\
TYPE NAME

#define OATPP_MACRO_DTO_FIELD_(X, TYPE, NAME, LIST) OATPP_MACRO_DTO_FIELD_##X(TYPE, NAME, LIST)
#define OATPP_MACRO_DTO_FIELD__(X, TYPE, NAME, LIST) OATPP_MACRO_DTO_FIELD_(X, TYPE, NAME, LIST)
#define OATPP_MACRO_DTO_FIELD___(TYPE, NAME, LIST) OATPP_MACRO_DTO_FIELD__(OATPP_MACRO_HAS_ARGS LIST, TYPE, NAME, LIST)

#define DTO_FIELD(TYPE, NAME, ...) \
OATPP_MACRO_DTO_FIELD___(TYPE, NAME, (__VA_ARGS__))

///
