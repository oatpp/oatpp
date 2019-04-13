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

#include "./IOWorker.hpp"
#include "./TimerWorker.hpp"

namespace oatpp { namespace async {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Executor::SubmissionProcessor

Executor::SubmissionProcessor::SubmissionProcessor()
  : m_isRunning(true)
{}

void Executor::SubmissionProcessor::run(){
  
  while(m_isRunning) {
    m_processor.waitForTasks();
    while (m_processor.iterate(100)) {}
  }
  
}

void Executor::SubmissionProcessor::stop() {
  m_isRunning = false;
  m_processor.stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Executor

const v_int32 Executor::THREAD_NUM_DEFAULT = OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT;

Executor::Executor(v_int32 threadsCount)
  : m_threadsCount(threadsCount)
  , m_threads(new std::thread[m_threadsCount])
  , m_processors(new SubmissionProcessor[m_threadsCount])
{

  //auto ioWorker = std::make_shared<IOWorker>();

  for(v_int32 i = 0; i < 2; i++) {
    m_workers.push_back(std::make_shared<TimerWorker>());
  }

  for(v_int32 i = 0; i < m_threadsCount; i ++) {

    auto& processor = m_processors[i];

    for(auto& worker : m_workers) {
      processor.getProcessor().addWorker(worker);
    }

//    for(v_int32 i = 0; i < 1; i++) {
//      auto worker = std::make_shared<TimerWorker>();
//      m_workers.push_back(worker);
//      processor.getProcessor().addWorker(worker);
//    }


    m_threads[i] = std::thread(&SubmissionProcessor::run, &processor);

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

  for(auto& worker : m_workers) {
    worker->stop();
  }
}
  
}}
