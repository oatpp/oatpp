/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__/__)(__)  |_|    |_|
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

#include "NullableFieldTest.hpp"

#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

namespace oatpp { namespace test { namespace data { namespace mapping {

namespace {

  #include OATPP_CODEGEN_BEGIN(DTO)

  class TestDto : public oatpp::DTO {

    DTO_INIT(TestDto, DTO)

    DTO_FIELD(String, name);
    DTO_FIELD(Int32, age);

    DTO_FIELD_INFO(nullable_field) {
      info->required = false;
      info->nullable = true;
      info->description = "A nullable field that can be omitted or null";
    }
    DTO_FIELD(Float32, nullable_field);

    DTO_FIELD_INFO(required_field) {
      info->required = true;
      info->nullable = false;
    }
    DTO_FIELD(Float32, required_field);

    DTO_FIELD_INFO(optional_field) {
      info->required = false;
      info->nullable = false;
    }
    DTO_FIELD(Float32, optional_field);

  };

  #include OATPP_CODEGEN_END(DTO)

}

void NullableFieldTest::onRun() {

  OATPP_LOGD(TAG, "Test 1: DTO with nullable=null field")
  {
    auto dto = TestDto::createShared();
    dto->nullable_field = nullptr;  // Explicitly set to null

    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

    // Serialize to JSON
    oatpp::String json = objectMapper->writeToString(dto);
    OATPP_LOGD(TAG, "JSON with nullable=null: {}", json)

    // Deserialize back
    auto parsedDto = objectMapper->readFromString<oatpp::Object<TestDto>>(json);

    OATPP_CHECK(parsedDto)
    OATPP_CHECK(parsedDto->nullable_field == nullptr)
    OATPP_LOGD(TAG, "✓ nullable field successfully serialized/deserialized as null")
  }

  OATPP_LOGD(TAG, "Test 2: DTO with nullable=value")
  {
    auto dto = TestDto::createShared();
    dto->nullable_field = 3.14f;

    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

    oatpp::String json = objectMapper->writeToString(dto);
    OATPP_LOGD(TAG, "JSON with nullable=value: {}", json)

    auto parsedDto = objectMapper->readFromString<oatpp::Object<TestDto>>(json);

    OATPP_CHECK(parsedDto)
    OATPP_CHECK(parsedDto->nullable_field)
    OATPP_CHECK(*parsedDto->nullable_field == 3.14f)
    OATPP_LOGD(TAG, "✓ nullable field successfully serialized/deserialized with value")
  }

  OATPP_LOGD(TAG, "Test 3: DTO with nullable field omitted (JSON without the field)")
  {
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

    // JSON without nullable_field
    oatpp::String json = "{\"name\":\"John\",\"age\":30,\"required_field\":1.0}";

    auto parsedDto = objectMapper->readFromString<oatpp::Object<TestDto>>(json);

    OATPP_CHECK(parsedDto)
    OATPP_CHECK(parsedDto->nullable_field == nullptr)  // Should be null when omitted
    OATPP_LOGD(TAG, "✓ nullable field defaults to null when omitted in JSON")
  }

  OATPP_LOGD(TAG, "Test 4: DTO with required field validation")
  {
    auto dto = TestDto::createShared();
    dto->name = "Test";
    dto->age = 25;
    dto->nullable_field = nullptr;
    dto->optional_field = 2.5f;
    // required_field is NOT set - should fail validation

    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

    try {
      oatpp::String json = objectMapper->writeToString(dto);
      OATPP_LOGD(TAG, "ERROR: Should have failed validation for missing required field!")
      OATPP_CHECK(false)
    } catch (const std::exception& e) {
      OATPP_LOGD(TAG, "✓ Correctly failed validation for missing required field: {}", e.what())
    }
  }

  OATPP_LOGD(TAG, "Test 5: Check field info properties")
  {
    const auto& fields = TestDto::getFields();
    bool foundNullable = false;

    for (const auto& field : fields) {
      if (field->name == "nullable_field") {
        OATPP_CHECK(field->info.required == false)
        OATPP_CHECK(field->info.nullable == true)
        OATPP_CHECK(field->info.description == "A nullable field that can be omitted or null")
        foundNullable = true;
        OATPP_LOGD(TAG, "✓ nullable_field info: required={}, nullable={}, description={{}{}{}}",
                   field->info.required, field->info.nullable, field->info.description)
        break;
      }
    }

    OATPP_CHECK(foundNullable)
    OATPP_LOGD(TAG, "✓ Field info correctly stores nullable property")
  }

  OATPP_LOGD(TAG, "Test 6: Serialization config - includeNullFields=false")
  {
    auto config = oatpp::parser::json::mapping::ObjectMapper::createShared();
    config->config->includeNullFields = false;

    auto dto = TestDto::createShared();
    dto->name = "Test";
    dto->age = 25;
    dto->nullable_field = nullptr;  // null value
    dto->required_field = 1.0f;

    oatpp::String json = config->writeToString(dto);
    OATPP_LOGD(TAG, "JSON with includeNullFields=false: {}", json)

    // When includeNullFields=false, nullable=null should not appear in JSON
    // But nullable field should still be allowed to be null
    OATPP_CHECK(json->find("nullable_field") == oatpp::String::npos)
    OATPP_LOGD(TAG, "✓ null nullable field not included in JSON when includeNullFields=false")
  }

  OATPP_LOGD(TAG, "Test 7: Serialization config - includeNullFields=true")
  {
    auto config = oatpp::parser::json::mapping::ObjectMapper::createShared();
    config->config->includeNullFields = true;

    auto dto = TestDto::createShared();
    dto->name = "Test";
    dto->age = 25;
    dto->nullable_field = nullptr;  // null value
    dto->required_field = 1.0f;

    oatpp::String json = config->writeToString(dto);
    OATPP_LOGD(TAG, "JSON with includeNullFields=true: {}", json)

    // When includeNullFields=true, nullable=null SHOULD appear in JSON
    OATPP_CHECK(json->find("nullable_field") != oatpp::String::npos)
    OATPP_CHECK(json->find("null") != oatpp::String::npos)
    OATPP_LOGD(TAG, "✓ null nullable field included in JSON when includeNullFields=true")
  }

  OATPP_LOGD(TAG, "All nullable field tests passed!")

}

}}}}
