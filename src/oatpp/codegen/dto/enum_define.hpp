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
IMPL{ NAME = VAL }

#define OATPP_MACRO_DTO_ENUM_VALUE_2(NAME, VAL, QUALIFIER) \
IMPL{ NAME (QUALIFIER) = VAL }

#define OATPP_MACRO_DTO_ENUM_VALUE(NAME, PARAM_LIST) \
OATPP_MACRO_DTO_ENUM_MACRO_SELECTOR(OATPP_MACRO_DTO_ENUM_VALUE_, NAME, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

// FOR EACH

#define OATPP_MACRO_DTO_ENUM_PARAM_DECL_FIRST(INDEX, COUNT, X) \
OATPP_MACRO_DTO_ENUM_PARAM_NAME X = OATPP_MACRO_DTO_ENUM_PARAM_VALUE X

#define OATPP_MACRO_DTO_ENUM_PARAM_DECL_REST(INDEX, COUNT, X) \
, OATPP_MACRO_DTO_ENUM_PARAM_NAME X = OATPP_MACRO_DTO_ENUM_PARAM_VALUE X

#define OATPP_MACRO_DTO_ENUM_PARAM_PUT(INDEX, COUNT, X) \
{ \
  "name": OATPP_MACRO_DTO_ENUM_PARAM_NAME X; \
  "value": OATPP_MACRO_DTO_ENUM_PARAM_VALUE X; \
  "name-str": OATPP_MACRO_DTO_ENUM_PARAM_NAME_STR X; \
  "value-str": OATPP_MACRO_DTO_ENUM_PARAM_VALUE_STR X; \
  "body": { \
    OATPP_MACRO_DTO_ENUM_PARAM_MACRO X \
  } \
}

// ENUM MACRO

#define OATPP_ENUM_0(NAME, ORDINAL_TYPE) \
enum NAME : ORDINAL_TYPE {}; \
\
template<> \
struct EnumInfo <NAME> { \
public: \
  typedef ORDINAL_TYPE ValueType; \
public: \
  static constexpr const char* name = #NAME; \
};

#define OATPP_ENUM_1(NAME, ORDINAL_TYPE, ...) \
enum NAME : ORDINAL_TYPE { \
  OATPP_MACRO_FOREACH_FIRST_AND_REST( \
    OATPP_MACRO_DTO_ENUM_PARAM_DECL_FIRST, \
    OATPP_MACRO_DTO_ENUM_PARAM_DECL_REST, \
    __VA_ARGS__ \
  ) \
}; \
\
template<> \
struct EnumInfo <NAME> { \
public: \
  typedef ORDINAL_TYPE ValueType; \
public: \
  static constexpr const char* name = #NAME; \
  \
  OATPP_MACRO_FOREACH(OATPP_MACRO_DTO_ENUM_PARAM_PUT, __VA_ARGS__) \
  \
};

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
