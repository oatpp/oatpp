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

#include "StringTemplateTest.hpp"

#include "oatpp/core/data/share/StringTemplate.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace share {

void StringTemplateTest::onRun() {

  typedef oatpp::data::share::StringTemplate StringTemplate;

  {
    OATPP_LOGI(TAG, "Case1 ...");
    StringTemplate t("{} World!", {{0, 1, "p1"}});
    auto result = t.format(std::vector<oatpp::String>({"Hello"}));
    OATPP_ASSERT(result == "Hello World!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case2 ...");
    StringTemplate t("{} World!", {{0, 1, "p1"}});
    auto result = t.format(std::unordered_map<oatpp::String, oatpp::String>({{"p1", "Hello"}}));
    OATPP_ASSERT(result == "Hello World!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case3 ...");
    StringTemplate t("Hello {}", {{6, 7, "p1"}});
    auto result = t.format(std::vector<oatpp::String>({"World!"}));
    OATPP_ASSERT(result == "Hello World!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case4 ...");
    StringTemplate t("Hello {}", {{6, 7, "p1"}});
    auto result = t.format(std::unordered_map<oatpp::String, oatpp::String>({{"p1", "World!"}}));
    OATPP_ASSERT(result == "Hello World!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case5 ...");
    StringTemplate t("Hello {} World!", {{6, 7, "p1"}});
    auto result = t.format(std::vector<oatpp::String>({"My"}));
    OATPP_ASSERT(result == "Hello My World!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case6 ...");
    StringTemplate t("Hello {} World!", {{6, 7, "p1"}});
    auto result = t.format(std::unordered_map<oatpp::String, oatpp::String>({{"p1", "My"}}));
    OATPP_ASSERT(result == "Hello My World!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case7 ...");
    StringTemplate t("? ? ?", {{0, 0, "p1"}, {2, 2, "p2"}, {4, 4, "p3"}});
    auto result = t.format(std::vector<oatpp::String>({"Hello", "World", "Oat++!"}));
    OATPP_ASSERT(result == "Hello World Oat++!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case8 ...");
    StringTemplate t("? ? ?", {{0, 0, "p1"}, {2, 2, "p2"}, {4, 4, "p3"}});
    auto result = t.format(std::unordered_map<oatpp::String, oatpp::String>({{"p3", "Hello"}, {"p2", "World"}, {"p1", "Oat++!"}}));
    OATPP_ASSERT(result == "Oat++! World Hello");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case9 ...");
    StringTemplate t("? ? ?", {{0, 0, "p1"}, {2, 2, "p2"}, {4, 4, "p3"}});
    auto result = t.format("A");
    OATPP_ASSERT(result == "A A A");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Case10 ...");
    StringTemplate t("? ? ?",
      {
        {0, 0, "p1", std::make_shared<oatpp::base::Countable>()},
        {2, 2, "p2", std::make_shared<oatpp::base::Countable>()},
        {4, 4, "p3", std::make_shared<oatpp::base::Countable>()}
      }
    );
    auto result = t.format("(A)");
    OATPP_ASSERT(result == "(A) (A) (A)");
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}
