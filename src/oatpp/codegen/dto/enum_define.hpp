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

/**
 * Enum entry value.
 * @param NAME - name of the enum. **required**.
 * @param ORDINAL_VALUE - corresponding ordinal value. **required**.
 * @param QUALIFIER - name qualifier to be used instead of the `NAME`. **optional**.
 * @param DESCRIPTION - description of the enum value. **optional**.
 */
#define VALUE(NAME, ...) \
OATPP_MACRO_DTO_ENUM_PARAM(OATPP_MACRO_DTO_ENUM_VALUE, NAME, (__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

#define OATPP_MACRO_DTO_ENUM_MACRO_SELECTOR(MACRO, NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(MACRO, (__VA_ARGS__)) (NAME, __VA_ARGS__))

//////////////////////////////////////////////////////////////////////////

// VALUE MACRO

#define OATPP_MACRO_DTO_ENUM_VALUE_1(NAME, VAL) \
{ \
  oatpp::data::mapping::type::EnumValueInfo<EnumType> entry = {EnumType::NAME, index ++, #NAME, nullptr}; \
  info.byName.insert({#NAME, entry}); \
  info.byValue.insert({static_cast<v_uint64>(EnumType::NAME), entry}); \
  info.byIndex.push_back(entry); \
}

#define OATPP_MACRO_DTO_ENUM_VALUE_2(NAME, VAL, QUALIFIER) \
{ \
  oatpp::data::mapping::type::EnumValueInfo<EnumType> entry = {EnumType::NAME, index ++, QUALIFIER, nullptr}; \
  info.byName.insert({QUALIFIER, entry}); \
  info.byValue.insert({static_cast<v_uint64>(EnumType::NAME), entry}); \
  info.byIndex.push_back(entry); \
}

#define OATPP_MACRO_DTO_ENUM_VALUE_3(NAME, VAL, QUALIFIER, DESCRIPTION) \
{ \
  oatpp::data::mapping::type::EnumValueInfo<EnumType> entry = {EnumType::NAME, index ++, QUALIFIER, DESCRIPTION}; \
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
namespace { \
\
class Z__OATPP_ENUM_META_##NAME : public oatpp::data::mapping::type::EnumMeta<NAME> { \
private: \
\
  static bool init() { \
    auto& info = *EnumMeta<NAME>::getInfo(); \
    v_int32 index = 0; \
    (void)index; \
    info.nameQualifier = #NAME; \
    return true; \
  } \
\
public: \
\
  static bool initializer() { \
    static bool initialized = init(); \
    return initialized; \
  } \
\
}; \
\
bool Z__OATPP_ENUM_META_INITIALIZER_##NAME = Z__OATPP_ENUM_META_##NAME::initializer(); \
\
}

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
    auto& info = *EnumMeta<NAME>::getInfo(); \
    v_int32 index = 0; \
    info.nameQualifier = #NAME; \
    OATPP_MACRO_FOREACH(OATPP_MACRO_DTO_ENUM_PARAM_PUT, __VA_ARGS__) \
    return true; \
  } \
\
public: \
\
  static bool initializer() { \
    static bool initialized = init(); \
    return initialized; \
  } \
\
}; \
\
static bool Z__OATPP_ENUM_META_INITIALIZER_##NAME = Z__OATPP_ENUM_META_##NAME::initializer();

// Chooser

#define OATPP_ENUM_MACRO_0(NAME, ORDINAL_TYPE) \
OATPP_ENUM_0(NAME, ORDINAL_TYPE)

#define OATPP_ENUM_MACRO_1(NAME, ORDINAL_TYPE, ...) \
OATPP_ENUM_1(NAME, ORDINAL_TYPE, __VA_ARGS__)

/**
 * Codegen macoro to generate oatpp mapping-enabled enum.
 * @param NAME - name of the enum. **required**.
 * @param UNDERLYING_TYPE - underlying ordinal type. **required**.
 * @param ... - enum values defined with &l:VALUE (...);. macro.
 */
#define ENUM(NAME, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_ENUM_MACRO_, (__VA_ARGS__)) (NAME, __VA_ARGS__))
