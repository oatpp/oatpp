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

#include "oatpp/data/mapping/Tree.hpp"
#include "oatpp/utils/Conversion.hpp"

#include <limits>

namespace oatpp { namespace data { namespace mapping {

namespace {

template<typename T>
void testTreeValue(T value) {

  OATPP_LOGd("TEST", "Test value retrieval for '{}'", Tree::NodePrimitiveType<T>::name)

  Tree node;

  //node.setPrimitive<T>(value);
  node = value;
  auto v = node.getPrimitive<T>();
  OATPP_ASSERT(std::memcmp(&v, &value, sizeof(T)) == 0 && "value check")

  node.setPrimitive<T>(std::numeric_limits<T>::min());
  auto min = node.getPrimitive<T>();
  auto minLim = std::numeric_limits<T>::min();
  OATPP_ASSERT(std::memcmp(&min, &minLim, sizeof(T)) == 0 && "min check")

  node.setPrimitive<T>(std::numeric_limits<T>::max());
  auto max = node.getPrimitive<T>();
  auto maxLim = std::numeric_limits<T>::max();
  OATPP_ASSERT(std::memcmp(&max, &maxLim, sizeof(T)) == 0 && "max check")

}

}

void TreeTest::onRun() {

  testTreeValue<bool>(true);
  testTreeValue<v_int8>(16);
  testTreeValue<v_uint8>(16);
  testTreeValue<v_int16>(16);
  testTreeValue<v_uint16>(16);
  testTreeValue<v_int32>(16);
  testTreeValue<v_uint32>(16);
  testTreeValue<v_int64>(16);
  testTreeValue<v_uint64>(16);
  testTreeValue<v_float32>(16);
  testTreeValue<v_float64>(16);

  {
    OATPP_LOGd(TAG, "Case 1")
    Tree node;
    oatpp::String original = "Hello World!";
    node.setString(original);
    auto stored = node.getString();
    OATPP_ASSERT(stored == original)
    OATPP_ASSERT(stored.get() == original.get())
  }

  {
    OATPP_LOGd(TAG, "Case 2")
    Tree node1;
    Tree node2;

    node1.setString("Hello World!");
    node2 = node1;

    OATPP_ASSERT(node1.getString() == "Hello World!")
    OATPP_ASSERT(node1.getType() == Tree::Type::STRING)

    OATPP_ASSERT(node2.getString() == "Hello World!")
    OATPP_ASSERT(node2.getType() == Tree::Type::STRING)
  }

  {
    OATPP_LOGd(TAG, "Case 3")
    Tree node1;
    Tree node2;

    node1.setString("Hello World!");
    node2 = std::move(node1);

    OATPP_ASSERT(node1.isNull())
    OATPP_ASSERT(node2.getString() == "Hello World!")
    OATPP_ASSERT(node2.getType() == Tree::Type::STRING)
  }

  {
    OATPP_LOGd(TAG, "Case 4")
    std::vector<Tree> originalVector(10);
    for(v_uint32 i = 0; i < 10; i ++) {
      originalVector.at(i).setPrimitive(i);
    }

    Tree node;
    node.setVector(originalVector);

    auto& vector = node.getVector();

    OATPP_ASSERT(vector.size() == originalVector.size())

    for(v_uint32 i = 0; i < originalVector.size(); i ++) {
      OATPP_ASSERT(originalVector.at(i).getPrimitive<v_uint32>() == vector.at(i).getPrimitive<v_uint32>())
    }

    originalVector.resize(5);
    OATPP_ASSERT(vector.size() == 10)

    vector.at(0).setString("Hello");

    OATPP_ASSERT(vector.at(0).getString() == "Hello")
    OATPP_ASSERT(originalVector.at(0).getPrimitive<v_uint32>() == 0)

  }

  {
    OATPP_LOGd(TAG, "Case 5")
    TreeMap originalMap;
    for(v_uint32 i = 0; i < 10; i ++) {
      originalMap["node_" + utils::Conversion::int32ToStr(static_cast<v_int32>(i))].setPrimitive(i);
    }

    Tree node;
    node.setMap(originalMap);

    auto& map = node.getMap();

    OATPP_ASSERT(map.size() == originalMap.size())

    for(v_uint32 i = 0; i < originalMap.size(); i ++) {
      OATPP_ASSERT(originalMap[i].first == map[i].first)
      OATPP_ASSERT(originalMap[i].second.get().getPrimitive<v_uint32>() == map[i].second.get().getPrimitive<v_uint32>())
    }

    originalMap[0].second.get().setPrimitive<v_uint32>(100);
    OATPP_ASSERT(map[0].second.get().getPrimitive<v_uint32>() == 0)
    OATPP_ASSERT(originalMap[0].second.get().getPrimitive<v_uint32>() == 100)

  }

  {
    OATPP_LOGd(TAG, "Case 6")
    Tree article;
    oatpp::Tree ot;


    article["name"] = "Hello World!";
    article["pages"] = 96;

    article["references"].setVector(2);
    article["references"][0]["author"] = "Alexander";
    article["references"][1]["author"] = "Leonid";

    v_int32 value = article["pages"];
    oatpp::String author = article["references"][0]["author"];

    OATPP_LOGd(TAG, "pages={}', refs='{}', node_type={}", value, author, static_cast<v_int32>(article.getType()))

  }

  {

    OATPP_LOGd(TAG, "Attributes Case 1")
    OATPP_LOGd(TAG, "size of Tree::Attributes='{}'", sizeof(Tree::Attributes))

    Tree::Attributes attr;

    attr["key1"] = "value1";
    attr["key1"] = "value1.2";
    attr["key2"] = "value2";

    OATPP_ASSERT(attr.size() == 2)

    OATPP_ASSERT(attr["key1"] == "value1.2")
    OATPP_ASSERT(attr["key2"] == "value2")
    OATPP_ASSERT(attr["key3"] == nullptr) // key3 added

    OATPP_ASSERT(attr.size() == 3)

    OATPP_ASSERT(attr[0].second.get() == "value1.2")
    OATPP_ASSERT(attr[1].second.get() == "value2")
    OATPP_ASSERT(attr[2].second.get() == nullptr)

  }

  {

    OATPP_LOGd(TAG, "Attributes Case 2")

    Tree::Attributes attr1;
    Tree::Attributes attr2;

    attr1["key1"] = "value1";
    attr1["key2"] = "value2";
    attr1["key3"] = nullptr;

    attr2["key1"] = "v1";
    attr2["key2"] = "v2";

    attr2 = attr1;

    attr1["key1"] = "1";
    attr1["key2"] = "2";
    attr1["key3"] = "3";

    OATPP_ASSERT(attr1[0].second.get() == "1")
    OATPP_ASSERT(attr1[1].second.get() == "2")
    OATPP_ASSERT(attr1[2].second.get() == "3")

    OATPP_ASSERT(attr1["key1"] == "1")
    OATPP_ASSERT(attr1["key2"] == "2")
    OATPP_ASSERT(attr1["key3"] == "3")

    OATPP_ASSERT(attr2[0].second.get() == "value1")
    OATPP_ASSERT(attr2[1].second.get() == "value2")
    OATPP_ASSERT(attr2[2].second.get() == nullptr)

    OATPP_ASSERT(attr2["key1"] == "value1")
    OATPP_ASSERT(attr2["key2"] == "value2")
    OATPP_ASSERT(attr2["key3"] == nullptr)

    Tree::Attributes attr3;
    attr2 = attr3;

    OATPP_ASSERT(attr2.empty())

  }

  {

    OATPP_LOGd(TAG, "Attributes Case 3")

    Tree tree1;
    Tree tree2;

    tree1 = "hello";
    tree2 = "world";

    tree1.attributes()["key1"] = "value1";
    tree1.attributes()["key2"] = "value2";
    tree1.attributes()["key3"] = nullptr;

    tree2.attributes()["key1"] = "v1";
    tree2.attributes()["key2"] = "v2";

    tree2 = tree1;

    tree1.attributes()["key1"] = "1";
    tree1.attributes()["key2"] = "2";
    tree1.attributes()["key3"] = "3";

    OATPP_ASSERT(tree1.attributes()[0].second.get() == "1")
    OATPP_ASSERT(tree1.attributes()[1].second.get() == "2")
    OATPP_ASSERT(tree1.attributes()[2].second.get() == "3")

    OATPP_ASSERT(tree1.attributes()["key1"] == "1")
    OATPP_ASSERT(tree1.attributes()["key2"] == "2")
    OATPP_ASSERT(tree1.attributes()["key3"] == "3")

    OATPP_ASSERT(tree2.attributes()[0].second.get() == "value1")
    OATPP_ASSERT(tree2.attributes()[1].second.get() == "value2")
    OATPP_ASSERT(tree2.attributes()[2].second.get() == nullptr)

    OATPP_ASSERT(tree2.attributes()["key1"] == "value1")
    OATPP_ASSERT(tree2.attributes()["key2"] == "value2")
    OATPP_ASSERT(tree2.attributes()["key3"] == nullptr)

    Tree tree3;
    tree2 = tree3;

    OATPP_ASSERT(tree2.attributes().empty())

  }

}

}}}
