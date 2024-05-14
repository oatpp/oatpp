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

#ifndef oatpp_test_UnitTest_hpp
#define oatpp_test_UnitTest_hpp

#include "oatpp/Environment.hpp"
#include "oatpp/base/Log.hpp"

#include <functional>

namespace oatpp { namespace test {

/**
 * Base class for unit tests.
 */
class UnitTest{
protected:
  const char* const TAG;
public:

  /**
   * Constructor.
   * @param testTAG - tag used for logs.
   */
  UnitTest(const char* testTAG)
    : TAG(testTAG)
  {}

  /**
   * Default virtual destructor.
   */
  virtual ~UnitTest() = default;

  /**
   * Run this test repeatedly for specified number of times.
   * @param times - number of times to run this test.
   */
  void run(v_int32 times);

  /**
   * Run this test.
   */
  void run(){
    run(1);
  }

  /**
   * Override this method. It should contain test logic.
   */
  virtual void onRun() = 0;
  /**
   * Optionally override this method. It should contain logic run before all test iterations.
   */
  virtual void before(){}
  /**
   * Optionally override this method. It should contain logic run after all test iterations.
   */
  virtual void after(){}

  /**
   * Run this test repeatedly for specified number of times.
   * @tparam T - Test class.
   * @param times - number of times to run this test.
   */
  template<class T>
  static void runTest(v_int32 times){
    T test;
    test.run(times);
  }
  
};

#define OATPP_RUN_TEST_0(TEST) \
oatpp::test::UnitTest::runTest<TEST>(1)

#define OATPP_RUN_TEST_1(TEST, N) \
oatpp::test::UnitTest::runTest<TEST>(N)

/**
 * Convenience macro to run test. <br>
 * Usage Example:<br>
 * `OATPP_RUN_TEST(oatpp::test::web::FullTest);`
 * Running the test 10 times:
 * `OATPP_RUN_TEST(oatpp::test::web::FullTest, 10);`
 */
#define OATPP_RUN_TEST(...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_BINARY_SELECTOR(OATPP_RUN_TEST_, (__VA_ARGS__)) (__VA_ARGS__))

}}

#endif /* oatpp_test_UnitTest_hpp */
