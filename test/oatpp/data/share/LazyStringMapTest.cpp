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

#include "LazyStringMapTest.hpp"

#include "oatpp/data/share/LazyStringMap.hpp"
#include "oatpp/Types.hpp"

namespace oatpp { namespace data { namespace share {

void LazyStringMapTest::onRun() {

  const char* text = "Hello World!";

  {

    LazyStringMap<StringKeyLabel> map;

    map.put("key1", StringKeyLabel(nullptr, text, 5));
    map.put("key2", StringKeyLabel(nullptr, text + 6, 6));

    oatpp::String s1 = map.get("key1");
    oatpp::String s2 = map.get("key2");

    OATPP_ASSERT(s1 == "Hello")
    OATPP_ASSERT(s2 == "World!")

    oatpp::String s12 = map.get("key1");
    oatpp::String s22 = map.get("key2");

    OATPP_ASSERT(s1.get() == s12.get())
    OATPP_ASSERT(s2.get() == s22.get())

    OATPP_ASSERT(map.get("KEY1") == nullptr)
    OATPP_ASSERT(map.get("KEY2") == nullptr)

    auto all = map.getAll();

    auto s13 = all["key1"];
    auto s23 = all["key2"];

    OATPP_ASSERT(s13.getData() == s1->data() && s13.getSize() == static_cast<v_buff_size>(s1->size()))
    OATPP_ASSERT(s23.getData() == s2->data() && s23.getSize() == static_cast<v_buff_size>(s2->size()))
    OATPP_ASSERT(s1.get() == s13.getMemoryHandle().get())
    OATPP_ASSERT(s2.get() == s23.getMemoryHandle().get())

    OATPP_ASSERT(map.getSize() == 2)

  }

  {

    LazyStringMap<StringKeyLabelCI> map;

    map.put("key1", StringKeyLabel(nullptr, text, 5));
    map.put("key2", StringKeyLabel(nullptr, text + 6, 6));

    auto s01 = map.getAsMemoryLabel_Unsafe<StringKeyLabel>("key1");
    auto s02 = map.getAsMemoryLabel_Unsafe<StringKeyLabel>("key2");

    OATPP_ASSERT(s01 == "Hello")
    OATPP_ASSERT(s02 == "World!")

    OATPP_ASSERT(s01.getMemoryHandle() == nullptr)
    OATPP_ASSERT(s02.getMemoryHandle() == nullptr)

    auto s1 = map.getAsMemoryLabel<StringKeyLabel>("key1");
    auto s2 = map.getAsMemoryLabel<StringKeyLabel>("key2");

    OATPP_ASSERT(s1 == "Hello")
    OATPP_ASSERT(s2 == "World!")

    oatpp::String s12 = map.get("key1");
    oatpp::String s22 = map.get("key2");

    OATPP_ASSERT(s1.getMemoryHandle().get() == s12.get())
    OATPP_ASSERT(s2.getMemoryHandle().get() == s22.get())

    OATPP_ASSERT(map.getAsMemoryLabel<StringKeyLabel>("KEY1") == s1)
    OATPP_ASSERT(map.getAsMemoryLabel<StringKeyLabel>("KEY2") == s2)

  }

  {

    LazyStringMap<StringKeyLabelCI> map1;
    LazyStringMap<StringKeyLabelCI> map2;

    map1.put("key1", StringKeyLabel(nullptr, text, 5));
    map1.put("key2", StringKeyLabel(nullptr, text + 6, 6));

    OATPP_ASSERT(map1.getSize() == 2)
    OATPP_ASSERT(map2.getSize() == 0)

    map2 = std::move(map1);

    OATPP_ASSERT(map1.getSize() == 0)
    OATPP_ASSERT(map2.getSize() == 2)

    {

      auto all = map2.getAll_Unsafe();

      auto s1 = all["key1"];
      auto s2 = all["key2"];

      OATPP_ASSERT(s1.getMemoryHandle() == nullptr)
      OATPP_ASSERT(s2.getMemoryHandle() == nullptr)

      OATPP_ASSERT(s1 == "Hello")
      OATPP_ASSERT(s2 == "World!")

    }

    {

      auto all = map2.getAll();

      auto s1 = all["key1"];
      auto s2 = all["key2"];

      OATPP_ASSERT(s1.getMemoryHandle())
      OATPP_ASSERT(s2.getMemoryHandle())

      OATPP_ASSERT(s1 == "Hello")
      OATPP_ASSERT(s2 == "World!")

      auto s12 = map2.get("key1");
      auto s22 = map2.get("key2");

      OATPP_ASSERT(s1.getMemoryHandle().get() == s12.get())
      OATPP_ASSERT(s2.getMemoryHandle().get() == s22.get())

    }

  }


}

}}}
