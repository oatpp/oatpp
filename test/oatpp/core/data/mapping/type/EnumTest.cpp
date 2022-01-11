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

#include <unordered_map>

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(Enum0, v_int32)

ENUM(Enum1, v_int32,
     VALUE(V1, 1),
     VALUE(V2, 2),
     VALUE(V3, 3)
)

ENUM(Enum2, v_int32,
  VALUE(NAME_1, 1, "name-1"),
  VALUE(NAME_2, 2, "name-2"),
  VALUE(NAME_3, 3, "name-3")
)

ENUM(Enum3, v_int32,
   VALUE(V_1, 1, "v-1", "description_1"),
   VALUE(V_2, 2, "v-2", "description_2"),
   VALUE(V_3, 3, "v-3", "description_3")
)

#include OATPP_CODEGEN_END(DTO)

void EnumTest::onRun() {

  {
    OATPP_LOGI(TAG, "Check Hash...");

    {
      auto v = std::hash<oatpp::Enum<Enum1>>{}(oatpp::Enum<Enum1>());
      OATPP_ASSERT(v == 0);
    }

    {
      auto v = std::hash<oatpp::Enum<Enum1>>{}(oatpp::Enum<Enum1>(Enum1::V1));
      OATPP_ASSERT(v == 1);
    }

    {
      auto v = std::hash<oatpp::Enum<Enum1>>{}(oatpp::Enum<Enum1>(Enum1::V2));
      OATPP_ASSERT(v == 2);
    }

    {
      auto v = std::hash<oatpp::Enum<Enum1>>{}(oatpp::Enum<Enum1>(Enum1::V3));
      OATPP_ASSERT(v == 3);
    }

    std::unordered_map<oatpp::Enum<Enum1>, oatpp::String> map({
      {Enum1::V1, "v1"},
      {Enum1::V2, "v2"},
      {Enum1::V3, "v3"},
    });

    OATPP_ASSERT(map.size() == 3);
    OATPP_ASSERT(map[Enum1::V1] == "v1");
    OATPP_ASSERT(map[Enum1::V2] == "v2");
    OATPP_ASSERT(map[Enum1::V3] == "v3");

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Check Meta...");
    {
      const auto &entries = oatpp::Enum<Enum0>::getEntries();
      OATPP_ASSERT(entries.size() == 0);
    }

    {
      const auto &v = oatpp::Enum<Enum1>::getEntries();
      OATPP_ASSERT(v.size() == 3);
      OATPP_ASSERT(v[0].index == 0 && v[0].name == "V1" && v[0].value == Enum1::V1 && v[0].description == nullptr);
      OATPP_ASSERT(v[1].index == 1 && v[1].name == "V2" && v[1].value == Enum1::V2 && v[1].description == nullptr);
      OATPP_ASSERT(v[2].index == 2 && v[2].name == "V3" && v[2].value == Enum1::V3 && v[2].description == nullptr);
    }

    {
      const auto &v = oatpp::Enum<Enum2>::getEntries();
      OATPP_ASSERT(v.size() == 3);
      OATPP_ASSERT(v[0].index == 0 && v[0].name == "name-1" && v[0].value == Enum2::NAME_1 && v[0].description == nullptr);
      OATPP_ASSERT(v[1].index == 1 && v[1].name == "name-2" && v[1].value == Enum2::NAME_2 && v[1].description == nullptr);
      OATPP_ASSERT(v[2].index == 2 && v[2].name == "name-3" && v[2].value == Enum2::NAME_3 && v[2].description == nullptr);
    }

    {
      const auto &v = oatpp::Enum<Enum3>::getEntries();
      OATPP_ASSERT(v.size() == 3);
      OATPP_ASSERT(v[0].index == 0 && v[0].name == "v-1" && v[0].value == Enum3::V_1 && v[0].description == "description_1");
      OATPP_ASSERT(v[1].index == 1 && v[1].name == "v-2" && v[1].value == Enum3::V_2 && v[1].description == "description_2");
      OATPP_ASSERT(v[2].index == 2 && v[2].name == "v-3" && v[2].value == Enum3::V_3 && v[2].description == "description_3");
    }

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Declaration...");
    OATPP_ASSERT(oatpp::Enum<Enum2>::Interpreter::notNull == false);
    OATPP_ASSERT(oatpp::Enum<Enum2>::NotNull::Interpreter::notNull == true);

    OATPP_ASSERT(oatpp::Enum<Enum2>::AsString::Interpreter::notNull == false);
    OATPP_ASSERT(oatpp::Enum<Enum2>::AsString::NotNull::Interpreter::notNull == true);

    OATPP_ASSERT(oatpp::Enum<Enum2>::AsNumber::Interpreter::notNull == false);
    OATPP_ASSERT(oatpp::Enum<Enum2>::AsNumber::NotNull::Interpreter::notNull == true);

    OATPP_ASSERT(oatpp::Enum<Enum2>::NotNull::AsString::Interpreter::notNull == true);
    OATPP_ASSERT(oatpp::Enum<Enum2>::NotNull::AsNumber::Interpreter::notNull == true);

    auto pd1 = static_cast<const oatpp::data::mapping::type::__class::AbstractEnum::PolymorphicDispatcher*>(
      oatpp::Enum<Enum2>::Class::getType()->polymorphicDispatcher
    );

    auto pd2 = static_cast<const oatpp::data::mapping::type::__class::AbstractEnum::PolymorphicDispatcher*>(
      oatpp::Enum<Enum2>::NotNull::Class::getType()->polymorphicDispatcher
    );

    OATPP_ASSERT(pd1->notNull == false);
    OATPP_ASSERT(pd2->notNull == true);

    {
      auto interEnum = pd1->getInterpretedEnum();
      OATPP_ASSERT(interEnum.size() == 3);
      OATPP_ASSERT(interEnum[0].getStoredType() == oatpp::String::Class::getType());
    }

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Interpreter AsString...");
    oatpp::data::mapping::type::EnumInterpreterError e = oatpp::data::mapping::type::EnumInterpreterError::OK;
    auto inter = oatpp::Enum<Enum2>::AsString::Interpreter::toInterpretation(oatpp::Enum<Enum2>::AsString(Enum2::NAME_1), e);
    OATPP_ASSERT(inter.getValueType() == oatpp::String::Class::getType());
    OATPP_ASSERT(e == oatpp::data::mapping::type::EnumInterpreterError::OK);

    auto interValue = inter.cast<oatpp::String>();
    OATPP_ASSERT(interValue == "name-1");

    oatpp::Void voidValue = oatpp::Enum<Enum2>::AsString::Interpreter::fromInterpretation(interValue, e);
    OATPP_ASSERT(voidValue.getValueType() == oatpp::Enum<Enum2>::AsString::Class::getType());
    OATPP_ASSERT(e == oatpp::data::mapping::type::EnumInterpreterError::OK);

    auto value = voidValue.cast<oatpp::Enum<Enum2>::AsString>();
    OATPP_ASSERT(value == Enum2::NAME_1);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Interpreter AsNumber...");
    oatpp::data::mapping::type::EnumInterpreterError e = oatpp::data::mapping::type::EnumInterpreterError::OK;
    auto inter = oatpp::Enum<Enum2>::AsNumber::Interpreter::toInterpretation(oatpp::Enum<Enum2>::AsNumber(Enum2::NAME_1), e);
    OATPP_ASSERT(inter.getValueType() == oatpp::Int32::Class::getType());
    OATPP_ASSERT(e == oatpp::data::mapping::type::EnumInterpreterError::OK);

    auto interValue = inter.cast<oatpp::Int32>();
    OATPP_ASSERT(interValue == static_cast<v_int32>(Enum2::NAME_1));

    oatpp::Void voidValue = oatpp::Enum<Enum2>::AsNumber::Interpreter::fromInterpretation(interValue, e);
    OATPP_ASSERT(voidValue.getValueType() == oatpp::Enum<Enum2>::AsNumber::Class::getType());
    OATPP_ASSERT(e == oatpp::data::mapping::type::EnumInterpreterError::OK);

    auto value = voidValue.cast<oatpp::Enum<Enum2>::AsNumber>();
    OATPP_ASSERT(value == Enum2::NAME_1);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test default constructors and == operators...");
    oatpp::Enum<Enum2>::AsString e1;
    oatpp::Enum<Enum2>::AsString e2;

    OATPP_ASSERT(!e1);
    OATPP_ASSERT(e1 == nullptr);
    OATPP_ASSERT(e1 == e2);
    OATPP_ASSERT(e2 == e1);

    oatpp::Enum<Enum2>::NotNull e3;

    OATPP_ASSERT(e1 == e3);
    OATPP_ASSERT(e3 == e1);

    oatpp::Enum<Enum2>::AsNumber::NotNull e4;

    OATPP_ASSERT(e1 == e4);
    OATPP_ASSERT(e4 == e1);

    OATPP_ASSERT(e1.getValueType() != e4.getValueType()); // Types are not equal because interpreters are different
    OATPP_ASSERT(e1.getValueType()->classId.id == e4.getValueType()->classId.id); // But classId is the same

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test value constructor and == operators...");
    oatpp::Enum<Enum2> e1(Enum2::NAME_1);
    oatpp::Enum<Enum2> e2(Enum2::NAME_1);
    oatpp::Enum<Enum2> e3;

    OATPP_ASSERT(e1);
    OATPP_ASSERT(e1 != nullptr);
    OATPP_ASSERT(e1 == e2);
    OATPP_ASSERT(e1 != e3);
    OATPP_ASSERT(e3 != e1);

    OATPP_ASSERT(e1 == Enum2::NAME_1);
    OATPP_ASSERT(e1 != Enum2::NAME_2);
    OATPP_ASSERT(e3 != Enum2::NAME_1);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test copy-assignment operator...");
    oatpp::Enum<Enum2>::AsString e1;
    oatpp::Enum<Enum2>::AsNumber e2(Enum2::NAME_1);
    Enum2 ve;

    OATPP_ASSERT(e1.getValueType() == oatpp::Enum<Enum2>::AsString::Class::getType());
    OATPP_ASSERT(e2.getValueType() == oatpp::Enum<Enum2>::AsNumber::Class::getType());
    OATPP_ASSERT(e1.getValueType() != e2.getValueType());

    e1 = e2;

    OATPP_ASSERT(e1.getValueType() == oatpp::Enum<Enum2>::AsString::Class::getType());
    OATPP_ASSERT(e2.getValueType() == oatpp::Enum<Enum2>::AsNumber::Class::getType());
    OATPP_ASSERT(e1.getValueType() != e2.getValueType());

    OATPP_ASSERT(e1 == e2);
    OATPP_ASSERT(e2 == e1);
    OATPP_ASSERT(e1.get() == e2.get());

    e1 = Enum2::NAME_2;

    OATPP_ASSERT(e1 != e2);
    OATPP_ASSERT(e2 != e1);
    OATPP_ASSERT(e1.get() != e2.get());

    OATPP_ASSERT(e1 == Enum2::NAME_2);
    OATPP_ASSERT(e2 == Enum2::NAME_1);

    ve = e1;

    OATPP_ASSERT(ve == Enum2::NAME_2);

    ve = e2;

    OATPP_ASSERT(ve == Enum2::NAME_1);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test move-assignment operator...");
    oatpp::Enum<Enum2>::AsString e1;
    oatpp::Enum<Enum2>::AsNumber e2(Enum2::NAME_1);

    e1 = std::move(e2);

    OATPP_ASSERT(e1);
    OATPP_ASSERT(!e2);

    OATPP_ASSERT(e1 == Enum2::NAME_1);

    OATPP_LOGI(TAG, "OK");
  }


}

}}}}}}
