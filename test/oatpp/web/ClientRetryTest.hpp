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

#ifndef oatpp_test_web_ClientRetryTest_hpp
#define oatpp_test_web_ClientRetryTest_hpp

#include "oatpp-test/UnitTest.hpp"

namespace oatpp { namespace test { namespace web {

class ClientRetryTest : public UnitTest {
private:
  v_uint16 m_port;
public:

  ClientRetryTest(v_uint16 port)
    : UnitTest("TEST[web::ClientRetryTest]")
    , m_port(port)
  {}

  void onRun() override;

};

}}}

#endif /* oatpp_test_web_ClientRetryTest_hpp */
