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

#ifndef oatpp_web_server_http2_PriorityStreamScheduler_hpp
#define oatpp_web_server_http2_PriorityStreamScheduler_hpp

#include <mutex>
#include <queue>
#include "oatpp/core/Types.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

 class PriorityStreamScheduler : public oatpp::base::Countable, public data::stream::OutputStream {

 private:
  class LockedSpinLock : public concurrency::SpinLock {
   public:
    LockedSpinLock() : SpinLock() {lock();};
  };

   class PrioritizedLockedSpinLock : public LockedSpinLock {
    private:
     v_uint32 m_prio;
    public:
     PrioritizedLockedSpinLock(v_uint32 prio) : LockedSpinLock(), m_prio(prio) {};
     bool operator<(const PrioritizedLockedSpinLock& rhs) const {
       return m_prio < rhs.m_prio;
     }
   };

  std::mutex m_queuemutex;
  std::shared_ptr<PrioritizedLockedSpinLock> m_current;
  std::priority_queue<std::shared_ptr<PrioritizedLockedSpinLock>> m_queue;
  std::shared_ptr<data::stream::OutputStream> m_stream;

 public:
  static const v_uint32 PRIORITY_MAX = UINT32_MAX;
  PriorityStreamScheduler(const std::shared_ptr<data::stream::OutputStream> &stream);

  void lock(v_uint32 priority);
  void unlock();


  v_io_size write(const void *data, v_buff_size count, async::Action &action) override;
  void setOutputStreamIOMode(data::stream::IOMode ioMode) override;
  data::stream::IOMode getOutputStreamIOMode() override;
  data::stream::Context &getOutputStreamContext() override;
 };

}}}}

#endif //oatpp_web_server_http2_PriorityStreamScheduler_hpp
