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

#include "CoroutineWaitList.hpp"

#include "./Processor.hpp"

namespace oatpp { namespace async {


CoroutineWaitList::CoroutineWaitList(CoroutineWaitList&& other) {
    m_list = std::move(other.m_list);
}

CoroutineWaitList::~CoroutineWaitList() {
  notifyAll();
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

void CoroutineWaitList::pushBack(CoroutineHandle* coroutine) {
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
    m_list.pushBack(coroutine);
  }
  if(m_listener != nullptr) {
    m_listener->onNewItem(*this);
  }
}

void CoroutineWaitList::notifyFirst() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  if(m_list.first) {
    auto coroutine = m_list.popFront();
    coroutine->_PP->pushOneTask(coroutine);
  }
}

void CoroutineWaitList::notifyAll() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
    while (!m_list.empty()) {
        auto curr = m_list.popFront();
        curr->_PP->pushOneTask(curr);
    }
}


}}