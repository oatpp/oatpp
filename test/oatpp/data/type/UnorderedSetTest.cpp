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

#include "UnorderedSetTest.hpp"

#include "oatpp/Types.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace data { namespace  type {

void UnorderedSetTest::onRun() {


  {
    OATPP_LOGi(TAG, "test default constructor...")
    oatpp::UnorderedSet<oatpp::String> set;

    OATPP_ASSERT(!set)
    OATPP_ASSERT(set == nullptr)

    OATPP_ASSERT(set.get() == nullptr)
    OATPP_ASSERT(set.getValueType()->classId.id == oatpp::data::type::__class::AbstractUnorderedSet::CLASS_ID.id)
    OATPP_ASSERT(set.getValueType()->params.size() == 1)
    OATPP_ASSERT(set.getValueType()->params.front() == oatpp::String::Class::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test empty ilist constructor...")
    oatpp::UnorderedSet<oatpp::String> set({});

    OATPP_ASSERT(set)
    OATPP_ASSERT(set != nullptr)
    OATPP_ASSERT(set->size() == 0)

    OATPP_ASSERT(set.get() != nullptr)
    OATPP_ASSERT(set.getValueType()->classId.id == oatpp::data::type::__class::AbstractUnorderedSet::CLASS_ID.id)
    OATPP_ASSERT(set.getValueType()->params.size() == 1)
    OATPP_ASSERT(set.getValueType()->params.front() == oatpp::String::Class::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test createShared()...")
    oatpp::UnorderedSet<oatpp::String> set = oatpp::UnorderedSet<oatpp::String>::createShared();

    OATPP_ASSERT(set)
    OATPP_ASSERT(set != nullptr)
    OATPP_ASSERT(set->size() == 0)

    OATPP_ASSERT(set.get() != nullptr)
    OATPP_ASSERT(set.getValueType()->classId.id == oatpp::data::type::__class::AbstractUnorderedSet::CLASS_ID.id)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test copy-assignment operator...")
    oatpp::UnorderedSet<oatpp::String> set1({});
    oatpp::UnorderedSet<oatpp::String> set2;

    set2 = set1;

    OATPP_ASSERT(set1)
    OATPP_ASSERT(set2)

    OATPP_ASSERT(set1->size() == 0)
    OATPP_ASSERT(set2->size() == 0)

    OATPP_ASSERT(set1.get() == set2.get())

    set2->insert("a");

    OATPP_ASSERT(set1->size() == 1)
    OATPP_ASSERT(set2->size() == 1)

    set2 = {"b", "c"};

    OATPP_ASSERT(set1->size() == 1)
    OATPP_ASSERT(set2->size() == 2)

    OATPP_ASSERT(set2["b"] == true)
    OATPP_ASSERT(set2["c"] == true)

    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test move-assignment operator...")
    oatpp::UnorderedSet<oatpp::String> set1({});
    oatpp::UnorderedSet<oatpp::String> set2;

    set2 = std::move(set1);

    OATPP_ASSERT(!set1)
    OATPP_ASSERT(set2)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "test polymorphicDispatcher...")
    oatpp::UnorderedSet<oatpp::String> set = {"a", "b", "c"};

    auto polymorphicDispatcher = static_cast<const typename oatpp::data::type::__class::Collection::PolymorphicDispatcher*>(
      set.getValueType()->polymorphicDispatcher
    );

    polymorphicDispatcher->addItem(set, oatpp::String("a"));
    polymorphicDispatcher->addItem(set, oatpp::String("b"));
    polymorphicDispatcher->addItem(set, oatpp::String("c"));

    polymorphicDispatcher->addItem(set, oatpp::String("d"));

    OATPP_ASSERT(set->size() == 4)

    OATPP_ASSERT(set["a"])
    OATPP_ASSERT(set["b"])
    OATPP_ASSERT(set["c"])
    OATPP_ASSERT(set["d"])
    OATPP_LOGi(TAG, "OK")
  }

}

}}}
