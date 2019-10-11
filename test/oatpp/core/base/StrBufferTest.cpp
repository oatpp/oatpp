/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
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

#include "StrBufferTest.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp-test/Checker.hpp"
#include <list>

namespace oatpp { namespace test { namespace base  {

void StrBufferTest::onRun() {


  oatpp::String base = "0123456789";


  {
    oatpp::String substr = base->substr(3);
    OATPP_ASSERT(substr->equals("3456789"))
  }

  {
    oatpp::String substr = base->substr(3,3);
    OATPP_ASSERT(substr->equals("345"))
  }

  {
    oatpp::String substr = base->substr(3,9);
    OATPP_ASSERT(substr->equals("3456789"))
  }

  {
    oatpp::String substr = base->substr(10);
    OATPP_ASSERT(substr->getSize() == 0)
  }

  {
    oatpp::String substr = base->substr(9);
    OATPP_ASSERT(substr == "9")
  }

  {
    oatpp::String substr = base->substr(-1);
    OATPP_ASSERT(substr->getSize() == 0)
  }

  {
    oatpp::String substr = base->substr(3, -1);
    OATPP_ASSERT(substr->getSize() == 0)
  }
}

}}}
