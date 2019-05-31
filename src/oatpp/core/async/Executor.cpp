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
#include "oatpp/core/async/worker/IOEventWorker.hpp"
#include "oatpp/core/async/worker/IOWorker.hpp"
#include "oatpp/core/async/worker/TimerWorker.hpp"

namespace oatpp { namespace async {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Executor::SubmissionProcessor

Executor::SubmissionProcessor::SubmissionProcessor()
  : worker::Worker(worker::Worker::Type::PROCESSOR)
  , m_isRunning(true)
{
  m_thread = std::thread(&Executor::SubmissionProcessor::run, this);
}

oatpp::async::Processor& Executor::SubmissionProcessor::getProcessor() {
  return m_processor;
}

void Executor::SubmissionProcessor::run() {
  
  while(m_isRunning) {
    m_processor.waitForTasks();
    while (m_processor.iterate(100)) {}
  }
  
}

void Executor::SubmissionProcessor::pushTasks(oatpp::collection::FastQueue<AbstractCoroutine>& tasks) {
  std::runtime_error("[oatpp::async::Executor::SubmissionProcessor::pushTasks]: Error. This method does nothing.");
}

void Executor::SubmissionProcessor::pushOneTask(AbstractCoroutine* task) {
  std::runtime_error("[oatpp::async::Executor::SubmissionProcessor::pushOneTask]: Error. This method does nothing.");
}

void Executor::SubmissionProcessor::stop() {
  m_isRunning = false;
  m_processor.stop();
}

void Executor::SubmissionProcessor::join() {
  m_thread.join();
}

void Executor::SubmissionProcessor::detach() {
  m_thread.detach();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Executor

const v_int32 Executor::THREAD_NUM_DEFAULT = OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT;

Executor::Executor(v_int32 processorWorkersCount, v_int32 ioWorkersCount, v_int32 timerWorkersCount)
  : m_balancer(0)
{

  for(v_int32 i = 0; i < processorWorkersCount; i ++) {
    m_processorWorkers.push_back(std::make_shared<SubmissionProcessor>());
  }

  m_allWorkers.insert(m_allWorkers.end(), m_processorWorkers.begin(), m_processorWorkers.end());

  std::vector<std::shared_ptr<worker::Worker>> ioWorkers;
  for(v_int32 i = 0; i < ioWorkersCount; i++) {
    ioWorkers.push_back(std::make_shared<worker::IOEventWorkerForeman>());
  }

  linkWorkers(ioWorkers);

  std::vector<std::shared_ptr<worker::Worker>> timerWorkers;
  for(v_int32 i = 0; i < timerWorkersCount; i++) {
    timerWorkers.push_back(std::make_shared<worker::TimerWorker>());
  }

  linkWorkers(timerWorkers);

}

Executor::~Executor() {
}

void Executor::linkWorkers(const std::vector<std::shared_ptr<worker::Worker>>& workers) {

  m_allWorkers.insert(m_allWorkers.end(), workers.begin(), workers.end());

  if(m_processorWorkers.size() > workers.size() && (m_processorWorkers.size() % workers.size()) == 0) {

    v_int32 wi = 0;
    for(v_int32 i = 0; i < m_processorWorkers.size(); i ++) {
      auto& p = m_processorWorkers[i];
      p->getProcessor().addWorker(workers[wi]);
      wi ++;
      if(wi == workers.size()) {
        wi = 0;
      }
    }

  } else if ((workers.size() % m_processorWorkers.size()) == 0) {

    v_int32 pi = 0;
    for(v_int32 i = 0; i < workers.size(); i ++) {
      auto& p = m_processorWorkers[pi];
      p->getProcessor().addWorker(workers[i]);
      pi ++;
      if(pi == m_processorWorkers.size()) {
        pi = 0;
      }
    }

  } else {

    for(v_int32 i = 0; i < m_processorWorkers.size(); i ++) {
      auto& p = m_processorWorkers[i];
      for(auto& w : workers) {
        p->getProcessor().addWorker(w);
      }
    }

  }

}

void Executor::join() {
  for(auto& worker : m_allWorkers) {
    worker->join();
  }
}

void Executor::detach() {
  for(auto& worker : m_allWorkers) {
    worker->detach();
  }
}

void Executor::stop() {
  for(auto& worker : m_allWorkers) {
    worker->stop();
  }
}

v_int32 Executor::getTasksCount() {

  v_int32 result = 0;

  for(auto procWorker : m_processorWorkers) {
    result += procWorker->getProcessor().getTasksCount();
  }

  return result;

}

void Executor::waitTasksFinished(const std::chrono::duration<v_int64, std::micro>& timeout) {

  auto startTime = std::chrono::system_clock::now();

  while(getTasksCount() != 0) {
    auto elapsed = std::chrono::system_clock::now() - startTime;
    if(elapsed < timeout) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else {
      break;
    }
  }

}
  
}}
