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

#include "StringTest.hpp"

#include "oatpp/core/Types.hpp"

#include <functional>

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

void StringTest::onRun() {

  {
    oatpp::String s = "hello"; // check hash function exists
    std::hash<oatpp::String>{}(s);
  }

  {
    OATPP_LOGI(TAG, "test default constructor");
    oatpp::String s;
    OATPP_ASSERT(!s);
    OATPP_ASSERT(s == nullptr);
    OATPP_ASSERT(s == (const char*) nullptr);
    OATPP_ASSERT(s.valueType == oatpp::String::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test const char* constructor");
    oatpp::String s("");
    OATPP_ASSERT(s);
    OATPP_ASSERT(s != nullptr);
    OATPP_ASSERT(s != (const char*) nullptr)
    OATPP_ASSERT(s->getSize() == 0);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test const char* implicit constructor");
    oatpp::String s = "";
    OATPP_ASSERT(s);
    OATPP_ASSERT(s != nullptr);
    OATPP_ASSERT(s != (const char*) nullptr)
    OATPP_ASSERT(s->getSize() == 0);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test '==', '!=' operators");
    oatpp::String s1 = "a";
    oatpp::String s2;

    OATPP_ASSERT(s1 != s2);
    OATPP_ASSERT(s2 != s1);

    OATPP_ASSERT(s1 == "a");
    OATPP_ASSERT(s1 != "aa");
    OATPP_ASSERT(s1 != "");

    s2 = "aa";

    OATPP_ASSERT(s1 != s2);
    OATPP_ASSERT(s2 != s1);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test copy-asssign operator");
    oatpp::String s1 = "s1";
    oatpp::String s2;

    s2 = s1;

    OATPP_ASSERT(s1 == s2);
    OATPP_ASSERT(s1.get() == s2.get());

    s1 = "s2";

    OATPP_ASSERT(s1 != s2);
    OATPP_ASSERT(s2 != s1);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test const char* assign operator");
    oatpp::String s1 = "s1";
    oatpp::String s2(s1);

    OATPP_ASSERT(s1 == s2);
    OATPP_ASSERT(s1.get() == s2.get());

    s1 = "s2";

    OATPP_ASSERT(s1 != s2);
    OATPP_ASSERT(s2 != s1);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test move assign operator");
    oatpp::String s1 = "s1";
    oatpp::String s2;

    s2 = std::move(s1);

    OATPP_ASSERT(s1 == nullptr);
    OATPP_ASSERT(s2 != nullptr);
    OATPP_ASSERT(s2 == "s1");

    OATPP_ASSERT(s1 != s2);
    OATPP_ASSERT(s1.get() != s2.get());
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}
