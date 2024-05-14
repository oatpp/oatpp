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

}

}}}
