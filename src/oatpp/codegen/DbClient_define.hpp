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

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

#define OATPP_MACRO_DB_CLIENT_PARAM_TYPE(MACRO, TYPE, PARAM_LIST) TYPE
#define OATPP_MACRO_DB_CLIENT_PARAM_NAME(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG PARAM_LIST
#define OATPP_MACRO_DB_CLIENT_PARAM_TYPE_STR(MACRO, TYPE, PARAM_LIST) #TYPE
#define OATPP_MACRO_DB_CLIENT_PARAM_NAME_STR(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG_STR PARAM_LIST

#define OATPP_MACRO_DB_CLIENT_PARAM_MACRO(MACRO, TYPE, PARAM_LIST) MACRO(TYPE, PARAM_LIST)
#define OATPP_MACRO_DB_CLIENT_PARAM_MACRO_TYPE(MACRO, TYPE, PARAM_LIST) MACRO ##_TYPE(TYPE, PARAM_LIST)

#define OATPP_MACRO_DB_CLIENT_PARAM(MACRO, TYPE, PARAM_LIST) (MACRO, TYPE, PARAM_LIST)

#define PARAM(TYPE, ...)        OATPP_MACRO_DB_CLIENT_PARAM(OATPP_MACRO_DB_CLIENT_PARAM_PARAM, TYPE, (__VA_ARGS__))
#define PARAMS_DTO(TYPE, ...)   OATPP_MACRO_DB_CLIENT_PARAM(OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO, TYPE, (__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(MACRO, TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (TYPE, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

// PARAM MACRO

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_1(TYPE, NAME) \
__params.insert({#NAME, NAME});

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_2(TYPE, NAME, QUALIFIER) \
__params.insert({QUALIFIER, NAME});

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM(TYPE, PARAM_LIST) \
OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(OATPP_MACRO_DB_CLIENT_PARAM_PARAM_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// PARAM_TYPE MACRO

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_TYPE_1(TYPE, NAME) \
map.insert({#NAME, TYPE::Class::getType()});

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_TYPE_2(TYPE, NAME, QUALIFIER) \
map.insert({QUALIFIER, TYPE::Class::getType()});

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_TYPE(TYPE, PARAM_LIST) \
OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(OATPP_MACRO_DB_CLIENT_PARAM_PARAM_TYPE_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)


// PARAMS_DTO MACRO

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_1(TYPE, NAME) \
params_putDtoFields(__params, NAME, #NAME);

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_2(TYPE, NAME, QUALIFIER) \
params_putDtoFields(__params, NAME, QUALIFIER);

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO(TYPE, PARAM_LIST) \
OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// PARAM_TYPE MACRO

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_TYPE_1(TYPE, NAME) \
types_putDtoFields(map, TYPE::Class::getType(), #NAME);

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_TYPE_2(TYPE, NAME, QUALIFIER) \
types_putDtoFields(map, TYPE::Class::getType(), QUALIFIER);

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_TYPE(TYPE, PARAM_LIST) \
OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(OATPP_MACRO_DB_CLIENT_PARAM_PARAMS_DTO_TYPE_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)


// FOR EACH

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT_DECL(INDEX, COUNT, X) \
const OATPP_MACRO_DB_CLIENT_PARAM_TYPE X & OATPP_MACRO_DB_CLIENT_PARAM_NAME X,

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT_TYPE(INDEX, COUNT, X) \
OATPP_MACRO_DB_CLIENT_PARAM_MACRO_TYPE X

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_DB_CLIENT_PARAM_MACRO X

// QUERY MACRO

#define OATPP_QUERY_0(NAME, QUERY_TEXT) \
const oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_##NAME = \
  m_executor->parseQueryTemplate(#NAME, QUERY_TEXT, {}); \
\
std::shared_ptr<oatpp::orm::QueryResult> NAME(const std::shared_ptr<oatpp::orm::Connection>& connection = nullptr) { \
  std::unordered_map<oatpp::String, oatpp::Void> __params; \
  return m_executor->execute(Z_QUERY_TEMPLATE_##NAME, __params, connection); \
}


#define OATPP_QUERY_1(NAME, QUERY_TEXT, ...) \
\
static oatpp::orm::Executor::ParamsTypeMap Z_QUERY_TEMPLATE_PARAMS_TYPE_MAP_CREATOR_##NAME() { \
  oatpp::orm::Executor::ParamsTypeMap map;  \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT_TYPE, __VA_ARGS__) \
  return map; \
} \
\
const oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_##NAME = \
  m_executor->parseQueryTemplate(#NAME, QUERY_TEXT, Z_QUERY_TEMPLATE_PARAMS_TYPE_MAP_CREATOR_##NAME()); \
\
std::shared_ptr<oatpp::orm::QueryResult> NAME( \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT_DECL, __VA_ARGS__) \
  const std::shared_ptr<oatpp::orm::Connection>& connection = nullptr \
) { \
  std::unordered_map<oatpp::String, oatpp::Void> __params; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT, __VA_ARGS__) \
  return m_executor->execute(Z_QUERY_TEMPLATE_##NAME, __params, connection); \
}

// Chooser

#define OATPP_QUERY_MACRO_0(NAME, QUERY_TEXT) \
OATPP_QUERY_0(NAME, QUERY_TEXT)

#define OATPP_QUERY_MACRO_1(NAME, QUERY_TEXT, ...) \
OATPP_QUERY_1(NAME, QUERY_TEXT, __VA_ARGS__)


#define QUERY(NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_QUERY_MACRO_, (__VA_ARGS__)) (NAME, __VA_ARGS__))
