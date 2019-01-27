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

bool Processor::considerContinueImmediately() {
  
  bool hasAction = checkWaitingQueue();
  
  if(hasAction) {
    m_inactivityTick = 0;
  } else if(m_inactivityTick == 0) {
    m_inactivityTick = oatpp::base::Environment::getMicroTickCount();
  } else if(oatpp::base::Environment::getMicroTickCount() - m_inactivityTick > 1000 * 100 /* 100 millis */) {
    return m_activeQueue.first != nullptr;
  }
  
  return true;
  
}
  
void Processor::addCoroutine(AbstractCoroutine* coroutine) {
  m_activeQueue.pushBack(coroutine);
}
  
void Processor::addWaitingCoroutine(AbstractCoroutine* coroutine) {
  m_waitingQueue.pushBack(coroutine);
}

bool Processor::iterate(v_int32 numIterations) {
  
  for(v_int32 i = 0; i < numIterations; i++) {
    
    auto CP = m_activeQueue.first;
    if(CP == nullptr) {
      break;
    }
    if(!CP->finished()) {
      const Action& action = CP->iterate();
      if(action.m_type == Action::TYPE_WAIT_RETRY) {
        m_waitingQueue.pushBack(m_activeQueue.popFront());
      } else {
        m_activeQueue.round();
      }
    } else {
      m_activeQueue.popFrontNoData();
    }
  }
  
  return considerContinueImmediately();
  
}
  
}}
