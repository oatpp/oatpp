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

#include "Lock.hpp"

namespace oatpp { namespace async {

Lock::Lock()
  : m_counter(0)
{
  m_list.setListener(this);
}

void Lock::onNewItem(CoroutineWaitList& list) {
  if(m_counter == 0) {
    list.notifyFirst();
  } else if(m_counter < 0) {
    throw std::runtime_error("[oatpp::async::Lock::onNewItem()]: Error. Invalid state.");
  }
}

Action Lock::waitAsync() {
  if(m_counter > 0) {
    return Action::createWaitListAction(&m_list);
  } else if(m_counter == 0) {
    return Action::createActionByType(Action::TYPE_REPEAT);
  }
  throw std::runtime_error("[oatpp::async::Lock::waitAsync()]: Error. Invalid state.");
}

void Lock::lock() {
  m_mutex.lock();
  ++ m_counter;
}

void Lock::unlock() {
  m_mutex.unlock();
  -- m_counter;
  if(m_counter < 0) {
    throw std::runtime_error("[oatpp::async::Lock::unlock()]: Error. Invalid state.");
  }
  m_list.notifyFirst();
}

bool Lock::try_lock() {
  bool result = m_mutex.try_lock();
  if(result) {
    ++ m_counter;
  }
  return result;
}

}}
