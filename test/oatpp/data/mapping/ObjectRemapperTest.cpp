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

#include "ObjectRemapperTest.hpp"

#include "oatpp/data/mapping/ObjectRemapper.hpp"

#include "oatpp/macro/codegen.hpp"

#include <iostream>

namespace oatpp { namespace data { namespace mapping {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(TestEnum1, v_int32,
     VALUE(VALUE_1, 1, "value-1"),
     VALUE(VALUE_2, 2, "value-2")
)

ENUM(TestEnum2, v_int32,
     VALUE(VALUE_1, 1, "other-value-1"),
     VALUE(VALUE_2, 2, "other-value-2")
)

class TestDto1 : public oatpp::DTO {

  DTO_INIT(TestDto1, DTO)

  DTO_FIELD(String, field1);
  DTO_FIELD(String, field2, "field-2");

  DTO_FIELD(Enum<TestEnum1>, enum1);
  DTO_FIELD(Enum<TestEnum1>, enum2, "enum-2");

  DTO_FIELD(Int32 , i32);

};

class TestDto2 : public oatpp::DTO {

  DTO_INIT(TestDto2, DTO)

  DTO_FIELD(String, field1);
  DTO_FIELD(String, field2, "other-field-2");

  DTO_FIELD(Enum<TestEnum2>, enum1);
  DTO_FIELD(Enum<TestEnum2>, enum2, "other-enum-2");

};

class PrimitivesDto : public oatpp::DTO {

  DTO_INIT(PrimitivesDto, DTO)

  DTO_FIELD(Boolean, b);

  DTO_FIELD(Int8, i8);
  DTO_FIELD(UInt8, ui8);

  DTO_FIELD(Int16, i16);
  DTO_FIELD(UInt16, ui16);

  DTO_FIELD(Int32, i32);
  DTO_FIELD(UInt32, ui32);

  DTO_FIELD(Int64, i64);
  DTO_FIELD(UInt64, ui64);

  DTO_FIELD(Float32, f32);
  DTO_FIELD(Float64, f64);

};

class TextPrimitivesDto : public oatpp::DTO {

  DTO_INIT(TextPrimitivesDto, DTO)

  DTO_FIELD(String, b);

  DTO_FIELD(String, i8);
  DTO_FIELD(String, ui8);

  DTO_FIELD(String, i16);
  DTO_FIELD(String, ui16);

  DTO_FIELD(String, i32);
  DTO_FIELD(String, ui32);

  DTO_FIELD(String, i64);
  DTO_FIELD(String, ui64);

  DTO_FIELD(String, f32);
  DTO_FIELD(String, f64);

};

#include OATPP_CODEGEN_END(DTO)

}

void ObjectRemapperTest::onRun() {

  ObjectRemapper remapper;

  {

    OATPP_LOGd(TAG, "Remap. useUnqualifiedFieldNames = false; useUnqualifiedEnumNames = true")

    remapper.objectToTreeConfig().useUnqualifiedFieldNames = false;
    remapper.treeToObjectConfig().useUnqualifiedFieldNames = false;

    remapper.objectToTreeConfig().useUnqualifiedEnumNames = true;
    remapper.treeToObjectConfig().useUnqualifiedEnumNames = true;

    auto obj1 = TestDto1::createShared();
    obj1->field1 = "f1";
    obj1->field2 = "f2";
    obj1->enum1 = TestEnum1::VALUE_1;
    obj1->enum2 = TestEnum1::VALUE_2;

    ErrorStack errorStack;
    auto obj2 = remapper.remap<oatpp::Object<TestDto2>>(obj1, errorStack);
    if(!errorStack.empty()) {
      std::cout << *errorStack.stacktrace() << std::endl;
    }

    OATPP_ASSERT(obj2->field1 == "f1")
    OATPP_ASSERT(obj2->field2 == nullptr)

    OATPP_ASSERT(obj2->enum1 == TestEnum2::VALUE_1)
    OATPP_ASSERT(obj2->enum2 == nullptr)

  }

  {

    OATPP_LOGd(TAG, "Remap. useUnqualifiedFieldNames = true; useUnqualifiedEnumNames = true")

    remapper.objectToTreeConfig().useUnqualifiedFieldNames = true;
    remapper.treeToObjectConfig().useUnqualifiedFieldNames = true;

    remapper.objectToTreeConfig().useUnqualifiedEnumNames = true;
    remapper.treeToObjectConfig().useUnqualifiedEnumNames = true;

    auto obj1 = TestDto1::createShared();
    obj1->field1 = "f1";
    obj1->field2 = "f2";
    obj1->enum1 = TestEnum1::VALUE_1;
    obj1->enum2 = TestEnum1::VALUE_2;

    ErrorStack errorStack;
    auto obj2 = remapper.remap<oatpp::Object<TestDto2>>(obj1, errorStack);
    if(!errorStack.empty()) {
      std::cout << *errorStack.stacktrace() << std::endl;
    }

    OATPP_ASSERT(obj2->field1 == "f1")
    OATPP_ASSERT(obj2->field2 == "f2")

    OATPP_ASSERT(obj2->enum1 == TestEnum2::VALUE_1)
    OATPP_ASSERT(obj2->enum2 == TestEnum2::VALUE_2)

  }

  {
    OATPP_LOGd(TAG, "Remap. Object to Vector")

    remapper.objectToTreeConfig().useUnqualifiedFieldNames = false;
    remapper.treeToObjectConfig().useUnqualifiedFieldNames = false;

    remapper.objectToTreeConfig().useUnqualifiedEnumNames = false;
    remapper.treeToObjectConfig().useUnqualifiedEnumNames = false;

    auto obj1 = TestDto1::createShared();
    obj1->field1 = "f1";
    obj1->field2 = "f2";
    obj1->enum1 = TestEnum1::VALUE_1;
    obj1->enum2 = TestEnum1::VALUE_2;
    obj1->i32 = nullptr;

    ErrorStack errorStack;
    auto vec = remapper.remap<oatpp::Vector<oatpp::String>>(obj1, errorStack);
    if(!errorStack.empty()) {
      std::cout << *errorStack.stacktrace() << std::endl;
    }
    OATPP_ASSERT(vec[0] == "f1")
    OATPP_ASSERT(vec[1] == "f2")
    OATPP_ASSERT(vec[2] == "value-1")
    OATPP_ASSERT(vec[3] == "value-2")
    OATPP_ASSERT(vec[4] == nullptr)
  }

  {

    OATPP_LOGd(TAG, "Remap tree fragments")

    Tree tree;
    tree.setVector(3);

    tree[0]["field_1"] = "val1";
    tree[0]["field_2"] = "val2";

    tree[1]["field_1"] = "val1.2";
    tree[1]["field_2"] = "val2.2";

    tree[2]["field_1"] = "val1.3";
    tree[2]["field_2"] = "val2.3";

    {
      auto map = remapper.remap<oatpp::UnorderedFields<String>>(tree[0]);
      OATPP_ASSERT(map->size() == 2)
      OATPP_ASSERT(map["field_1"] == "val1")
      OATPP_ASSERT(map["field_2"] == "val2")
    }

    {
      auto map = remapper.remap<oatpp::UnorderedFields<String>>(tree[1]);
      OATPP_ASSERT(map->size() == 2)
      OATPP_ASSERT(map["field_1"] == "val1.2")
      OATPP_ASSERT(map["field_2"] == "val2.2")
    }

    {
      auto map = remapper.remap<oatpp::UnorderedFields<String>>(tree[2]);
      OATPP_ASSERT(map->size() == 2)
      OATPP_ASSERT(map["field_1"] == "val1.3")
      OATPP_ASSERT(map["field_2"] == "val2.3")
    }

  }

  {
    oatpp::Tree tree;
    OATPP_ASSERT(tree == nullptr)

    OATPP_ASSERT(tree->isUndefined()) // implicitly initialized
    OATPP_ASSERT(tree != nullptr)

    tree["hello"] = "world";
    std::cout << *tree->debugPrint() << std::endl;

  }

  {

    remapper.treeToObjectConfig().allowLexicalCasting = true;

    auto p = PrimitivesDto::createShared();
    p->b = true;
    p->i8 = -8;
    p->ui8 = 8;
    p->i16 = -16;
    p->ui16 = 16;
    p->i32 = -32;
    p->ui32 = 32;
    p->i64 = -64;
    p->ui64 = 64;
    p->f32 = 0.5f;
    p->f64 = 0.5;

    ErrorStack errorStack;
    auto tp = remapper.remap<oatpp::Object<TextPrimitivesDto>>(p, errorStack);
    if(!errorStack.empty()) {
      OATPP_LOGe(TAG, "stack:\n{}", errorStack.stacktrace())
    }

    OATPP_LOGd(TAG, "f32={}, f64={}", tp->f32, tp->f64)

    OATPP_ASSERT(tp->b == "true")
    OATPP_ASSERT(tp->i8 == "-8")
    OATPP_ASSERT(tp->ui8 == "8")
    OATPP_ASSERT(tp->i16 == "-16")
    OATPP_ASSERT(tp->ui16 == "16")
    OATPP_ASSERT(tp->i32 == "-32")
    OATPP_ASSERT(tp->ui32 == "32")
    OATPP_ASSERT(tp->i64 == "-64")
    OATPP_ASSERT(tp->ui64 == "64")
    OATPP_ASSERT(!tp->f32->empty())
    OATPP_ASSERT(!tp->f64->empty())

  }

  {

    remapper.treeToObjectConfig().allowLexicalCasting = true;

    auto tp = TextPrimitivesDto::createShared();
    tp->b = "true";
    tp->i8 = "-8";
    tp->ui8 = "8";
    tp->i16 = "-16";
    tp->ui16 = "16";
    tp->i32 = "-32";
    tp->ui32 = "32";
    tp->i64 = "-64";
    tp->ui64 = "64";
    tp->f32 = "0.5";
    tp->f64 = "0.5";

    ErrorStack errorStack;
    auto p = remapper.remap<oatpp::Object<PrimitivesDto>>(tp, errorStack);
    if(!errorStack.empty()) {
      OATPP_LOGe(TAG, "stack:\n{}", errorStack.stacktrace())
    }

    OATPP_LOGd(TAG, "f32={}, f64={}", p->f32, p->f64)

    OATPP_ASSERT(p->b == true)
    OATPP_ASSERT(p->i8 == -8)
    OATPP_ASSERT(p->ui8 == 8)
    OATPP_ASSERT(p->i16 == -16)
    OATPP_ASSERT(p->ui16 == 16)
    OATPP_ASSERT(p->i32 == -32)
    OATPP_ASSERT(p->ui32 == 32)
    OATPP_ASSERT(p->i64 == -64)
    OATPP_ASSERT(p->ui64 == 64)
    OATPP_ASSERT(p->f32 > 0.4f && p->f32 < 0.6f)
    OATPP_ASSERT(p->f64 > 0.4  && p->f64 < 0.6 )

  }

}

}}}
