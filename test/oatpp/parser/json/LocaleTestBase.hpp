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

#ifndef oatpp_test_parser_json_LocaleTestHelper_hpp
#define oatpp_test_parser_json_LocaleTestHelper_hpp

#include <clocale>
#include <cstddef>
#include <cstring>

#include "oatpp-test/UnitTest.hpp"
#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace test { namespace parser { namespace json {

class LocaleTestBase : public UnitTest {
public:
  static constexpr size_t BUFFER_SIZE = 128;
  static constexpr char US_LOCALE[] = "en_US.utf8";
  static constexpr char GERMAN_LOCALE[] = "de_DE.utf8";
  static constexpr char NUMBER_WITH_POINT[] = "-123456.125";
  static constexpr char NUMBER_WITH_COMMA[] = "-123456,125";
  static constexpr char NUMBER_WITH_POINT_AND_OTHERS[] = R"(-123456.125,"other_things":1.1)";
  static constexpr char NUMBER_WITH_COMMA_AND_OTHERS[] = R"(-123456,125,"other_things":1.1)";
  static constexpr v_float32 NUMBER_FLOAT32 = -123456.125;
  static constexpr v_float64 NUMBER_FLOAT64 = -123456.125;

  class Guard {
   public:
    Guard();
    ~Guard();
   private:
    char original_locale[BUFFER_SIZE];
  };

  explicit LocaleTestBase(const char* testTAG):UnitTest(testTAG) {}
  virtual void localeOnRun() = 0;

 protected:
  char json_buffer[BUFFER_SIZE];
  const p_char8 json_string = (p_char8)json_buffer;
  const p_char8 json_end = json_string + BUFFER_SIZE;
  char locale_buffer[BUFFER_SIZE];
  const p_char8 locale_string = (p_char8)locale_buffer;
  const p_char8 locale_end = locale_string + BUFFER_SIZE;

 private:
  bool ShouldWeSkip() const;
  void onRun() override;
};

}}}}

#endif /* oatpp_test_parser_json_LocaleTestHelper_hpp */
