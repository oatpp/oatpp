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

#include "TreeToObjectMapperTest.hpp"

#include "oatpp/json/ObjectMapper.hpp"

#include "oatpp/data/mapping/ObjectToTreeMapper.hpp"
#include "oatpp/data/mapping/TreeToObjectMapper.hpp"

#include "oatpp/macro/codegen.hpp"

#include <iostream>

namespace oatpp { namespace data { namespace mapping {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class TestDto1 : public oatpp::DTO {

  DTO_INIT(TestDto1, DTO)

  DTO_FIELD(String, str);

  DTO_FIELD(Int8, i8);
  DTO_FIELD(UInt8, ui8);

  DTO_FIELD(Int16, i16);
  DTO_FIELD(UInt16, ui16);

  DTO_FIELD(Int32, i32);
  DTO_FIELD(UInt32, ui32);

  DTO_FIELD(Int64, i64);
  DTO_FIELD(UInt64, ui64);

  DTO_FIELD(Vector<oatpp::Object<TestDto1>>, vector);
  DTO_FIELD(Fields<oatpp::Object<TestDto1>>, map);

};

#include OATPP_CODEGEN_END(DTO)

}

void TreeToObjectMapperTest::onRun() {

  json::ObjectMapper jsonMapper;
  jsonMapper.getSerializer()->getConfig()->useBeautifier = true;
  jsonMapper.getSerializer()->getConfig()->includeNullFields = false;

  TreeToObjectMapper mapper;
  TreeToObjectMapper::Config config;

  ObjectToTreeMapper reverseMapper;
  ObjectToTreeMapper::Config reverseConfig;

  {
    Tree tree;

    tree["str"] = "Hello World!";
    tree["i8"] = -8;
    tree["ui8"] = 8;
    tree["i16"] = -16;
    tree["ui16"] = 16;
    tree["i32"] = -32;
    tree["ui32"] = 32;
    tree["i64"] = -64;
    tree["ui64"] = 64;

    tree["vector"].setVector(3);
    tree["vector"][0]["str"] = "nested_1 (in vector)";
    tree["vector"][1]["str"] = "nested_2 (in vector)";
    tree["vector"][2]["str"] = "nested_3 (in vector)";

    tree["map"]["nested_1"]["i32"] = 1;
    tree["map"]["nested_2"]["i32"] = 2;
    tree["map"]["nested_3"]["i32"] = 3;

    TreeToObjectMapper::MappingState state;
    state.config = &config;
    state.tree = &tree;

    const auto& obj = mapper.map(state,oatpp::Object<TestDto1>::Class::getType());

    if(state.errorStack.empty()) {
      auto json = jsonMapper.writeToString(obj);
      std::cout << *json << std::endl;
    } else {
      auto err = state.errorStacktrace();
      std::cout << *err << std::endl;
    }

    Tree clonedTree;

    ObjectToTreeMapper::MappingState reverseState;
    reverseState.config = &reverseConfig;
    reverseState.tree = &clonedTree;

    reverseMapper.map(reverseState, obj);

    auto debugStr = clonedTree.debugPrint();

    std::cout << *debugStr << std::endl;

  }

}

}}}
