/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
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

#include "CoroutineWaitList.hpp"

#include "./Processor.hpp"
#include <algorithm>
#include <set>

namespace oatpp { namespace async {


CoroutineWaitList::CoroutineWaitList(CoroutineWaitList&& other) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock{other.m_lock};
    m_list = std::move(other.m_list);
  }
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock{other.m_timeoutsLock};
    m_coroutinesWithTimeout = std::move(other.m_coroutinesWithTimeout);

    m_timeoutCheckingProcessors = std::move(other.m_timeoutCheckingProcessors);
    for (const std::pair<Processor*, v_int64>& entry : m_timeoutCheckingProcessors) {
      Processor* processor = entry.first;
      processor->removeCoroutineWaitListWithTimeouts(std::addressof(other));
      processor->addCoroutineWaitListWithTimeouts(this);
    }
  }  
}

CoroutineWaitList::~CoroutineWaitList() {
  notifyAll();
}

void CoroutineWaitList::checkCoroutinesForTimeouts() {
  std::lock_guard<oatpp::concurrency::SpinLock> listLock{m_lock};
  std::lock_guard<oatpp::concurrency::SpinLock> lock{m_timeoutsLock};
  const auto currentTimeSinceEpochMS = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
  const auto newEndIt = std::remove_if(std::begin(m_coroutinesWithTimeout), std::end(m_coroutinesWithTimeout), [&](const std::pair<CoroutineHandle*, v_int64>& entry) {
    return currentTimeSinceEpochMS > entry.second;
  });
  
  for (CoroutineHandle* curr = m_list.first, *prev = nullptr; !m_list.empty() && m_list.last->_ref != curr; curr = curr->_ref) {
    const bool removeFromWaitList = std::any_of(newEndIt, std::end(m_coroutinesWithTimeout), [=](const std::pair<CoroutineHandle*, v_int64>& entry) {
      return entry.first == curr;
    });
    if (!removeFromWaitList) {
      prev = curr;
      continue;
    }
    
    m_list.cutEntry(curr, prev);   

    if (--m_timeoutCheckingProcessors[curr->_PP] <= 0) {
      curr->_PP->removeCoroutineWaitListWithTimeouts(this);
      m_timeoutCheckingProcessors.erase(curr->_PP);
    }
    curr->_PP->pushOneTask(curr);
  }

  m_coroutinesWithTimeout.erase(newEndIt, std::end(m_coroutinesWithTimeout));
}

void CoroutineWaitList::setListener(Listener* listener) {
  m_listener = listener;
}

void CoroutineWaitList::pushFront(CoroutineHandle* coroutine) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
    m_list.pushFront(coroutine);
  }
  if(m_listener != nullptr) {
    m_listener->onNewItem(*this);
  }
}

void CoroutineWaitList::pushFront(CoroutineHandle* coroutine, v_int64 timeoutTimeSinceEpochMS) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock{m_timeoutsLock};
    m_coroutinesWithTimeout.emplace_back(coroutine, timeoutTimeSinceEpochMS);
    if (++m_timeoutCheckingProcessors[coroutine->_PP] == 1) {
      coroutine->_PP->addCoroutineWaitListWithTimeouts(this);
    }
  }
  pushFront(coroutine);
}

void CoroutineWaitList::pushBack(CoroutineHandle* coroutine) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
    m_list.pushBack(coroutine);
  }
  if(m_listener != nullptr) {
    m_listener->onNewItem(*this);
  }
}

void CoroutineWaitList::pushBack(CoroutineHandle* coroutine, v_int64 timeoutTimeSinceEpochMS) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock{m_timeoutsLock};
    m_coroutinesWithTimeout.emplace_back(coroutine, timeoutTimeSinceEpochMS);
    if (++m_timeoutCheckingProcessors[coroutine->_PP] == 1) {
      coroutine->_PP->addCoroutineWaitListWithTimeouts(this);
    }
  }
  pushBack(coroutine);
}

void CoroutineWaitList::notifyFirst() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock{m_lock};
  if(m_list.first) {
    removeFirstCoroutine();
  }
}

void CoroutineWaitList::notifyAll() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  while (!m_list.empty()) {
    removeFirstCoroutine();
  }
}

void CoroutineWaitList::removeFirstCoroutine() {
  auto coroutine = m_list.popFront();
  
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock{m_timeoutsLock};
    if (--m_timeoutCheckingProcessors[coroutine->_PP] <= 0) {
      coroutine->_PP->removeCoroutineWaitListWithTimeouts(this);
      m_timeoutCheckingProcessors.erase(coroutine->_PP);
    }
  }

  coroutine->_PP->pushOneTask(coroutine);
}

CoroutineWaitList& CoroutineWaitList::operator=(CoroutineWaitList&& other) {
    if (this == std::addressof(other)) return *this;
      
    notifyAll();
    
    {
      std::lock_guard<oatpp::concurrency::SpinLock> otherLock{other.m_lock};
      std::lock_guard<oatpp::concurrency::SpinLock> myLock{m_lock};
      m_list = std::move(other.m_list);
    }
    {
      std::lock_guard<oatpp::concurrency::SpinLock> otherLock{other.m_timeoutsLock};
      std::lock_guard<oatpp::concurrency::SpinLock> myLock{m_timeoutsLock};
      m_coroutinesWithTimeout = std::move(other.m_coroutinesWithTimeout);

      m_timeoutCheckingProcessors = std::move(other.m_timeoutCheckingProcessors);
      for (const std::pair<Processor*, v_int64>& entry : m_timeoutCheckingProcessors) {
        Processor* processor = entry.first;
        processor->removeCoroutineWaitListWithTimeouts(std::addressof(other));
        processor->addCoroutineWaitListWithTimeouts(this);
      }
    }
    return *this;
  }

}}