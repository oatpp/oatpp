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

#include <thread>

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

  {
    mapperFmt.getSerializer()->getConfig()->useBeautifier = true;
    auto test = DTO_64_0::createShared();
    test->f64 = 123456.123456;
    OATPP_LOGI(TAG, "using config's \"%%.1f\" format(useBeautifier) ...");
    auto json = mapperFmt.writeToString(test);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == "{\n  \"f64\": 123456.1\n}");
    OATPP_LOGI(TAG, "OK");
  }

  {
    int count = 10000;
    bool parallel_result = true;
    auto test1 = DTO_64_0::createShared();
    test1->f64 = 123456.123456;
    OATPP_LOGI(TAG, "parallel 1: using default format...");
    OATPP_LOGI(TAG, "expect: json1='%s'", "{\"f64\":123456.123456}");
    std::thread proc1([&]() {
      for (int i = 0; i < count; ++i) {
        auto json = mapper.writeToString(test1);
        if (json != "{\"f64\":123456.123456}") {
          OATPP_LOGE(TAG, "json1='%s'", json->c_str());
          parallel_result = false;
          break;
        }
      }
    });

    auto test2 = DTO_64_1::createShared();
    test2->f64 = 123456.123456;
    OATPP_LOGI(TAG, "parallel 2: using \"%%.2f\" format...");
    OATPP_LOGI(TAG, "expect:  json2='%s'", "{\"f64\":123456.12}");
    std::thread proc2([&]() {
      for (int i = 0; i < count; ++i) {
        auto json = mapper.writeToString(test2);
        if (json != "{\"f64\":123456.12}") {
          OATPP_LOGE(TAG, "json2='%s'", json->c_str());
          parallel_result = false;
          break;
        }
      }
    });
    proc1.join();
    proc2.join();

    OATPP_ASSERT(parallel_result == true);
    OATPP_LOGI(TAG, "parallel OK");
  }

}

}}}}}
