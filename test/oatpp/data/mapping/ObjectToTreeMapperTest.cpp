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

#include "ObjectToTreeMapperTest.hpp"

#include "oatpp/data/mapping/ObjectToTreeMapper.hpp"

#include "oatpp/macro/codegen.hpp"

#include <iostream>

namespace oatpp { namespace data { namespace mapping {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(TestEnum, v_int32,
  VALUE(VALUE_1, 1, "value-1"),
  VALUE(VALUE_2, 2, "value-2")
)

class TestDto1 : public oatpp::DTO {

  DTO_INIT(TestDto1, DTO)

  DTO_FIELD(String, str, "str-q");

  DTO_FIELD(Int8, i8, "i8-q");
  DTO_FIELD(UInt8, ui8, "ui8-q");

  DTO_FIELD(Int16, i16, "i16-q");
  DTO_FIELD(UInt16, ui16, "ui16-q");

  DTO_FIELD(Int32, i32, "i32-q");
  DTO_FIELD(UInt32, ui32, "ui32-q");

  DTO_FIELD(Int64, i64, "i64-q");
  DTO_FIELD(UInt64, ui64, "ui64-q");

  DTO_FIELD(Vector<oatpp::Object<TestDto1>>, vector, "vector-q");
  DTO_FIELD(UnorderedFields<oatpp::Object<TestDto1>>, map, "map-q");
  DTO_FIELD(Fields<String>, pairs, "pairs-q");

};

#include OATPP_CODEGEN_END(DTO)

}

void ObjectToTreeMapperTest::onRun() {

  ObjectToTreeMapper mapper;
  ObjectToTreeMapper::Config config;

  {
    OATPP_LOGd(TAG, "Map String")
    Tree tree;
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &config;

    oatpp::String str = oatpp::String("Hello World");

    mapper.map(state, str);
    OATPP_ASSERT(state.errorStack.empty())

    OATPP_ASSERT(tree.getType() == Tree::Type::STRING)
    OATPP_ASSERT(tree.getString() == str)
    OATPP_ASSERT(tree.getString().get() == str.get())
  }

  {
    OATPP_LOGd(TAG, "Map String in Any")
    Tree tree;
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &config;

    oatpp::String str = oatpp::String("Hello World");
    oatpp::Any any = str;

    mapper.map(state, any);
    OATPP_ASSERT(state.errorStack.empty())

    OATPP_ASSERT(tree.getType() == Tree::Type::STRING)
    OATPP_ASSERT(tree.getString() == str)
    OATPP_ASSERT(tree.getString().get() == str.get())
  }

  {
    OATPP_LOGd(TAG, "Map Object qualified")
    config.useUnqualifiedFieldNames = false;

    Tree tree;
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &config;

    auto obj = TestDto1::createShared();

    obj->str = "hello object";

    obj->i8 = -8;
    obj->ui8 = 8;
    obj->i16 = -16;
    obj->ui16 = 16;
    obj->i32 = -32;
    obj->ui32 = 32;
    obj->i64 = -64;
    obj->ui64 = 64;

    obj->vector = {TestDto1::createShared(), TestDto1::createShared(), TestDto1::createShared()};
    obj->map = {{"key1", TestDto1::createShared()}, {"key2", TestDto1::createShared()}, {"key3", TestDto1::createShared()}};
    obj->pairs = {{"same-key", "value1"}, {"same-key", "value2"}, {"same-key", "value3"}};

    obj->vector[0]->str = "vec-item-1";
    obj->vector[1]->str = "vec-item-2";
    obj->vector[2]->str = "vec-item-3";

    obj->map["key1"]->i64 = 1;
    obj->map["key2"]->i64 = 2;
    obj->map["key3"]->i64 = 3;

    mapper.map(state, obj);

    OATPP_ASSERT(state.errorStack.empty())

    std::cout << *tree.debugPrint() << std::endl;

    OATPP_ASSERT(tree.getType() == Tree::Type::MAP)

    OATPP_ASSERT(tree["str-q"].getString() == "hello object")
    OATPP_ASSERT(tree["i8-q"].getPrimitive<v_int8>() == -8)
    OATPP_ASSERT(tree["ui8-q"].getPrimitive<v_uint8>() == 8)
    OATPP_ASSERT(tree["i16-q"].getPrimitive<v_int16>() == -16)
    OATPP_ASSERT(tree["ui16-q"].getPrimitive<v_uint16>() == 16)
    OATPP_ASSERT(tree["i32-q"].getPrimitive<v_int32>() == -32)
    OATPP_ASSERT(tree["ui32-q"].getPrimitive<v_uint32>() == 32)
    OATPP_ASSERT(tree["i64-q"].getPrimitive<v_int64>() == -64)
    OATPP_ASSERT(tree["ui64-q"].getPrimitive<v_uint64>() == 64)

    OATPP_ASSERT(tree["vector-q"].getVector().size() == 3)
    OATPP_ASSERT(tree["vector-q"][0]["str-q"].getString() == "vec-item-1")
    OATPP_ASSERT(tree["vector-q"][1]["str-q"].getString() == "vec-item-2")
    OATPP_ASSERT(tree["vector-q"][2]["str-q"].getString() == "vec-item-3")

    OATPP_ASSERT(tree["map-q"].getMap().size() == 3)
    OATPP_ASSERT(tree["map-q"]["key1"]["i64-q"].getPrimitive<v_int64>() == 1)
    OATPP_ASSERT(tree["map-q"]["key2"]["i64-q"].getPrimitive<v_int64>() == 2)
    OATPP_ASSERT(tree["map-q"]["key3"]["i64-q"].getPrimitive<v_int64>() == 3)

    auto& pairs = tree["pairs-q"].getPairs();
    OATPP_ASSERT(pairs.size() == 3)
    OATPP_ASSERT(pairs[0].first == "same-key" && pairs[0].second.getString() == "value1")
    OATPP_ASSERT(pairs[1].first == "same-key" && pairs[1].second.getString() == "value2")
    OATPP_ASSERT(pairs[2].first == "same-key" && pairs[2].second.getString() == "value3")

  }

  {
    OATPP_LOGd(TAG, "Map Object unqualified")
    config.useUnqualifiedFieldNames = true;

    Tree tree;
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &config;

    auto obj = TestDto1::createShared();

    obj->str = "hello object";

    obj->i8 = -8;
    obj->ui8 = 8;
    obj->i16 = -16;
    obj->ui16 = 16;
    obj->i32 = -32;
    obj->ui32 = 32;
    obj->i64 = -64;
    obj->ui64 = 64;

    obj->vector = {TestDto1::createShared(), TestDto1::createShared(), TestDto1::createShared()};
    obj->map = {{"key1", TestDto1::createShared()}, {"key2", TestDto1::createShared()}, {"key3", TestDto1::createShared()}};
    obj->pairs = {{"same-key", "value1"}, {"same-key", "value2"}, {"same-key", "value3"}};

    obj->vector[0]->str = "vec-item-1";
    obj->vector[1]->str = "vec-item-2";
    obj->vector[2]->str = "vec-item-3";

    obj->map["key1"]->i64 = 1;
    obj->map["key2"]->i64 = 2;
    obj->map["key3"]->i64 = 3;

    mapper.map(state, obj);

    OATPP_ASSERT(state.errorStack.empty())

    std::cout << *tree.debugPrint() << std::endl;

    OATPP_ASSERT(tree.getType() == Tree::Type::MAP)

    OATPP_ASSERT(tree["str"].getString() == "hello object")
    OATPP_ASSERT(tree["i8"].getPrimitive<v_int8>() == -8)
    OATPP_ASSERT(tree["ui8"].getPrimitive<v_uint8>() == 8)
    OATPP_ASSERT(tree["i16"].getPrimitive<v_int16>() == -16)
    OATPP_ASSERT(tree["ui16"].getPrimitive<v_uint16>() == 16)
    OATPP_ASSERT(tree["i32"].getPrimitive<v_int32>() == -32)
    OATPP_ASSERT(tree["ui32"].getPrimitive<v_uint32>() == 32)
    OATPP_ASSERT(tree["i64"].getPrimitive<v_int64>() == -64)
    OATPP_ASSERT(tree["ui64"].getPrimitive<v_uint64>() == 64)

    OATPP_ASSERT(tree["vector"].getVector().size() == 3)
    OATPP_ASSERT(tree["vector"][0]["str"].getString() == "vec-item-1")
    OATPP_ASSERT(tree["vector"][1]["str"].getString() == "vec-item-2")
    OATPP_ASSERT(tree["vector"][2]["str"].getString() == "vec-item-3")

    OATPP_ASSERT(tree["map"].getMap().size() == 3)
    OATPP_ASSERT(tree["map"]["key1"]["i64"].getPrimitive<v_int64>() == 1)
    OATPP_ASSERT(tree["map"]["key2"]["i64"].getPrimitive<v_int64>() == 2)
    OATPP_ASSERT(tree["map"]["key3"]["i64"].getPrimitive<v_int64>() == 3)

    auto& pairs = tree["pairs"].getPairs();
    OATPP_ASSERT(pairs.size() == 3)
    OATPP_ASSERT(pairs[0].first == "same-key" && pairs[0].second.getString() == "value1")
    OATPP_ASSERT(pairs[1].first == "same-key" && pairs[1].second.getString() == "value2")
    OATPP_ASSERT(pairs[2].first == "same-key" && pairs[2].second.getString() == "value3")

  }

  {
    OATPP_LOGd(TAG, "Map Enum qualified")
    config.useUnqualifiedEnumNames = false;

    Tree tree;
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &config;

    oatpp::Enum<TestEnum> enumObj(TestEnum::VALUE_1);

    mapper.map(state, enumObj);

    std::cout << *tree.debugPrint() << std::endl;

    OATPP_ASSERT(tree.getString() == "value-1");
  }

  {
    OATPP_LOGd(TAG, "Map Enum Unqualified")
    config.useUnqualifiedEnumNames = true;

    Tree tree;
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &config;

    oatpp::Enum<TestEnum> enumObj = oatpp::Enum<TestEnum>::getEntryByUnqualifiedName("VALUE_2").value;

    mapper.map(state, enumObj);

    std::cout << *tree.debugPrint() << std::endl;

    OATPP_ASSERT(tree.getString() == "VALUE_2");
  }

}

}}}
