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

#ifndef oatpp_async_worker_IOEventWorker_hpp
#define oatpp_async_worker_IOEventWorker_hpp

#include "./Worker.hpp"
#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"

#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async { namespace worker {

class IOEventWorker : public Worker {
private:
  static constexpr const v_int32 MAX_EVENTS = 10000;
private:
  bool m_running;
  oatpp::collection::FastQueue<AbstractCoroutine> m_backlog;
  oatpp::concurrency::SpinLock m_backlogLock;
private:
  oatpp::data::v_io_handle m_eventQueueHandle;
  p_char8 m_inEvents;
  v_int32 m_inEventsCount;
  p_char8 m_outEvents;
private:
  void consumeBacklog();
  void waitEvents();
private:
  void initEventQueue();
  void triggerWakeup();
  void setTriggerEvent(p_char8 eventPtr);
  void setCoroutineEvent(AbstractCoroutine* coroutine, p_char8 eventPtr);
public:

  IOEventWorker();

  ~IOEventWorker();

  void pushTasks(oatpp::collection::FastQueue<AbstractCoroutine>& tasks) override;

  void pushOneTask(AbstractCoroutine* task) override;

  void work();

  void stop() override;

};

}}}

#endif //oatpp_async_worker_IOEventWorker_hpp
