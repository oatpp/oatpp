/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
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


#define OATPP_MACRO_DTO_ENUM_PARAM_MACRO(MACRO, NAME, PARAM_LIST) MACRO(NAME, PARAM_LIST)
#define OATPP_MACRO_DTO_ENUM_PARAM_NAME(MACRO, NAME, PARAM_LIST) NAME
#define OATPP_MACRO_DTO_ENUM_PARAM_NAME_STR(MACRO, NAME, PARAM_LIST) #NAME
#define OATPP_MACRO_DTO_ENUM_PARAM_VALUE(MACRO, NAME, PARAM_LIST) OATPP_MACRO_FIRSTARG PARAM_LIST
#define OATPP_MACRO_DTO_ENUM_PARAM_VALUE_STR(MACRO, NAME, PARAM_LIST) OATPP_MACRO_FIRSTARG_STR PARAM_LIST
#define OATPP_MACRO_DTO_ENUM_PARAM(MACRO, NAME, PARAM_LIST) (MACRO, NAME, PARAM_LIST)

#define VALUE(NAME, ...) OATPP_MACRO_DTO_ENUM_PARAM(OATPP_MACRO_DTO_ENUM_VALUE, NAME, (__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_DTO_ENUM_MACRO_SELECTOR(MACRO, NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (NAME, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

// VALUE MACRO

#define OATPP_MACRO_DTO_ENUM_VALUE_1(NAME, VAL) \
{ \
  oatpp::data::mapping::type::EnumValueInfo<EnumType> entry = {EnumType::NAME, index ++, #NAME}; \
  info.byName.insert({#NAME, entry}); \
  info.byValue.insert({static_cast<v_uint64>(EnumType::NAME), entry}); \
  info.byIndex.push_back(entry); \
}

#define OATPP_MACRO_DTO_ENUM_VALUE_2(NAME, VAL, QUALIFIER) \
{ \
  oatpp::data::mapping::type::EnumValueInfo<EnumType> entry = {EnumType::NAME, index ++, QUALIFIER}; \
  info.byName.insert({QUALIFIER, entry}); \
  info.byValue.insert({static_cast<v_uint64>(EnumType::NAME), entry}); \
  info.byIndex.push_back(entry); \
}

#define OATPP_MACRO_DTO_ENUM_VALUE(NAME, PARAM_LIST) \
OATPP_MACRO_DTO_ENUM_MACRO_SELECTOR(OATPP_MACRO_DTO_ENUM_VALUE_, NAME, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// FOR EACH

#define OATPP_MACRO_DTO_ENUM_PARAM_DECL_FIRST(INDEX, COUNT, X) \
OATPP_MACRO_DTO_ENUM_PARAM_NAME X = OATPP_MACRO_DTO_ENUM_PARAM_VALUE X

#define OATPP_MACRO_DTO_ENUM_PARAM_DECL_REST(INDEX, COUNT, X) \
, OATPP_MACRO_DTO_ENUM_PARAM_NAME X = OATPP_MACRO_DTO_ENUM_PARAM_VALUE X

#define OATPP_MACRO_DTO_ENUM_PARAM_PUT(INDEX, COUNT, X) \
OATPP_MACRO_DTO_ENUM_PARAM_MACRO X

// ENUM MACRO

#define OATPP_ENUM_0(NAME, ORDINAL_TYPE) \
enum class NAME : ORDINAL_TYPE {}; \
\
class Z__OATPP_ENUM_META_##NAME : public oatpp::data::mapping::type::EnumMeta<NAME> { \
private: \
\
  static bool init() { \
    auto& info = EnumMeta<NAME>::__info; \
    info.nameQualifier = #NAME; \
    return true; \
  } \
\
private: \
  static bool initialized; \
}; \
\
bool Z__OATPP_ENUM_META_##NAME::initialized = Z__OATPP_ENUM_META_##NAME::init();

#define OATPP_ENUM_1(NAME, ORDINAL_TYPE, ...) \
enum class NAME : ORDINAL_TYPE { \
  OATPP_MACRO_FOREACH_FIRST_AND_REST( \
    OATPP_MACRO_DTO_ENUM_PARAM_DECL_FIRST, \
    OATPP_MACRO_DTO_ENUM_PARAM_DECL_REST, \
    __VA_ARGS__ \
  ) \
}; \
\
class Z__OATPP_ENUM_META_##NAME : public oatpp::data::mapping::type::EnumMeta<NAME> { \
private: \
\
  static bool init() { \
    auto& info = EnumMeta<NAME>::__info; \
    v_int32 index = 0; \
    info.nameQualifier = #NAME; \
    OATPP_MACRO_FOREACH(OATPP_MACRO_DTO_ENUM_PARAM_PUT, __VA_ARGS__) \
    return true; \
  } \
\
private: \
  static bool initialized; \
}; \
\
bool Z__OATPP_ENUM_META_##NAME::initialized = Z__OATPP_ENUM_META_##NAME::init();

// Chooser

#define OATPP_ENUM_MACRO_0(NAME, ORDINAL_TYPE) \
OATPP_ENUM_0(NAME, ORDINAL_TYPE)

#define OATPP_ENUM_MACRO_1(NAME, ORDINAL_TYPE, ...) \
OATPP_ENUM_1(NAME, ORDINAL_TYPE, __VA_ARGS__)

/**
 * Codegen macoro to be used in `oatpp::web::client::ApiClient` to generate REST API-Calls.
 * @param METHOD - Http method ("GET", "POST", "PUT", etc.)
 * @param PATH - Path to endpoint (without host)
 * @param NAME - Name of the generated method
 * @return - std::shared_ptr to &id:oatpp::web::protocol::http::incoming::Response;
 */
#define ENUM(NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_ENUM_MACRO_, (__VA_ARGS__)) (NAME, __VA_ARGS__))
