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

#ifndef oatpp_async_ConditionVariable_hpp
#define oatpp_async_ConditionVariable_hpp

#include "./Lock.hpp"

#include <functional>

namespace oatpp { namespace async {

class ConditionVariable : private CoroutineWaitList::Listener {
public:

  class Condition {
  public:
    virtual bool check() = 0;
  };

public:

  template<typename F>
  class ConditionTemplate : public Condition {
  private:
    F lambda;
  public:
    ConditionTemplate(const F& f) : lambda(f) {}
    bool check() override {
      return lambda();
    }
  };

private:
  std::atomic_bool m_notified;
  CoroutineWaitList m_list;
private:
  void onNewItem(CoroutineWaitList& list) override;
public:

  ConditionVariable();

  CoroutineStarter wait(LockGuard& lockGuard, std::function<bool()> condition);
  CoroutineStarter waitUntil(LockGuard& lockGuard, std::function<bool()> condition, const std::chrono::system_clock::time_point& timeoutTime);
  CoroutineStarter waitFor(LockGuard& lockGuard, std::function<bool()>, const std::chrono::duration<v_int64, std::micro>& timeout);

  void notifyFirst();
  void notifyAll();

};

}}

#endif //oatpp_async_ConditionVariable_hpp
