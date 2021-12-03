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

#include "oatpp/parser/json/LocaleDecimalSeparatorTest.hpp"

#include "oatpp/parser/json/Utils.hpp"

namespace oatpp { namespace test { namespace parser { namespace json {

void LocaleDecimalSeparatorTest::localeOnRun() {

  using oatpp::parser::json::Utils;

  OATPP_LOGD(TAG, "GivenUsLocale_WhenConversionFromJsonToLocale_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    strcpy(json_buffer, NUMBER_WITH_POINT_AND_OTHERS);
    Utils::convertFirstDecimalSeparatorFromJsonToLocale(json_string, json_end);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", json_buffer, NUMBER_WITH_POINT_AND_OTHERS);
    OATPP_ASSERT(strcmp(json_buffer, NUMBER_WITH_POINT_AND_OTHERS) == 0);
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenConversionFromJsonToLocale_ThenNumberUsesComma");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    strcpy(json_buffer, NUMBER_WITH_POINT_AND_OTHERS);
    Utils::convertFirstDecimalSeparatorFromJsonToLocale(json_string, json_end);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", json_buffer, NUMBER_WITH_COMMA_AND_OTHERS);
    OATPP_ASSERT(strcmp(json_buffer, NUMBER_WITH_COMMA_AND_OTHERS) == 0);
  }

  OATPP_LOGD(TAG, "GivenUsLocale_WhenConversionFromLocaleToJson_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    strcpy(locale_buffer, NUMBER_WITH_POINT_AND_OTHERS);
    Utils::convertFirstDecimalSeparatorFromLocaleToJson(locale_string, locale_end);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", locale_buffer, NUMBER_WITH_POINT_AND_OTHERS);
    OATPP_ASSERT(strcmp(locale_buffer, NUMBER_WITH_POINT_AND_OTHERS) == 0);
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenConversionFromLocaleToJson_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    strcpy(locale_buffer, NUMBER_WITH_COMMA_AND_OTHERS);
    Utils::convertFirstDecimalSeparatorFromLocaleToJson(locale_string, locale_end);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", locale_buffer, NUMBER_WITH_POINT_AND_OTHERS);
    OATPP_ASSERT(strcmp(locale_buffer, NUMBER_WITH_POINT_AND_OTHERS) == 0);
  }

  OATPP_LOGD(TAG, "GivenUsLocale_WhenConversionFromFloat32ToJson_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    Utils::float32ToJson(NUMBER_FLOAT32, json_string, BUFFER_SIZE);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", json_buffer, NUMBER_WITH_POINT);
    OATPP_ASSERT(strcmp(json_buffer, NUMBER_WITH_POINT) == 0);
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenConversionFromFloat32ToJson_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    Utils::float32ToJson(NUMBER_FLOAT32, json_string, BUFFER_SIZE);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", json_buffer, NUMBER_WITH_POINT);
    OATPP_ASSERT(strcmp(json_buffer, NUMBER_WITH_POINT) == 0);
  }

  OATPP_LOGD(TAG, "GivenUsLocale_WhenConversionFromFloat64ToJson_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, US_LOCALE));
    Utils::float64ToJson(NUMBER_FLOAT64, json_string, BUFFER_SIZE);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", json_buffer, NUMBER_WITH_POINT);
    OATPP_ASSERT(strcmp(json_buffer, NUMBER_WITH_POINT) == 0);
  }

  OATPP_LOGD(TAG, "GivenGermanLocale_WhenConversionFromFloat64ToJson_ThenNumberUsesPoint");
  {
    OATPP_ASSERT(setlocale(LC_NUMERIC, GERMAN_LOCALE));
    Utils::float64ToJson(NUMBER_FLOAT64, json_string, BUFFER_SIZE);
    OATPP_LOGD(TAG, "actual='%s' expected='%s'", json_buffer, NUMBER_WITH_POINT);
    OATPP_ASSERT(strcmp(json_buffer, NUMBER_WITH_POINT) == 0);
  }

}

}}}}
