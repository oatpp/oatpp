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

#include "oatpp/parser/json/LocaleTestBase.hpp"

namespace oatpp { namespace test { namespace parser { namespace json {

constexpr size_t LocaleTestBase::BUFFER_SIZE;
constexpr char LocaleTestBase::US_LOCALE[];
constexpr char LocaleTestBase::GERMAN_LOCALE[];
constexpr char LocaleTestBase::NUMBER_WITH_POINT[];
constexpr char LocaleTestBase::NUMBER_WITH_COMMA[];
constexpr char LocaleTestBase::NUMBER_WITH_POINT_AND_OTHERS[];
constexpr char LocaleTestBase::NUMBER_WITH_COMMA_AND_OTHERS[];
constexpr v_float32 LocaleTestBase::NUMBER_FLOAT32;
constexpr v_float64 LocaleTestBase::NUMBER_FLOAT64;

LocaleTestBase::Guard::Guard() {
  strcpy(original_locale, setlocale(LC_NUMERIC, nullptr));
}
LocaleTestBase::Guard::~Guard() {
  setlocale(LC_NUMERIC, original_locale);
}

bool LocaleTestBase::ShouldWeSkip() const {
  if (setlocale(LC_NUMERIC, US_LOCALE) == nullptr) {
    OATPP_LOGD(TAG, "Skip tests because of missing %s", US_LOCALE);
    return true;
  }
  if (setlocale(LC_NUMERIC, GERMAN_LOCALE) == nullptr) {
    OATPP_LOGD(TAG, "Skip tests because of missing %s", GERMAN_LOCALE);
    return true;
  }
  return false;
}

void LocaleTestBase::onRun() {
  LocaleTestBase::Guard guard;

  if (LocaleTestBase::ShouldWeSkip()) {
    return;
  }

  localeOnRun();
}

}}}}
