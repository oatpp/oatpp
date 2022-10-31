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

#include "FloatTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)


class DTO_32_0 : public oatpp::DTO {

  DTO_INIT(DTO_32_0, DTO)

  DTO_FIELD(Float32, f32);
};

class DTO_32_1 : public oatpp::DTO {

  DTO_INIT(DTO_32_1, DTO)

  DTO_FIELD_INFO(f32) {
    info->format = "%.2f";
  }
  DTO_FIELD(Float32, f32);
};

class DTO_64_0 : public oatpp::DTO {

  DTO_INIT(DTO_64_0, DTO)

  DTO_FIELD(Float64, f64);
};

class DTO_64_1 : public oatpp::DTO {

  DTO_INIT(DTO_64_1, DTO)

  DTO_FIELD_INFO(f64) {
    info->format = "%.2f";
  }
  DTO_FIELD(Float64, f64);
};

class DTO_64_2 : public oatpp::DTO {

  DTO_INIT(DTO_64_2, DTO)

  DTO_FIELD_INFO(f64_1) {
    info->format = "%.1f";
  }
  DTO_FIELD(Float64, f64_1);

  DTO_FIELD_INFO(f64_2) {
    info->format = "%.2f";
  }
  DTO_FIELD(Float64, f64_2);

  DTO_FIELD(Int32, i);
};

#include OATPP_CODEGEN_END(DTO)

}

void FloatTest::onRun() {

  oatpp::parser::json::mapping::ObjectMapper mapper;
  oatpp::parser::json::mapping::ObjectMapper mapperFmt;
  mapperFmt.getSerializer()->getConfig()->floatStringFormat = "%.1f";

  OATPP_LOGI(TAG, "Serialize float: 123456.123456");
  {
    auto test = DTO_32_0::createShared();
    test->f32 = 123456.123456;
    OATPP_LOGI(TAG, "using default format...");
    auto json = mapper.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json != "{\"f32\":123456.123456}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_32_1::createShared();
    test->f32 = 123456.123456;
    OATPP_LOGI(TAG, "using \"%%.2f\" format...");
    auto json = mapper.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f32\":123456.12}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_64_0::createShared();
    test->f64 = 123456.123456;// +0.2;
    OATPP_LOGI(TAG, "using default format...");
    auto json = mapper.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f64\":123456.123456}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_64_1::createShared();
    test->f64 = 123456.123456;
    OATPP_LOGI(TAG, "using \"%%.2f\" format...");
    auto json = mapper.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f64\":123456.12}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_64_2::createShared();
    test->f64_1 = 123456.123456;
    test->f64_2 = 123456.123456;
    test->i = 10;
    OATPP_LOGI(TAG, "using 2 formats...");
    auto json = mapper.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f64_1\":123456.1,\"f64_2\":123456.12,\"i\":10}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_32_0::createShared();
    test->f32 = 123456.123456;
    OATPP_LOGI(TAG, "using config's \"%%.1f\" format...");
    auto json = mapperFmt.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f32\":123456.1}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_64_0::createShared();
    test->f64 = 123456.123456;
    OATPP_LOGI(TAG, "using config's \"%%.1f\" format...");
    auto json = mapperFmt.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f64\":123456.1}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    auto test = DTO_64_1::createShared();
    test->f64 = 123456.123456;
    OATPP_LOGI(TAG, "using field's \"%%.2f\" to override config's \"%%.1f\" format...");
    auto json = mapperFmt.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\"f64\":123456.12}");
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}