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

#include "ListTest.hpp"

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

void ListTest::onRun() {

  {
    OATPP_LOGI(TAG, "test default constructor...");
    oatpp::List<oatpp::String> list;

    OATPP_ASSERT(!list);
    OATPP_ASSERT(list == nullptr);

    OATPP_ASSERT(list.get() == nullptr);
    OATPP_ASSERT(list.getValueType()->classId.id == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID.id);
    OATPP_ASSERT(list.getValueType()->params.size() == 1);
    OATPP_ASSERT(list.getValueType()->params.front() == oatpp::String::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test empty ilist constructor...");
    oatpp::List<oatpp::String> list({});

    OATPP_ASSERT(list);
    OATPP_ASSERT(list != nullptr);
    OATPP_ASSERT(list->size() == 0);

    OATPP_ASSERT(list.get() != nullptr);
    OATPP_ASSERT(list.getValueType()->classId.id == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID.id);
    OATPP_ASSERT(list.getValueType()->params.size() == 1);
    OATPP_ASSERT(list.getValueType()->params.front() == oatpp::String::Class::getType());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test createShared()...");
    oatpp::List<oatpp::String> list = oatpp::List<oatpp::String>::createShared();

    OATPP_ASSERT(list);
    OATPP_ASSERT(list != nullptr);
    OATPP_ASSERT(list->size() == 0);

    OATPP_ASSERT(list.get() != nullptr);
    OATPP_ASSERT(list.getValueType()->classId.id == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID.id);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test copy-assignment operator...");
    oatpp::List<oatpp::String> list1({});
    oatpp::List<oatpp::String> list2;

    list2 = list1;

    OATPP_ASSERT(list1);
    OATPP_ASSERT(list2);

    OATPP_ASSERT(list1->size() == 0);
    OATPP_ASSERT(list2->size() == 0);

    OATPP_ASSERT(list1.get() == list2.get());

    list2->push_back("a");

    OATPP_ASSERT(list1->size() == 1);
    OATPP_ASSERT(list2->size() == 1);

    list2 = {"b", "c"};

    OATPP_ASSERT(list1->size() == 1);
    OATPP_ASSERT(list2->size() == 2);

    OATPP_ASSERT(list2[0] == "b");
    OATPP_ASSERT(list2[1] == "c");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test move-assignment operator...");
    oatpp::List<oatpp::String> list1({});
    oatpp::List<oatpp::String> list2;

    list2 = std::move(list1);

    OATPP_ASSERT(!list1);
    OATPP_ASSERT(list2);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test get element by index...");
    oatpp::List<oatpp::String> list = {"a", "b", "c"};

    OATPP_ASSERT(list);
    OATPP_ASSERT(list != nullptr);
    OATPP_ASSERT(list->size() == 3);

    OATPP_ASSERT(list[0] == "a");
    OATPP_ASSERT(list[1] == "b");
    OATPP_ASSERT(list[2] == "c");

    list[1] = "Hello!";

    OATPP_ASSERT(list->size() == 3);

    OATPP_ASSERT(list[0] == "a");
    OATPP_ASSERT(list[1] == "Hello!");
    OATPP_ASSERT(list[2] == "c");
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "test polymorphicDispatcher...");
    oatpp::List<oatpp::String> list = {"a", "b", "c"};

    auto polymorphicDispatcher = static_cast<const oatpp::data::mapping::type::__class::Collection::PolymorphicDispatcher*>(
      list.getValueType()->polymorphicDispatcher
    );

    polymorphicDispatcher->addItem(list, oatpp::String("d"));

    OATPP_ASSERT(list->size() == 4);

    OATPP_ASSERT(list[0] == "a");
    OATPP_ASSERT(list[1] == "b");
    OATPP_ASSERT(list[2] == "c");
    OATPP_ASSERT(list[3] == "d");
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}
