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

#include "ConditionVariableTest.hpp"

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/async/ConditionVariable.hpp"

namespace oatpp { namespace test { namespace async {

namespace {

struct Resource {
  v_int64 counter;
};

class TestCoroutine : public oatpp::async::Coroutine<TestCoroutine> {
private:
  std::shared_ptr<Resource> m_resource;
  oatpp::async::Lock* m_lock;
  oatpp::async::ConditionVariable* m_cv;
public:

  TestCoroutine(std::shared_ptr<Resource> resource,
                oatpp::async::Lock* lock,
                oatpp::async::ConditionVariable* cv)
    : m_resource(resource)
    , m_lock(lock)
    , m_cv(cv)
  {}

  bool condition() {
    OATPP_LOGD("Resource", "%d", m_resource->counter)
    return m_resource->counter == 100;
  }

  Action act() override {
    OATPP_LOGD("TestCoroutine", "Waiting...")
    return m_cv->waitFor(m_lock, [this]{return condition();}, std::chrono::milliseconds(5000)).next(yieldTo(&TestCoroutine::onReady));
  }

  Action onReady() {
    OATPP_LOGD("TestCoroutine", "Ready!!!")
    return finish();
  }

};

}

void ConditionVariableTest::onRun() {

  for(v_int32 iter = 0; iter < 100; iter ++ ) {

    OATPP_LOGD("ITER", "%d", iter)

    oatpp::async::Executor executor;

    auto resource = std::make_shared<Resource>();
    oatpp::async::Lock lock;
    oatpp::async::ConditionVariable cv;
    executor.execute<TestCoroutine>(resource, &lock, &cv);

    for (v_int32 i = 0; i < 100; i++) {
      std::thread t([&resource, &lock, &cv] {
        {
          std::lock_guard<oatpp::async::Lock> guard(lock);
          resource->counter++;
        }
        cv.notifyAll();
      });
      t.detach();
    }

    executor.waitTasksFinished();
    executor.stop();
    executor.join();

  }

}

}}}
