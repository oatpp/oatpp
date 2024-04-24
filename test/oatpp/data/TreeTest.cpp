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

#include "TreeTest.hpp"

#include "oatpp/data/Tree.hpp"
#include "oatpp/utils/Conversion.hpp"

#include <limits>

namespace oatpp { namespace data {

namespace {

template<typename T>
void testNodeValue(T value) {

  Tree::Node node;

  node.setValue<T>(value);
  auto v = node.getValue<T>();
  OATPP_ASSERT(v == value && "value check")

  node.setValue<T>(std::numeric_limits<T>::min());
  auto min = node.getValue<T>();
  OATPP_ASSERT(min == std::numeric_limits<T>::min() && "min check")

  node.setValue<T>(std::numeric_limits<T>::max());
  auto max = node.getValue<T>();
  OATPP_ASSERT(max == std::numeric_limits<T>::max() && "max check")

}

}

void TreeTest::onRun() {

  testNodeValue<bool>(true);
  testNodeValue<v_int8>(16);
  testNodeValue<v_uint8>(16);
  testNodeValue<v_int16>(16);
  testNodeValue<v_uint16>(16);
  testNodeValue<v_int32>(16);
  testNodeValue<v_uint32>(16);
  testNodeValue<v_int64>(16);
  testNodeValue<v_uint64>(16);
  testNodeValue<v_float32>(16);
  testNodeValue<v_float64>(16);

  {
    Tree::Node node;
    oatpp::String original = "Hello World!";
    node.setString(original);
    auto stored = node.getString();
    OATPP_ASSERT(stored == original)
    OATPP_ASSERT(stored.get() == original.get())
  }

  {
    Tree::Node node1;
    Tree::Node node2;

    node1.setString("Hello World!");
    node2 = node1;

    OATPP_ASSERT(node1.getString() == "Hello World!")
    OATPP_ASSERT(node1.getType() == Tree::Node::Type::STRING)

    OATPP_ASSERT(node2.getString() == "Hello World!")
    OATPP_ASSERT(node2.getType() == Tree::Node::Type::STRING)
  }

  {
    Tree::Node node1;
    Tree::Node node2;

    node1.setString("Hello World!");
    node2 = std::move(node1);

    OATPP_ASSERT(node1.isNull())
    OATPP_ASSERT(node2.getString() == "Hello World!")
    OATPP_ASSERT(node2.getType() == Tree::Node::Type::STRING)
  }

  {
    std::vector<Tree::Node> originalVector(10);
    for(v_uint32 i = 0; i < 10; i ++) {
      originalVector.at(i).setValue(i);
    }

    Tree::Node node;
    node.setVector(originalVector);

    auto& vector = node.getVector();

    OATPP_ASSERT(vector.size() == originalVector.size())

    for(v_uint32 i = 0; i < originalVector.size(); i ++) {
      OATPP_ASSERT(originalVector.at(i).getValue<v_uint32>() == vector.at(i).getValue<v_uint32>())
    }

    originalVector.resize(5);
    OATPP_ASSERT(vector.size() == 10)

    vector.at(0).setString("Hello");

    OATPP_ASSERT(vector.at(0).getString() == "Hello")
    OATPP_ASSERT(originalVector.at(0).getValue<v_uint32>() == 0)

  }

  {
    std::vector<std::pair<oatpp::String, Tree::Node>> originalMap(10);
    for(v_uint32 i = 0; i < 10; i ++) {
      originalMap.at(i).first = "node_" + utils::Conversion::int32ToStr(i);
      originalMap.at(i).second.setValue(i);
    }

    Tree::Node node;
    node.setMap(originalMap);

    auto& map = node.getMap();

    OATPP_ASSERT(map.size() == originalMap.size())

    for(v_uint32 i = 0; i < originalMap.size(); i ++) {
      OATPP_ASSERT(originalMap.at(i).first = map.at(i).first)
      OATPP_ASSERT(originalMap.at(i).second.getValue<v_uint32>() == map.at(i).second.getValue<v_uint32>())
    }

    originalMap.resize(5);
    OATPP_ASSERT(map.size() == 10)

  }

}

}}
