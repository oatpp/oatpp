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

ENUM(Enum2, v_int32,
     VALUE(NAME_1, 1, "name-1"),
     VALUE(NAME_2, 2, "name-2"),
     VALUE(NAME_3, 3, "name-3")
)

ENUM(Enum3, v_int32)

#include OATPP_CODEGEN_END(DTO)

void EnumTest::onRun() {

  {
    OATPP_LOGI(TAG, "Declaration...");
    OATPP_ASSERT(oatpp::Enum<Enum1>::Interpreter::notNull == false);
    OATPP_ASSERT(oatpp::Enum<Enum1>::NotNull::Interpreter::notNull == true);

    OATPP_ASSERT(oatpp::Enum<Enum1>::AsString::Interpreter::notNull == false);
    OATPP_ASSERT(oatpp::Enum<Enum1>::AsString::NotNull::Interpreter::notNull == true);

    OATPP_ASSERT(oatpp::Enum<Enum1>::AsInteger::Interpreter::notNull == false);
    OATPP_ASSERT(oatpp::Enum<Enum1>::AsInteger::NotNull::Interpreter::notNull == true);

    OATPP_ASSERT(oatpp::Enum<Enum1>::NotNull::AsString::Interpreter::notNull == true);
    OATPP_ASSERT(oatpp::Enum<Enum1>::NotNull::AsInteger::Interpreter::notNull == true);

    auto pd1 = static_cast<const oatpp::data::mapping::type::__class::AbstractEnum::AbstractPolymorphicDispatcher*>(
      oatpp::Enum<Enum1>::Class::getType()->polymorphicDispatcher
    );

    auto pd2 = static_cast<const oatpp::data::mapping::type::__class::AbstractEnum::AbstractPolymorphicDispatcher*>(
      oatpp::Enum<Enum1>::NotNull::Class::getType()->polymorphicDispatcher
    );

    OATPP_ASSERT(pd1->notNull == false);
    OATPP_ASSERT(pd2->notNull == true);

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

  {
    OATPP_LOGI(TAG, "Test default constructors and == operators...");
    oatpp::Enum<Enum1>::AsString e1;
    oatpp::Enum<Enum1>::AsString e2;

    OATPP_ASSERT(!e1);
    OATPP_ASSERT(e1 == nullptr);
    OATPP_ASSERT(e1 == e2);
    OATPP_ASSERT(e2 == e1);

    oatpp::Enum<Enum1>::NotNull e3;

    OATPP_ASSERT(e1 == e3);
    OATPP_ASSERT(e3 == e1);

    oatpp::Enum<Enum1>::AsInteger::NotNull e4;

    OATPP_ASSERT(e1 == e4);
    OATPP_ASSERT(e4 == e1);

    OATPP_ASSERT(e1.valueType != e4.valueType); // Types are not equal because interpreters are different
    OATPP_ASSERT(e1.valueType->classId.id == e4.valueType->classId.id); // But classId is the same

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test value constructor and == operators...");
    oatpp::Enum<Enum1> e1(Enum1::NAME_1);
    oatpp::Enum<Enum1> e2(Enum1::NAME_1);
    oatpp::Enum<Enum1> e3;

    OATPP_ASSERT(e1);
    OATPP_ASSERT(e1 != nullptr);
    OATPP_ASSERT(e1 == e2);
    OATPP_ASSERT(e1 != e3);
    OATPP_ASSERT(e3 != e1);

    OATPP_ASSERT(e1 == Enum1::NAME_1);
    OATPP_ASSERT(e1 != Enum1::NAME_2);
    OATPP_ASSERT(e3 != Enum1::NAME_1);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test copy-assignment operator...");
    oatpp::Enum<Enum1>::AsString e1;
    oatpp::Enum<Enum1>::AsInteger e2(Enum1::NAME_1);
    Enum1 ve;

    OATPP_ASSERT(e1.valueType == oatpp::Enum<Enum1>::AsString::Class::getType());
    OATPP_ASSERT(e2.valueType == oatpp::Enum<Enum1>::AsInteger::Class::getType());
    OATPP_ASSERT(e1.valueType != e2.valueType);

    e1 = e2;

    OATPP_ASSERT(e1.valueType == oatpp::Enum<Enum1>::AsString::Class::getType());
    OATPP_ASSERT(e2.valueType == oatpp::Enum<Enum1>::AsInteger::Class::getType());
    OATPP_ASSERT(e1.valueType != e2.valueType);

    OATPP_ASSERT(e1 == e2);
    OATPP_ASSERT(e2 == e1);
    OATPP_ASSERT(e1.get() == e2.get());

    e1 = Enum1::NAME_2;

    OATPP_ASSERT(e1 != e2);
    OATPP_ASSERT(e2 != e1);
    OATPP_ASSERT(e1.get() != e2.get());

    OATPP_ASSERT(e1 == Enum1::NAME_2);
    OATPP_ASSERT(e2 == Enum1::NAME_1);

    ve = e1;

    OATPP_ASSERT(ve == Enum1::NAME_2);

    ve = e2;

    OATPP_ASSERT(ve == Enum1::NAME_1);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test move-assignment operator...");
    oatpp::Enum<Enum1>::AsString e1;
    oatpp::Enum<Enum1>::AsInteger e2(Enum1::NAME_1);

    e1 = std::move(e2);

    OATPP_ASSERT(e1);
    OATPP_ASSERT(!e2);

    OATPP_ASSERT(e1 == Enum1::NAME_1);

    OATPP_LOGI(TAG, "OK");
  }


}

}}}}}}
