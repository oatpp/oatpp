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

#include "EnumTest.hpp"

#include "oatpp/json/ObjectMapper.hpp"

#include "oatpp/macro/codegen.hpp"

namespace oatpp { namespace json {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(Enum0, v_int32);

ENUM(Enum1, v_int32,
  VALUE(V1, 10, "enum1-v1"),
  VALUE(V2, 20, "enum1-v2"),
  VALUE(V3, 30, "enum1-v3")
);

class DTO1 : public oatpp::DTO {

  DTO_INIT(DTO1, DTO)

  DTO_FIELD(Enum<Enum1>::AsString, enum1);

};

#include OATPP_CODEGEN_END(DTO)

}

void EnumTest::onRun() {

  oatpp::json::ObjectMapper mapper;

  {
    OATPP_LOGi(TAG, "Serializer as string...")
    oatpp::Fields<oatpp::Enum<Enum1>::AsString> map = {{"enum", Enum1::V1}};
    auto json = mapper.writeToString(map);
    OATPP_LOGd(TAG, "json='{}'", json->c_str())
    OATPP_ASSERT(json == "{\"enum\":\"enum1-v1\"}")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Serializer as string null...")
    oatpp::Fields<oatpp::Enum<Enum1>::AsString> map = {{"enum", nullptr}};
    auto json = mapper.writeToString(map);
    OATPP_LOGd(TAG, "json='{}'", json->c_str())
    OATPP_ASSERT(json == "{\"enum\":null}")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Serializer as string error on null...")
    bool error = false;
    oatpp::Fields<oatpp::Enum<Enum1>::AsString::NotNull> map = {{"enum", nullptr}};
    try {
      auto json = mapper.writeToString(map);
    } catch (const data::mapping::MappingError& e) {
      OATPP_LOGd(TAG, "error - '{}'\ntrace:\n{}", e.what(), e.errorStack().stacktrace()->c_str())
      error = true;
    }
    OATPP_ASSERT(error == true)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Serializer as int...")
    oatpp::Fields<oatpp::Enum<Enum1>::AsNumber> map = {{"enum", Enum1::V1}};
    auto json = mapper.writeToString(map);
    OATPP_LOGd(TAG, "json='{}'", json)
    OATPP_ASSERT(json == "{\"enum\":10}")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Serializer as int null...")
    oatpp::Fields<oatpp::Enum<Enum1>::AsNumber> map = {{"enum", nullptr}};
    auto json = mapper.writeToString(map);
    OATPP_LOGd(TAG, "json='{}'", json)
    OATPP_ASSERT(json == "{\"enum\":null}")
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Serializer as int error on null...")
    bool error = false;
    oatpp::Fields<oatpp::Enum<Enum1>::AsNumber::NotNull> map = {{"enum", nullptr}};
    try {
      auto json = mapper.writeToString(map);
    } catch (const data::mapping::MappingError& e) {
      OATPP_LOGd(TAG, "error - '{}'\ntrace:\n{}", e.what(), e.errorStack().stacktrace())
      error = true;
    }
    OATPP_ASSERT(error == true)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Deserializer as string...")
    oatpp::String json = "{\"enum\":\"enum1-v2\"}";
    auto map = mapper.readFromString<oatpp::Fields<oatpp::Enum<Enum1>::AsString>>(json);
    OATPP_ASSERT(map["enum"] == Enum1::V2)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Deserializer as string null...")
    oatpp::String json = "{\"enum\":null}";
    auto map = mapper.readFromString<oatpp::Fields<oatpp::Enum<Enum1>::AsString>>(json);
    OATPP_ASSERT(map["enum"] == nullptr)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Deserializer as string error on null...")
    bool error = false;
    oatpp::String json = "{\"enum\":null}";
    try {
      auto map = mapper.readFromString<oatpp::Fields<oatpp::Enum<Enum1>::AsString::NotNull>>(json);
    } catch (const data::mapping::MappingError& e) {
      OATPP_LOGd(TAG, "error - '{}'\ntrace:\n{}", e.what(), e.errorStack().stacktrace()->c_str())
      error = true;
    }
    OATPP_ASSERT(error == true)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Deserializer as int...")
    oatpp::String json = "{\"enum\":20}";
    auto map = mapper.readFromString<oatpp::Fields<oatpp::Enum<Enum1>::AsNumber>>(json);
    OATPP_ASSERT(map["enum"] == Enum1::V2)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Deserializer as int null...")
    oatpp::String json = "{\"enum\":null}";
    auto map = mapper.readFromString<oatpp::Fields<oatpp::Enum<Enum1>::AsNumber>>(json);
    OATPP_ASSERT(map["enum"] == nullptr)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Deserializer as int error on null...")
    bool error = false;
    oatpp::String json = "{\"enum\":null}";
    try {
      auto map = mapper.readFromString<oatpp::Fields<oatpp::Enum<Enum1>::AsNumber::NotNull>>(json);
    } catch (const data::mapping::MappingError& e) {
      OATPP_LOGd(TAG, "error - '{}'\ntrace:\n{}", e.what(), e.errorStack().stacktrace()->c_str())
      error = true;
    }
    OATPP_ASSERT(error == true)
    OATPP_LOGi(TAG, "OK")
  }

}

}}
