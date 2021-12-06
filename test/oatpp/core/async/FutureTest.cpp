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

#include "FutureTest.hpp"

#include "oatpp/core/async/Future.hpp"
#include "oatpp/core/async/Executor.hpp"

#include <thread>

namespace oatpp { namespace test { namespace async {

namespace {

static std::atomic<v_int64> TestCoroutine1_Counter {0};
static std::atomic<v_int64> TestCoroutine2_Counter {0};
static std::atomic<v_int64> TestCoroutine3_Counter {0};

class TestCoroutine1 : public oatpp::async::Coroutine<TestCoroutine1> {
public:

  Action act() override {
    oatpp::async::Promise<oatpp::String> p;
    auto f = p.getFuture();

    std::thread t([p]() mutable {
      p.setValue("TestCoroutine1");
    });
    t.detach();

    return f.getAsync().callbackTo(&TestCoroutine1::onValue);
  }

  Action onValue(const oatpp::String& result) {
    TestCoroutine1_Counter ++;
    OATPP_ASSERT(result == "TestCoroutine1")
    return finish();
  }

  Action handleError(Error* error) override {
    OATPP_ASSERT(false || "TestCoroutine1 - no error expected")
    return error;
  }

};

class TestCoroutine2 : public oatpp::async::Coroutine<TestCoroutine2> {
public:

  Action act() override {
    oatpp::async::Promise<oatpp::String> p;
    auto f = p.getFuture();

    std::thread t([p]() mutable {
      try{
        throw std::runtime_error("test-error");
      } catch(...) {
        p.setException(std::current_exception());
      }
    });
    t.detach();
    return f.getAsync().callbackTo(&TestCoroutine2::onValue);
  }

  Action onValue(const oatpp::String& result) {
    OATPP_ASSERT(false || "TestCoroutine2 - no onValue expected")
    return finish();
  }

  Action handleError(Error* error) override {
    OATPP_ASSERT(oatpp::String(error->what()) == "test-error")
    TestCoroutine2_Counter ++;
    return error;
  }

};

class TestCoroutine3 : public oatpp::async::Coroutine<TestCoroutine3> {
private:
  oatpp::async::Future<oatpp::String> f1;
  oatpp::async::Future<oatpp::String> f2;
  oatpp::async::Future<oatpp::String> f3;
public:

  Action act() override {
    oatpp::async::Promise<oatpp::String> p1;
    oatpp::async::Promise<oatpp::String> p2;
    oatpp::async::Promise<oatpp::String> p3;
    f1 = p1.getFuture();
    f2 = p2.getFuture();
    f3 = p3.getFuture();

    std::thread t([p1, p2, p3]() mutable {
      p1.setValue("TestCoroutine3.1");
      p2.setValue("TestCoroutine3.2");
      p3.setValue("TestCoroutine3.3");
    });
    t.detach();

    return f1.waitAsync().next(f2.waitAsync()).next(f3.waitAsync()).next(yieldTo(&TestCoroutine3::onValues));
  }

  Action onValues() {
    TestCoroutine3_Counter ++;
    OATPP_ASSERT(f1.get() == "TestCoroutine3.1")
    OATPP_ASSERT(f2.get() == "TestCoroutine3.2")
    OATPP_ASSERT(f3.get() == "TestCoroutine3.3")
    return finish();
  }

  Action handleError(Error* error) override {
    OATPP_ASSERT(false || "TestCoroutine3 - no error expected")
    return error;
  }

};

}

void FutureTest::onRun() {

  TestCoroutine1_Counter = 0;
  TestCoroutine2_Counter = 0;
  TestCoroutine3_Counter = 0;

  oatpp::async::Executor executor;

  for(v_int32 i = 0; i < 100; i ++) {
    executor.execute<TestCoroutine1>();
    executor.execute<TestCoroutine2>();
    executor.execute<TestCoroutine3>();
  }

  executor.waitTasksFinished();
  executor.stop();
  executor.join();

  OATPP_ASSERT(TestCoroutine1_Counter == 100);
  OATPP_ASSERT(TestCoroutine2_Counter == 100);
  OATPP_ASSERT(TestCoroutine3_Counter == 100);

}

}}}
