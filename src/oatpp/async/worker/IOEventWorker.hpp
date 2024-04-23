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
#include "oatpp/concurrency/SpinLock.hpp"

#include <thread>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(OATPP_IO_EVENT_INTERFACE)

  #if defined(__linux__) || defined(linux) || defined(__linux)

    #define OATPP_IO_EVENT_INTERFACE "epoll"
    #define OATPP_IO_EVENT_INTERFACE_EPOLL

  #elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
        defined(__bsdi__) || defined(__DragonFly__)|| defined(__APPLE__)

    #define OATPP_IO_EVENT_INTERFACE "kqueue"
    #define OATPP_IO_EVENT_INTERFACE_KQUEUE

  #else

    #define OATPP_IO_EVENT_INTERFACE "not-implemented(windows)"
    #define OATPP_IO_EVENT_INTERFACE_STUB

  #endif

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace oatpp { namespace async { namespace worker {

class IOEventWorkerForeman; // FWD

/**
 * Event-based implementation of I/O worker.
 * <ul>
 *   <li>`kqueue` based implementation - for Mac/BSD systems</li>
 *   <li>`epoll` based implementation - for Linux systems</li>
 * </ul>
 */
class IOEventWorker : public Worker {
private:
  static constexpr const v_int32 MAX_EVENTS = 10000;
private:
  IOEventWorkerForeman* m_foreman;
  Action::IOEventType m_specialization;
  std::atomic<bool> m_running;
  utils::FastQueue<CoroutineHandle> m_backlog;
  oatpp::concurrency::SpinLock m_backlogLock;
private:
  oatpp::v_io_handle m_eventQueueHandle;
  oatpp::v_io_handle m_wakeupTrigger;
  std::unique_ptr<v_char8[]> m_inEvents;
  v_int32 m_inEventsCount;
  v_int32 m_inEventsCapacity;
  std::unique_ptr<v_char8[]> m_outEvents;
private:
  std::thread m_thread;
private:
  void consumeBacklog();
  void waitEvents();
private:
  void initEventQueue();
  void triggerWakeup();
  void setTriggerEvent(p_char8 eventPtr);
  void setCoroutineEvent(CoroutineHandle* coroutine, int operation, p_char8 eventPtr);
public:

  /**
   * Constructor.
   */
  IOEventWorker(IOEventWorkerForeman* foreman, Action::IOEventType specialization);

  /**
   * Virtual destructor.
   */
  ~IOEventWorker() override;

  /**
   * Push list of tasks to worker.
   * @param tasks - &id:oatpp::async::utils::FastQueue; of &id:oatpp::async::CoroutineHandle;.
   */
  void pushTasks(utils::FastQueue<CoroutineHandle>& tasks) override;

  /**
   * Push one task to worker.
   * @param task - &id:CoroutineHandle;.
   */
  void pushOneTask(CoroutineHandle* task) override;

  /**
   * Run worker.
   */
  void run();

  /**
   * Break run loop.
   */
  void stop() override;

  /**
   * Join all worker-threads.
   */
  void join() override;

  /**
   * Detach all worker-threads.
   */
  void detach() override;

};

/**
 * Class responsible to assign I/O tasks to specific &l:IOEventWorker; according to worker's "specialization". <br>
 * Needed in order to support full-duplex I/O mode without duplicating file-descriptors.
 */
class IOEventWorkerForeman : public Worker {
private:
  IOEventWorker m_reader;
  IOEventWorker m_writer;
public:

  /**
   * Constructor.
   */
  IOEventWorkerForeman();

  /**
   * Virtual destructor.
   */
  ~IOEventWorkerForeman() override;

  /**
   * Push list of tasks to worker.
   * @param tasks - &id:oatpp::async::utils::FastQueue; of &id:oatpp::async::CoroutineHandle;.
   */
  void pushTasks(utils::FastQueue<CoroutineHandle>& tasks) override;

  /**
   * Push one task to worker.
   * @param task - &id:CoroutineHandle;.
   */
  void pushOneTask(CoroutineHandle* task) override;

  /**
 * Break run loop.
 */
  void stop() override;

  /**
   * Join all worker-threads.
   */
  void join() override;

  /**
   * Detach all worker-threads.
   */
  void detach() override;

};

}}}

#endif //oatpp_async_worker_IOEventWorker_hpp
