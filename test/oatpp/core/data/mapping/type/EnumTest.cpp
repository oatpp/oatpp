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

#include "EnumTest.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(Enum1, v_int32,
  VALUE(NAME_1, 1, "name-1"),
  VALUE(NAME_2, 2, "name-2"),
  VALUE(NAME_3, 3, "name-3")
)

#include OATPP_CODEGEN_END(DTO)

void EnumTest::onRun() {

  {
    OATPP_LOGI(TAG, "Declaration...");
    OATPP_ASSERT(oatpp::Enum<Enum1>::Interpreter::isNullable == true);
    OATPP_ASSERT(oatpp::Enum<Enum1>::NotNull::Interpreter::isNullable == false);

    OATPP_ASSERT(oatpp::Enum<Enum1>::AsString::Interpreter::isNullable == true);
    OATPP_ASSERT(oatpp::Enum<Enum1>::AsString::NotNull::Interpreter::isNullable == false);

    OATPP_ASSERT(oatpp::Enum<Enum1>::AsInteger::Interpreter::isNullable == true);
    OATPP_ASSERT(oatpp::Enum<Enum1>::AsInteger::NotNull::Interpreter::isNullable == false);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Interpreter AsString...");
    auto inter = oatpp::Enum<Enum1>::AsString::Interpreter::toInterpretation(Enum1::NAME_1);
    OATPP_ASSERT(inter.valueType == oatpp::String::Class::getType());

    auto interValue = inter.staticCast<oatpp::String>();
    OATPP_ASSERT(interValue == "name-1");

    Enum1 value = oatpp::Enum<Enum1>::AsString::Interpreter::fromInterpretation(interValue);
    OATPP_ASSERT(value == Enum1::NAME_1);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Interpreter AsInteger...");
    auto inter = oatpp::Enum<Enum1>::AsInteger::Interpreter::toInterpretation(Enum1::NAME_1);
    OATPP_ASSERT(inter.valueType == oatpp::Int32::Class::getType());

    auto interValue = inter.staticCast<oatpp::Int32>();
    OATPP_ASSERT(interValue == static_cast<v_int32>(Enum1::NAME_1));

    Enum1 value = oatpp::Enum<Enum1>::AsInteger::Interpreter::fromInterpretation(interValue);
    OATPP_ASSERT(value == Enum1::NAME_1);
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}
