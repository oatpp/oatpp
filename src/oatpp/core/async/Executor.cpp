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

Executor::Executor(v_int32 processorThreads, v_int32 ioThreads, v_int32 timerThreads)
  : m_processorThreads(processorThreads)
  , m_ioThreads(ioThreads)
  , m_timerThreads(timerThreads)
  , m_threads(new std::thread[m_processorThreads])
  , m_processors(new SubmissionProcessor[m_processorThreads])
{

  for(v_int32 i = 0; i < m_processorThreads; i ++) {
    m_threads[i] = std::thread(&SubmissionProcessor::run, &m_processors[i]);
  }

  std::vector<std::shared_ptr<Worker>> ioWorkers;
  for(v_int32 i = 0; i < m_ioThreads; i++) {
    ioWorkers.push_back(std::make_shared<IOWorker>());
  }

  linkWorkers(ioWorkers);

  std::vector<std::shared_ptr<Worker>> timerWorkers;
  for(v_int32 i = 0; i < m_timerThreads; i++) {
    timerWorkers.push_back(std::make_shared<TimerWorker>());
  }

  linkWorkers(timerWorkers);

}

Executor::~Executor() {
  delete [] m_processors;
  delete [] m_threads;
}

void Executor::linkWorkers(const std::vector<std::shared_ptr<Worker>>& workers) {

  m_workers.insert(m_workers.end(), workers.begin(), workers.end());

  if(m_processorThreads > workers.size() && (m_processorThreads % workers.size()) == 0) {

    v_int32 wi = 0;
    for(v_int32 i = 0; i < m_processorThreads; i ++) {
      auto& p = m_processors[i];
      p.getProcessor().addWorker(workers[wi]);
      wi ++;
      if(wi == workers.size()) {
        wi = 0;
      }
    }

  } else if ((workers.size() % m_processorThreads) == 0) {

    v_int32 pi = 0;
    for(v_int32 i = 0; i < workers.size(); i ++) {
      auto& p = m_processors[pi];
      p.getProcessor().addWorker(workers[i]);
      pi ++;
      if(pi == m_processorThreads) {
        pi = 0;
      }
    }

  } else {

    for(v_int32 i = 0; i < m_processorThreads; i ++) {
      auto& p = m_processors[i];
      for(auto& w : workers) {
        p.getProcessor().addWorker(w);
      }
    }

  }

}

void Executor::join() {
  for(v_int32 i = 0; i < m_processorThreads; i ++) {
    m_threads[i].join();
  }
}

void Executor::detach() {
  for(v_int32 i = 0; i < m_processorThreads; i ++) {
    m_threads[i].detach();
  }
}

void Executor::stop() {
  for(v_int32 i = 0; i < m_processorThreads; i ++) {
    m_processors[i].stop();
  }

  for(auto& worker : m_workers) {
    worker->stop();
  }
}
  
}}
