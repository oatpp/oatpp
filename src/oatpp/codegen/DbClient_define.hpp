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

#define OATPP_MACRO_DB_CLIENT_PARAM_MACRO(MACRO, TYPE, PARAM_LIST) MACRO(TYPE, PARAM_LIST)
#define OATPP_MACRO_DB_CLIENT_PARAM_TYPE(MACRO, TYPE, PARAM_LIST) const TYPE&
#define OATPP_MACRO_DB_CLIENT_PARAM_NAME(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG PARAM_LIST
#define OATPP_MACRO_DB_CLIENT_PARAM_TYPE_STR(MACRO, TYPE, PARAM_LIST) #TYPE
#define OATPP_MACRO_DB_CLIENT_PARAM_NAME_STR(MACRO, TYPE, PARAM_LIST) OATPP_MACRO_FIRSTARG_STR PARAM_LIST
#define OATPP_MACRO_DB_CLIENT_PARAM(MACRO, TYPE, PARAM_LIST) (MACRO, TYPE, PARAM_LIST)

#define PARAM(TYPE, ...) OATPP_MACRO_DB_CLIENT_PARAM(OATPP_MACRO_DB_CLIENT_PARAM_PARAM, TYPE, (__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(MACRO, TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (TYPE, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

// PARAM MACRO

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_1(TYPE, NAME) \
__params[#NAME] = NAME;

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM_2(TYPE, NAME, QUALIFIER) \
__params[QUALIFIER] = NAME;

#define OATPP_MACRO_DB_CLIENT_PARAM_PARAM(TYPE, PARAM_LIST) \
OATPP_MACRO_DB_CLIENT_MACRO_SELECTOR(OATPP_MACRO_DB_CLIENT_PARAM_PARAM_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// FOR EACH

#define OATPP_MACRO_DB_CLIENT_PARAM_DECL_FIRST(INDEX, COUNT, X) \
OATPP_MACRO_DB_CLIENT_PARAM_TYPE X OATPP_MACRO_DB_CLIENT_PARAM_NAME X

#define OATPP_MACRO_DB_CLIENT_PARAM_DECL_REST(INDEX, COUNT, X) \
, OATPP_MACRO_DB_CLIENT_PARAM_TYPE X OATPP_MACRO_DB_CLIENT_PARAM_NAME X

#define OATPP_MACRO_DB_CLIENT_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_DB_CLIENT_PARAM_MACRO X

// QUERY MACRO

#define OATPP_QUERY_0(NAME, QUERY_TEXT) \
oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_##NAME = m_executor->parseQueryTemplate(QUERY_TEXT); \
\
oatpp::db::QueryResult NAME() { \
  std::unordered_map<oatpp::String, oatpp::Any> __params; \
  return m_executor->execute(Z_QUERY_TEMPLATE_##NAME, __params); \
}



#define OATPP_QUERY_1(NAME, QUERY_TEXT, ...) \
oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_##NAME = m_executor->parseQueryTemplate(QUERY_TEXT); \
\
oatpp::db::QueryResult NAME( \
  OATPP_MACRO_FOREACH_FIRST_AND_REST( \
    OATPP_MACRO_DB_CLIENT_PARAM_DECL_FIRST, \
    OATPP_MACRO_DB_CLIENT_PARAM_DECL_REST, \
    __VA_ARGS__ \
  ) \
) { \
  std::unordered_map<oatpp::String, oatpp::Any> __params; \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DB_CLIENT_PARAM_PUT, __VA_ARGS__) \
  return m_executor->execute(Z_QUERY_TEMPLATE_##NAME, __params); \
}

// Chooser

#define OATPP_QUERY_MACRO_0(NAME, QUERY_TEXT) \
OATPP_QUERY_0(NAME, QUERY_TEXT)

#define OATPP_QUERY_MACRO_1(NAME, QUERY_TEXT, ...) \
OATPP_QUERY_1(NAME, QUERY_TEXT, __VA_ARGS__)


#define QUERY(NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_QUERY_MACRO_, (__VA_ARGS__)) (NAME, __VA_ARGS__))
