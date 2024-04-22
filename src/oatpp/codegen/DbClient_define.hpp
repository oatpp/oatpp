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

#include "oatpp/macro/basic.hpp"
#include "oatpp/macro/codegen.hpp"

#define OATPP_MACRO_DB_CLIENT_PARAM_TYPE(MACRO, TYPE, PARAM_LIST) TYPE
#define OATPP_MACRO_DB_CLIENT_PARAM_NAME(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG PARAM_LIST
#define OATPP_MACRO_DB_CLIENT_PARAM_TYPE_STR(MACRO, TYPE, PARAM_LIST) #TYPE
#define OATPP_MACRO_DB_CLIENT_PARAM_NAME_STR(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG_STR PARAM_LIST

#define OATPP_MACRO_DB_CLIENT_PARAM_MACRO(MACRO, TYPE, PARAM_LIST) MACRO(TYPE, PARAM_LIST)
#define OATPP_MACRO_DB_CLIENT_PREPARE_MACRO(MACRO, VAL) MACRO(VAL)

#define OATPP_MACRO_DB_CLIENT_PARAM_MACRO_TYPE(MACRO, TYPE, PARAM_LIST) MACRO ##_TYPE(TYPE, PARAM_LIST)

#define OATPP_MACRO_DB_CLIENT_PARAM(MACRO, TYPE, PARAM_LIST) (MACRO, TYPE, PARAM_LIST)
#define OATPP_MACRO_DB_CLIENT_PREPARE(MACRO, VAL) (MACRO, VAL)

#define PARAM(TYPE, ...)        OATPP_MACRO_DB_CLIENT_PARAM(OATPP_MACRO_DB_CLIENT_PARAM_PARAM, TYPE, (__VA_ARGS__))
#define PREPARE(VAL)            OATPP_MACRO_DB_CLIENT_PREPARE(OATPP_MACRO_DB_CLIENT_PARAM_PREPARE, VAL)

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(MACRO, TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (TYPE, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

// PARAM MACRO USE-CASE

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_PUT_DECL(X) \
const OATPP_MACRO_DB_CLIENT_PARAM_TYPE X & OATPP_MACRO_DB_CLIENT_PARAM_NAME X,

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_PUT_TYPE(X) \
OATPP_MACRO_DB_CLIENT_PARAM_MACRO_TYPE X

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_PUT_BODY(X) \
OATPP_MACRO_DB_CLIENT_PARAM_MACRO X

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

// PREPARE MACRO USE-CASE

#define OATPP_MACRO_DB_CLIENT_PARAM_PREPARE_PUT_DECL(X)

#define OATPP_MACRO_DB_CLIENT_PARAM_PREPARE_PUT_TYPE(X) \
OATPP_MACRO_DB_CLIENT_PREPARE_MACRO X

#define OATPP_MACRO_DB_CLIENT_PARAM_PREPARE_PUT_BODY(X)

// PREPARE MACRO

#define OATPP_MACRO_DB_CLIENT_PARAM_PREPARE(VAL) \
__prepare = VAL;

// PARAMS USE-CASE

#define OATPP_MACRO_PARAM_USECASE_DECL(MACRO, ...) MACRO ## _PUT_DECL((MACRO, __VA_ARGS__))
#define OATPP_MACRO_PARAM_USECASE_TYPE(MACRO, ...) MACRO ## _PUT_TYPE((MACRO, __VA_ARGS__))
#define OATPP_MACRO_PARAM_USECASE_BODY(MACRO, ...) MACRO ## _PUT_BODY((MACRO, __VA_ARGS__))

// FOR EACH

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT_DECL(INDEX, COUNT, X) \
OATPP_MACRO_PARAM_USECASE_DECL X

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT_TYPE(INDEX, COUNT, X) \
OATPP_MACRO_PARAM_USECASE_TYPE X

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_PARAM_USECASE_BODY X

// QUERY MACRO

#define OATPP_QUERY_0(NAME, QUERY_TEXT) \
const oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_##NAME = \
  this->parseQueryTemplate(#NAME, QUERY_TEXT, {}, false); \
\
std::shared_ptr<oatpp::orm::QueryResult> NAME(const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection = nullptr) { \
  std::unordered_map<oatpp::String, oatpp::Void> __params; \
  return this->execute(Z_QUERY_TEMPLATE_##NAME, __params, connection); \
}


#define OATPP_QUERY_1(NAME, QUERY_TEXT, ...) \
\
oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_CREATOR_##NAME() { \
  bool __prepare = false; \
  oatpp::orm::Executor::ParamsTypeMap map;  \
  \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT_TYPE, __VA_ARGS__) \
  \
  return this->parseQueryTemplate(#NAME, QUERY_TEXT, map, __prepare); \
} \
\
const oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_##NAME = Z_QUERY_TEMPLATE_CREATOR_##NAME(); \
\
std::shared_ptr<oatpp::orm::QueryResult> NAME( \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT_DECL, __VA_ARGS__) \
  const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection = nullptr \
) { \
  std::unordered_map<oatpp::String, oatpp::Void> __params; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT, __VA_ARGS__) \
  return this->execute(Z_QUERY_TEMPLATE_##NAME, __params, connection); \
}

// Chooser

#define OATPP_QUERY_MACRO_0(NAME, QUERY_TEXT) \
OATPP_QUERY_0(NAME, QUERY_TEXT)

#define OATPP_QUERY_MACRO_1(NAME, QUERY_TEXT, ...) \
OATPP_QUERY_1(NAME, QUERY_TEXT, __VA_ARGS__)


#define QUERY(NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_QUERY_MACRO_, (__VA_ARGS__)) (NAME, __VA_ARGS__))
