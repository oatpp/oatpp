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

const v_int32 Executor::THREAD_NUM_DEFAULT = OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT;

Executor::SubmissionProcessor::SubmissionProcessor()
  : m_atom(false)
  , m_isRunning(true)
{}

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

void Executor::SubmissionProcessor::stop() {
  m_isRunning = false;
}

void Executor::SubmissionProcessor::addTaskSubmission(const std::shared_ptr<TaskSubmission>& task){
  oatpp::concurrency::SpinLock lock(m_atom);
  m_pendingTasks.pushBack(task);
  m_taskCondition.notify_one();
}


Executor::Executor(v_int32 threadsCount)
  : m_threadsCount(threadsCount)
  , m_threads(new std::thread[m_threadsCount])
  , m_processors(new SubmissionProcessor[m_threadsCount])
{
  for(v_int32 i = 0; i < m_threadsCount; i ++) {
    m_threads[i] = std::thread(&SubmissionProcessor::run, &m_processors[i]);
  }
}

Executor::~Executor() {
  delete [] m_processors;
  delete [] m_threads;
}

void Executor::join() {
  for(v_int32 i = 0; i < m_threadsCount; i ++) {
    m_threads[i].join();
  }
}

void Executor::detach() {
  for(v_int32 i = 0; i < m_threadsCount; i ++) {
    m_threads[i].detach();
  }
}

void Executor::stop() {
  for(v_int32 i = 0; i < m_threadsCount; i ++) {
    m_processors[i].stop();
  }
}
  
}}
