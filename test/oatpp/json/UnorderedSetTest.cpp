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

#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace json {

void UnorderedSetTest::onRun() {

  oatpp::json::ObjectMapper mapper;

  {
    oatpp::UnorderedSet<oatpp::String> set = {"Hello", "World", "!"};
    auto json = mapper.writeToString(set);
    OATPP_LOGd(TAG, "json='{}'", json->c_str())
  }

  {
    oatpp::String json = "[\"Hello\",\"World\",\"!\",\"Hello\",\"World\",\"!\"]";
    auto set = mapper.readFromString<oatpp::UnorderedSet<oatpp::String>>(json);
    OATPP_ASSERT(set)
    OATPP_ASSERT(set->size() == 3)
    for(auto& item : *set) {
      OATPP_LOGd(TAG, "item='{}'", item->c_str())
    }
  }
  
}

}}
