/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#ifndef oatpp_concurrency_ThreadPool_hpp
#define oatpp_concurrency_ThreadPool_hpp

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "Thread.hpp"
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>

namespace oatpp { namespace concurrency {

template <class T>
class ThreadPool : public oatpp::base::Countable {
 private:
  v_buff_size m_maxThreads;
  std::atomic_bool m_run;
  std::atomic_uintmax_t m_running;
  std::atomic_uintmax_t m_waiting;
  std::vector<std::thread> m_threads;
  std::queue<std::shared_ptr<T>> m_queue;
  std::condition_variable m_cond;
  std::mutex m_qmutex;

 private:
  void poolFunc() {
    ++m_running;
    while (m_run) {
      std::shared_ptr<T> task;
      {
        std::unique_lock<std::mutex> ul(m_qmutex);
        ++m_waiting;
        m_cond.wait(ul, [this](){ return !m_queue.empty() || !m_run;});
        --m_waiting;
        if (!m_run) {
          break;
        }
        task = m_queue.front();
        m_queue.pop();
      }
      task->run();
    }
    --m_running;
  }

 public:
  ThreadPool(v_buff_size threads = oatpp::concurrency::getHardwareConcurrency()) : m_maxThreads(threads), m_run(true) {}
  ~ThreadPool() {
    join();
  }

  v_buff_size enqueue(const std::shared_ptr<T> task) {
    v_buff_size pos;
    {
      std::unique_lock<std::mutex> lg(m_qmutex);
      m_queue.push(task);
      pos = m_queue.size();
    }
    // grow if needed
    if ((m_queue.size() > m_threads.size() || m_waiting == 0) && m_threads.size() < m_maxThreads) {
      m_threads.emplace_back(&ThreadPool<T>::poolFunc, this);
    }
    m_cond.notify_one();
    return pos;
  }

  void stop() {
    m_run = false;
    m_cond.notify_all();
  }

  void join() {
    for (auto &t : m_threads) {
      if (t.joinable()) {
        t.join();
      }
    }
    m_threads.clear();
  }
};

}}

#endif /* concurrency_Thread_hpp */