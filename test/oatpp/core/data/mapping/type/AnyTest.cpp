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

#include "AnyTest.hpp"

#include "oatpp/core/data/mapping/type/Any.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class Dto1 : public oatpp::DTO {
  DTO_INIT(Dto1, DTO);
};

class Dto2 : public oatpp::DTO {
  DTO_INIT(Dto2, DTO);
};

class Test : public oatpp::DTO {

  DTO_INIT(Test, DTO);

  DTO_FIELD(oatpp::Any, any);

};

#include OATPP_CODEGEN_END(DTO)

}

void AnyTest::onRun() {

  {
    OATPP_LOGI(TAG, "Test default constructor...");
    oatpp::Any any;
    OATPP_ASSERT(!any);
    OATPP_ASSERT(any.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any.getStoredType() == nullptr);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test nullptr constructor...");
    oatpp::Any any(nullptr);
    OATPP_ASSERT(!any);
    OATPP_ASSERT(any.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any.getStoredType() == nullptr);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test retrieve()...");
    oatpp::Any any(oatpp::String("Hello Any!"));
    OATPP_ASSERT(any);
    OATPP_ASSERT(any.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any.getStoredType() == oatpp::data::mapping::type::__class::String::getType());
    auto str = any.retrieve<oatpp::String>();
    OATPP_ASSERT(str == "Hello Any!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test store()...");
    oatpp::Any any(oatpp::Int32(32));

    OATPP_ASSERT(any);
    OATPP_ASSERT(any.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any.getStoredType() == oatpp::data::mapping::type::__class::Int32::getType());

    any.store(oatpp::String("Hello Any!"));

    OATPP_ASSERT(any);
    OATPP_ASSERT(any.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any.getStoredType() == oatpp::data::mapping::type::__class::String::getType());

    auto str = any.retrieve<oatpp::String>();
    OATPP_ASSERT(str == "Hello Any!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test retrieve() class check...");
    oatpp::Any any(Dto1::createShared());
    OATPP_ASSERT(any);
    OATPP_ASSERT(any.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any.getStoredType() == Object<Dto1>::Class::getType());

    bool wasError = false;

    try {
      auto obj = any.retrieve<oatpp::Object<Dto2>>(); // wrong object
    } catch (std::runtime_error&) {
      wasError = true;
    }

    OATPP_ASSERT(wasError);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test copy-assign operator...");
    oatpp::Any any1(oatpp::String("Hello!"));
    oatpp::Any any2;

    any2 = any1;

    OATPP_ASSERT(any1);
    OATPP_ASSERT(any2);

    OATPP_ASSERT(any1.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any2.getValueType() == oatpp::data::mapping::type::__class::Any::getType());

    OATPP_ASSERT(any1.getStoredType() == oatpp::data::mapping::type::__class::String::getType());
    OATPP_ASSERT(any2.getStoredType() == oatpp::data::mapping::type::__class::String::getType());

    OATPP_ASSERT(any1 == any2);
    OATPP_ASSERT(any1.getPtr().get() != any2.getPtr().get());

    auto str1 = any1.retrieve<oatpp::String>();
    auto str2 = any2.retrieve<oatpp::String>();

    OATPP_ASSERT(str1 == str2);
    OATPP_ASSERT(str1.get() == str2.get() && str1 == "Hello!");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test move-assign operator...");
    oatpp::Any any1(oatpp::String("Hello!"));
    oatpp::Any any2;

    any2 = std::move(any1);

    OATPP_ASSERT(!any1);
    OATPP_ASSERT(any2);

    OATPP_ASSERT(any1.getValueType() == oatpp::data::mapping::type::__class::Any::getType());
    OATPP_ASSERT(any2.getValueType() == oatpp::data::mapping::type::__class::Any::getType());

    OATPP_ASSERT(any1.getStoredType() == nullptr);
    OATPP_ASSERT(any2.getStoredType() == oatpp::data::mapping::type::__class::String::getType());

    OATPP_ASSERT(any1 != any2);
    OATPP_ASSERT(any1.getPtr().get() != any2.getPtr().get());

    auto str1 = any1.retrieve<oatpp::String>();
    auto str2 = any2.retrieve<oatpp::String>();

    OATPP_ASSERT(str1 != str2);
    OATPP_ASSERT(str2 == "Hello!");
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}