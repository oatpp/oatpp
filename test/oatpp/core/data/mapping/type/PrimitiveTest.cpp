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
    auto h = std::hash<T>{}(val);
    OATPP_LOGI("HASH", "type='%s', hash=%llu", val.getValueType()->classId.name, h);
  }

}

void PrimitiveTest::onRun() {

  {
    checkHash(oatpp::Boolean(true));
    checkHash(oatpp::Int8(0x7F));
    checkHash(oatpp::UInt8(0xFF));
    checkHash(oatpp::Int16(0x7FFF));
    checkHash(oatpp::UInt16(0xFFFF));
    checkHash(oatpp::Int32(0x7FFFFFFF));
    checkHash(oatpp::UInt32(0xFFFFFFFF));
    checkHash(oatpp::Int64(0x7FFFFFFFFFFFFFFF));
    checkHash(oatpp::UInt64(0xFFFFFFFFFFFFFFFF));
    checkHash(oatpp::Float32(0.2f));
    checkHash(oatpp::Float64(0.2));
  }

  {
    OATPP_LOGI(TAG, "test default constructor");
    oatpp::Int32 i;
    OATPP_ASSERT(!i);
    OATPP_ASSERT(i == nullptr);
    OATPP_ASSERT(i.getValueType() == oatpp::Int32::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test value constructor");
    oatpp::Int32 i(0);
    OATPP_ASSERT(i);
    OATPP_ASSERT(i != nullptr);
    OATPP_ASSERT(i == 0);
    OATPP_ASSERT(i.getValueType() == oatpp::Int32::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test implicit value constructor");
    oatpp::Int32 i = 0;
    OATPP_ASSERT(i);
    OATPP_ASSERT(i != nullptr);
    OATPP_ASSERT(i == 0);
    OATPP_ASSERT(i.getValueType() == oatpp::Int32::Class::getType());
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
    OATPP_LOGI(TAG, "test move-assign operator");
    oatpp::Int32 i = 0;
    v_int32 v = i;
    OATPP_ASSERT(v == i);
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
    OATPP_LOGI(TAG, "Test Boolean nullptr constructor");
    oatpp::Boolean b = nullptr;

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

  {
    OATPP_LOGI(TAG, "check default value");
    oatpp::UInt8 s0;
    oatpp::UInt8 s1 = 255;
    OATPP_ASSERT(s0.getValue(128) == 128)
    OATPP_ASSERT(s1.getValue(128) == 255)
  }

}

}}}}}}