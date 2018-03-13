/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#ifndef __testbase__UnitTest__
#define __testbase__UnitTest__

#include "../src/base/Environment.hpp"

namespace oatpp { namespace test {
  
class UnitTest{
protected:
  const char* const TAG;
public:

  UnitTest(const char* testTAG)
    : TAG(testTAG)
  {}
  
  virtual ~UnitTest(){
  }
  
  v_int64 getTickCount();
  
  bool run(v_int32 times);

  bool run(){
    return run(1);
  }
  
  virtual bool onRun() = 0;
  
  template<class T>
  static void runTest(v_int32 times){
    T test;
    test.run(times);
  }
  
};
  
#define OATPP_RUN_TEST(TEST) oatpp::test::UnitTest::runTest<TEST>(1)
  
}}

#endif /* defined(__testbase__UnitTest__) */
