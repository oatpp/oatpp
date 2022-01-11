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

void Executor::SubmissionProcessor::pushTasks(utils::FastQueue<CoroutineHandle>& tasks) {
  (void)tasks;
  throw std::runtime_error("[oatpp::async::Executor::SubmissionProcessor::pushTasks]: Error. This method does nothing.");
}

void Executor::SubmissionProcessor::pushOneTask(CoroutineHandle* task) {
  (void)task;
  throw std::runtime_error("[oatpp::async::Executor::SubmissionProcessor::pushOneTask]: Error. This method does nothing.");
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

Executor::Executor(v_int32 processorWorkersCount, v_int32 ioWorkersCount, v_int32 timerWorkersCount, v_int32 ioWorkerType)
  : m_balancer(0)
{

  processorWorkersCount = chooseProcessorWorkersCount(processorWorkersCount);
  ioWorkersCount = chooseIOWorkersCount(processorWorkersCount, ioWorkersCount);
  timerWorkersCount = chooseTimerWorkersCount(timerWorkersCount);
  ioWorkerType = chooseIOWorkerType(ioWorkerType);

  for(v_int32 i = 0; i < processorWorkersCount; i ++) {
    m_processorWorkers.push_back(std::make_shared<SubmissionProcessor>());
  }

  m_allWorkers.insert(m_allWorkers.end(), m_processorWorkers.begin(), m_processorWorkers.end());

  std::vector<std::shared_ptr<worker::Worker>> ioWorkers;
  ioWorkers.reserve(ioWorkersCount);
  switch(ioWorkerType) {

    case IO_WORKER_TYPE_NAIVE: {
      for (v_int32 i = 0; i < ioWorkersCount; i++) {
        ioWorkers.push_back(std::make_shared<worker::IOWorker>());
      }
      break;
    }

    case IO_WORKER_TYPE_EVENT: {
      for (v_int32 i = 0; i < ioWorkersCount; i++) {
        ioWorkers.push_back(std::make_shared<worker::IOEventWorkerForeman>());
      }
      break;
    }

    default:
      throw std::runtime_error("[oatpp::async::Executor::Executor()]: Error. Unknown IO worker type.");

  }

  linkWorkers(ioWorkers);

  std::vector<std::shared_ptr<worker::Worker>> timerWorkers;
  timerWorkers.reserve(timerWorkersCount);
  for(v_int32 i = 0; i < timerWorkersCount; i++) {
    timerWorkers.push_back(std::make_shared<worker::TimerWorker>());
  }

  linkWorkers(timerWorkers);

}

v_int32 Executor::chooseProcessorWorkersCount(v_int32 processorWorkersCount) {
  if(processorWorkersCount >= 1) {
    return processorWorkersCount;
  }
  if(processorWorkersCount == VALUE_SUGGESTED) {
    return oatpp::concurrency::getHardwareConcurrency();
  }
  throw std::runtime_error("[oatpp::async::Executor::chooseProcessorWorkersCount()]: Error. Invalid processor workers count specified.");
}

v_int32 Executor::chooseIOWorkersCount(v_int32 processorWorkersCount, v_int32 ioWorkersCount) {
  if(ioWorkersCount >= 1) {
    return ioWorkersCount;
  }
  if(ioWorkersCount == VALUE_SUGGESTED) {
    v_int32 count = processorWorkersCount >> 1;
    if(count == 0) {
      count = 1;
    }
    return count;
  }
  throw std::runtime_error("[oatpp::async::Executor::chooseIOWorkersCount()]: Error. Invalid I/O workers count specified.");
}

v_int32 Executor::chooseTimerWorkersCount(v_int32 timerWorkersCount) {
  if(timerWorkersCount >= 1) {
    return timerWorkersCount;
  }
  if(timerWorkersCount == VALUE_SUGGESTED) {
    return 1;
  }
  throw std::runtime_error("[oatpp::async::Executor::chooseTimerWorkersCount()]: Error. Invalid timer workers count specified.");
}

v_int32 Executor::chooseIOWorkerType(v_int32 ioWorkerType) {

  if(ioWorkerType == VALUE_SUGGESTED) {
#if defined(OATPP_IO_EVENT_INTERFACE_STUB)
    return IO_WORKER_TYPE_NAIVE;
#else
    return IO_WORKER_TYPE_EVENT;
#endif
  }

  return ioWorkerType;

}

void Executor::linkWorkers(const std::vector<std::shared_ptr<worker::Worker>>& workers) {

  m_allWorkers.insert(m_allWorkers.end(), workers.begin(), workers.end());

  if(m_processorWorkers.size() > workers.size() && (m_processorWorkers.size() % workers.size()) == 0) {

    size_t wi = 0;
    for(auto & p : m_processorWorkers) {
      p->getProcessor().addWorker(workers[wi]);
      wi ++;
      if(wi == workers.size()) {
        wi = 0;
      }
    }

  } else if ((workers.size() % m_processorWorkers.size()) == 0) {

    size_t pi = 0;
    for(const auto & worker : workers) {
      auto& p = m_processorWorkers[pi];
      p->getProcessor().addWorker(worker);
      pi ++;
      if(pi == m_processorWorkers.size()) {
        pi = 0;
      }
    }

  } else {

    for(auto & p : m_processorWorkers) {
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

  for(const auto& procWorker : m_processorWorkers) {
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
