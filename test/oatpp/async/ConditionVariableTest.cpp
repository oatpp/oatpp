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

#include "oatpp/async/Executor.hpp"
#include "oatpp/async/ConditionVariable.hpp"

namespace oatpp { namespace async {

namespace {

struct Resource {
  v_int64 counter;
};

class TestCoroutineWait : public oatpp::async::Coroutine<TestCoroutineWait> {
private:
  std::shared_ptr<Resource> m_resource;
  oatpp::async::LockGuard m_lockGuard;
  oatpp::async::ConditionVariable* m_cv;
public:

  TestCoroutineWait(std::shared_ptr<Resource> resource,
                    oatpp::async::Lock* lock,
                    oatpp::async::ConditionVariable* cv)
    : m_resource(resource)
    , m_lockGuard(lock)
    , m_cv(cv)
  {}

  Action act() override {
    return m_cv->wait(m_lockGuard, [this]() noexcept {return m_resource->counter == 100;})
            .next(yieldTo(&TestCoroutineWait::onReady));
  }

  Action onReady() {
    OATPP_ASSERT(m_lockGuard.owns_lock())
    return finish();
  }

};

class TestCoroutineWaitWithTimeout : public oatpp::async::Coroutine<TestCoroutineWaitWithTimeout> {
private:
  std::shared_ptr<Resource> m_resource;
  oatpp::async::LockGuard m_lockGuard;
  oatpp::async::ConditionVariable* m_cv;
public:

  TestCoroutineWaitWithTimeout(std::shared_ptr<Resource> resource,
                               oatpp::async::Lock* lock,
                               oatpp::async::ConditionVariable* cv)
    : m_resource(resource)
    , m_lockGuard(lock)
    , m_cv(cv)
  {}

  Action act() override {
    return m_cv->waitFor(m_lockGuard, [this]() noexcept{return m_resource->counter == 100;}, std::chrono::seconds(5))
      .next(yieldTo(&TestCoroutineWaitWithTimeout::onReady));
  }

  Action onReady() {
    OATPP_ASSERT(m_lockGuard.owns_lock())
    return finish();
  }

};

class TestCoroutineTimeout : public oatpp::async::Coroutine<TestCoroutineTimeout> {
private:
  std::shared_ptr<Resource> m_resource;
  oatpp::async::LockGuard m_lockGuard;
  oatpp::async::ConditionVariable* m_cv;
public:

  TestCoroutineTimeout(std::shared_ptr<Resource> resource,
                       oatpp::async::Lock* lock,
                       oatpp::async::ConditionVariable* cv)
    : m_resource(resource)
    , m_lockGuard(lock)
    , m_cv(cv)
  {}

  Action act() override {
    return m_cv->waitFor(m_lockGuard, []() noexcept{return false;}, std::chrono::seconds(1))
      .next(yieldTo(&TestCoroutineTimeout::onReady));
  }

  Action onReady() {
    return finish();
  }

};

}

void ConditionVariableTest::onRun() {

  std::atomic<bool> finished(false);

  std::thread timeoutThread([&finished] {
    auto now = oatpp::Environment::getMicroTickCount();
    while(!finished) {
      auto ticks = oatpp::Environment::getMicroTickCount();
      if(ticks - now > 10 * 60 * 1000 * 1000) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    OATPP_ASSERT(finished)
  });

  {

    for (v_int32 iter = 0; iter < 100; iter++) {

      OATPP_LOGd("ITERATION 'WAIT'", "{}", iter)

      oatpp::async::Executor executor;

      auto resource = std::make_shared<Resource>();
      oatpp::async::Lock lock;
      oatpp::async::ConditionVariable cv;
      executor.execute<TestCoroutineWait>(resource, &lock, &cv);

      std::vector<std::thread> threads;

      for (v_int32 i = 0; i < 100; i++) {
        threads.push_back(std::thread([&resource, &lock, &cv] {
          {
            std::lock_guard<oatpp::async::Lock> guard(lock);
            resource->counter++;
          }
          cv.notifyAll();
        }));
      }

      for(auto& t : threads) {
        t.join();
      }

      executor.waitTasksFinished();
      executor.stop();
      executor.join();

    }

  }

  {

    for (v_int32 iter = 0; iter < 100; iter++) {

      OATPP_LOGd("ITERATION 'WAIT-WITH-TIMEOUT'", "{}", iter)

      oatpp::async::Executor executor;

      auto resource = std::make_shared<Resource>();
      oatpp::async::Lock lock;
      oatpp::async::ConditionVariable cv;
      executor.execute<TestCoroutineWaitWithTimeout>(resource, &lock, &cv);

      std::vector<std::thread> threads;

      for (v_int32 i = 0; i < 100; i++) {
        threads.push_back(std::thread([&resource, &lock, &cv] {
          {
            std::lock_guard<oatpp::async::Lock> guard(lock);
            resource->counter++;
          }
          cv.notifyAll();
        }));
      }

      for(auto& t : threads) {
        t.join();
      }

      executor.waitTasksFinished();
      executor.stop();
      executor.join();

    }

  }

  {

    for (v_int32 iter = 0; iter < 5; iter++) {

      OATPP_LOGd("ITERATION 'TIMEOUT'", "{}", iter)

      oatpp::async::Executor executor;

      auto resource = std::make_shared<Resource>();
      oatpp::async::Lock lock;
      oatpp::async::ConditionVariable cv;
      executor.execute<TestCoroutineTimeout>(resource, &lock, &cv);

      executor.waitTasksFinished();
      executor.stop();
      executor.join();

    }

  }

  finished = true;
  timeoutThread.join();

}

}}
