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

#include "Processor.hpp"

namespace oatpp { namespace async {
  
bool Processor::checkWaitingQueue() {
  bool hasActions = false;
  AbstractCoroutine* curr = m_waitingQueue.first;
  AbstractCoroutine* prev = nullptr;
  while (curr != nullptr) {
    
    const Action& action = curr->iterate();
    if(action.m_type == Action::TYPE_ABORT) {
      m_waitingQueue.removeEntry(curr, prev);
      if(prev != nullptr) {
        curr = prev;
      } else {
        curr = m_waitingQueue.first;
      }
    } else if(action.m_type != Action::TYPE_WAIT_RETRY) {
      oatpp::collection::FastQueue<AbstractCoroutine>::moveEntry(m_waitingQueue, m_activeQueue, curr, prev);
      hasActions = true;
      if(prev != nullptr) {
        curr = prev;
      } else {
        curr = m_waitingQueue.first;
      }
    }
    
    prev = curr;
    if(curr != nullptr) {
      curr = curr->_ref;
    }
    
  }
  return hasActions;
}

bool Processor::countdownToSleep() {
  ++ m_sleepCountdown;
  if(m_sleepCountdown > 1000) {
    return checkWaitingQueue();
  }
  checkWaitingQueue();
  std::this_thread::yield();
  return true;
}
  
void Processor::addCoroutine(AbstractCoroutine* coroutine) {
  m_activeQueue.pushBack(coroutine);
}

bool Processor::iterate(v_int32 numIterations) {
  
  for(v_int32 i = 0; i < numIterations; i++) {
    
    auto CP = m_activeQueue.first;
    if(CP == nullptr) {
      break;
    } else {
      m_sleepCountdown = 0;
    }
    if(!CP->finished()) {
      const Action& action = CP->iterate();
      if(action.m_type == Action::TYPE_WAIT_RETRY) {
        m_waitingQueue.pushBack(m_activeQueue.popFront());
      }
    } else {
      m_activeQueue.popFrontNoData();
    }
  }
  
  return ((m_activeQueue.first != nullptr) || countdownToSleep());
  
}
  
}}
