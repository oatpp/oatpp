/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2021-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include "AbortTest.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/async/Lock.hpp"

#include <thread>
#include <list>

namespace oatpp { namespace test { namespace async {

class AbortTestCoroutine : public oatpp::async::Coroutine<AbortTestCoroutine> {
 private:
  bool m_repeated;
  v_uint32 m_id;
  std::shared_ptr<std::list<v_uint32>> m_list;
 public:
  AbortTestCoroutine(const std::shared_ptr<std::list<v_uint32>> &list, v_uint32 id) : m_repeated(false), m_list(list), m_id(id) {
//    OATPP_LOGD("oatpp::test::async::AbortTestCoroutine", "Constructing %u", m_id);
  };

  ~AbortTestCoroutine() {
//    OATPP_LOGD("oatpp::test::async::AbortTestCoroutine", "Destructing %u", m_id);
  }

  oatpp::async::Action act() override {
    if (m_repeated) {
//      OATPP_LOGD("oatpp::test::async::AbortTestCoroutine", "Pushing id %u", m_id);
      m_list->push_back(m_id);
      return finish();
    }
    m_repeated = true;
//    OATPP_LOGD("oatpp::test::async::AbortTestCoroutine", "Waiting 1000000µs before pushing id %u", m_id);
    return oatpp::async::Action::createWaitRepeatAction(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 1000000);
  }
};


void AbortTest::onRun() {

  oatpp::async::Executor executor(10, 2, 2);
  std::vector<std::pair<v_uint32, v_uint64>> coroutines;
  auto results = std::make_shared<std::list<v_uint32>>();

  static const v_uint32 ABORTID = 17;
  static const v_uint32 COROUTINES = 33;

  for (v_uint32 i = 0; i < COROUTINES; ++i) {
    coroutines.emplace_back(i, executor.execute<AbortTestCoroutine>(results, i));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  executor.abortCoroutine(coroutines.at(ABORTID).second);
  executor.waitTasksFinished(std::chrono::seconds(3));
  executor.stop();
  executor.join();

  OATPP_ASSERT(results->size() == COROUTINES-1);
  for (auto r : *results) {
    OATPP_ASSERT(r != coroutines.at(ABORTID).first)
  }
}

}}}