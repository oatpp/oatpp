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

#include "PrimitiveTest.hpp"

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

namespace {

  template<class T>
  void checkHash(const T& val) {
    std::hash<T>{}(val);
  }

}

void PrimitiveTest::onRun() {

  {
    //checkHash(oatpp::Int8(8));
    //checkHash(oatpp::UInt8(8));
  }

  {
    OATPP_LOGI(TAG, "test default constructor");
    oatpp::Int32 i;
    OATPP_ASSERT(!i);
    OATPP_ASSERT(i == nullptr);
    OATPP_ASSERT(i.valueType == oatpp::Int32::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test value constructor");
    oatpp::Int32 i(0);
    OATPP_ASSERT(i);
    OATPP_ASSERT(i != nullptr);
    OATPP_ASSERT(i == 0);
    OATPP_ASSERT(i.valueType == oatpp::Int32::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test implicit value constructor");
    oatpp::Int32 i = 0;
    OATPP_ASSERT(i);
    OATPP_ASSERT(i != nullptr);
    OATPP_ASSERT(i == 0);
    OATPP_ASSERT(i.valueType == oatpp::Int32::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test '==' and '!=' operators");
    oatpp::Int32 i1 = 0;
    oatpp::Int32 i2;

    OATPP_ASSERT(i1);
    OATPP_ASSERT(i1 != nullptr);
    OATPP_ASSERT(i1 == 0);
    OATPP_ASSERT(i1 != 1);

    OATPP_ASSERT(!i2);
    OATPP_ASSERT(i2 == nullptr)

    OATPP_ASSERT(i1 != i2);
    OATPP_ASSERT(i2 != i1);

    i2 = 0;

    OATPP_ASSERT(i1 == i2);
    OATPP_ASSERT(i2 == i1);

    i1 = nullptr;
    i2 = nullptr;

    OATPP_ASSERT(i1 == i2);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test copy-assign operator");
    oatpp::Int32 i1 = 0;
    oatpp::Int32 i2;

    OATPP_ASSERT(i1 != i2);

    i2 = i1;

    OATPP_ASSERT(i1 == i2);
    OATPP_ASSERT(i1.get() == i2.get());

    i2 = 1;

    OATPP_ASSERT(i1 != i2);
    OATPP_ASSERT(i1.get() != i2.get());

    OATPP_ASSERT(i1 == 0);
    OATPP_ASSERT(i2 == 1);
  }

  {
    OATPP_LOGI(TAG, "test move-assign operator");
    oatpp::Int32 i1 = 0;
    oatpp::Int32 i2;

    OATPP_ASSERT(i1 != i2);

    i2 = std::move(i1);

    OATPP_ASSERT(i1 == nullptr);
    OATPP_ASSERT(i2 != nullptr);

    OATPP_ASSERT(i2 == 0);
  }

  {
    OATPP_LOGI(TAG, "Test Boolean [nullptr]");
    oatpp::Boolean b;

    OATPP_ASSERT(!b);
    OATPP_ASSERT(b == nullptr);
    OATPP_ASSERT(b != false);
    OATPP_ASSERT(b != true);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Boolean [false]");
    oatpp::Boolean b = false;

    OATPP_ASSERT(!b); // <--- still !b
    OATPP_ASSERT(b != nullptr);
    OATPP_ASSERT(b == false);
    OATPP_ASSERT(b != true);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Boolean [true]");
    oatpp::Boolean b = true;

    OATPP_ASSERT(b);
    OATPP_ASSERT(b != nullptr);
    OATPP_ASSERT(b != false);
    OATPP_ASSERT(b == true);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Boolean copy-assign operator");
    oatpp::Boolean b1 = true;
    oatpp::Boolean b2;

    b2 = b1;

    OATPP_ASSERT(b2);
    OATPP_ASSERT(b1);
    OATPP_ASSERT(b1 == b2);
    OATPP_ASSERT(b1.get() == b2.get());

    b2 = false;

    OATPP_ASSERT(b1.get() != b2.get());
    OATPP_ASSERT(b1 != b2);
    OATPP_ASSERT(b2 != b1);

    b1 = false;
    b2 = nullptr;

    OATPP_ASSERT(b1 != b2);
    OATPP_ASSERT(b2 != b1);

    b1 = nullptr;
    b2 = nullptr;

    OATPP_ASSERT(b1 == b2);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Boolean move-assign operator");
    oatpp::Boolean b1 = true;
    oatpp::Boolean b2;

    b2 = std::move(b1);

    OATPP_ASSERT(b2 != nullptr);
    OATPP_ASSERT(b1 == nullptr);

    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}