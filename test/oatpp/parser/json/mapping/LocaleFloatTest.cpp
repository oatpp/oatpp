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

#include "LocaleFloatTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {

namespace {
#include OATPP_CODEGEN_BEGIN(DTO)

class FloatDto : public oatpp::DTO {
  DTO_INIT(FloatDto, DTO)
  DTO_FIELD(Float32, float32);
  DTO_FIELD(Float64, float64);
};

#include OATPP_CODEGEN_END(DTO)
}
  
void LocaleFloatTest::localeOnRun(){
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

  oatpp::Object<FloatDto> obj;

  OATPP_LOGD(TAG, "GivenUsLocale_WhenParsingFloat32_ThenWorks");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    obj = mapper->readFromString<oatpp::Object<FloatDto>>(R"({"float32":-1.110999941825867})");
    OATPP_ASSERT(obj);
    OATPP_ASSERT(obj->float32 == -1.110999941825867f);
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenParsingFloat32_ThenWorks");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    obj = mapper->readFromString<oatpp::Object<FloatDto>>(R"({"float32":-1.110999941825867})");
    OATPP_ASSERT(obj);
    OATPP_ASSERT(obj->float32 == -1.110999941825867f);
  }

  OATPP_LOGD(TAG, "GivenUsLocale_WhenParsingFloat64_ThenWorks");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    obj = mapper->readFromString<oatpp::Object<FloatDto>>(R"({"float64":-1.234567891234568})");
    OATPP_ASSERT(obj);
    OATPP_ASSERT(obj->float64 == -1.234567891234568);
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenParsingFloat64_ThenWorks");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    obj = mapper->readFromString<oatpp::Object<FloatDto>>(R"({"float64":-1.234567891234568})");
    OATPP_ASSERT(obj);
    OATPP_ASSERT(obj->float64 == -1.234567891234568);
  }

  OATPP_LOGD(TAG, "GivenUsLocale_WhenSerialisingFloat32And64_ThenWorks");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    obj->float32 = -1.110999941825867f;
    obj->float64 = -1.234567891234568;
    auto json = mapper->writeToString(obj);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == R"({"float32":-1.110999941825867,"float64":-1.234567891234568})");
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenSerialisingFloat32And64_ThenWorks");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    obj->float32 = -1.110999941825867f;
    obj->float64 = -1.234567891234568;
    auto json = mapper->writeToString(obj);
    OATPP_LOGD(TAG, "json='%s'", json->c_str());
    OATPP_ASSERT(json == R"({"float32":-1.110999941825867,"float64":-1.234567891234568})");
  }

}
  
}}}}}
