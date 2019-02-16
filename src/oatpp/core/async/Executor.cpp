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

#include "Executor.hpp"

namespace oatpp { namespace async {
  
void Executor::SubmissionProcessor::consumeTasks() {
  oatpp::concurrency::SpinLock lock(m_atom);
  auto curr = m_pendingTasks.getFirstNode();
  while (curr != nullptr) {
    m_processor.addWaitingCoroutine(curr->getData()->createCoroutine());
    curr = curr->getNext();
  }
  m_pendingTasks.clear();
}

void Executor::SubmissionProcessor::run(){
  
  while(m_isRunning) {
    
    /* Load all waiting connections into processor */
    consumeTasks();
    
    /* Process all, and check for incoming connections once in 1000 iterations */
    while (m_processor.iterate(1000)) {
      consumeTasks();
    }
    
    std::unique_lock<std::mutex> lock(m_taskMutex);
    if(m_processor.isEmpty()) {
      /* No tasks in the processor. Wait for incoming connections */
      m_taskCondition.wait_for(lock, std::chrono::milliseconds(500));
    } else {
      /* There is still something in slow queue. Wait and get back to processing */
      /* Waiting for IO is not Applicable here as slow queue may contain NON-IO tasks */
      //OATPP_LOGD("proc", "waiting slow queue");
      m_taskCondition.wait_for(lock, std::chrono::milliseconds(10));
    }
    
  }
  
}
  
}}
