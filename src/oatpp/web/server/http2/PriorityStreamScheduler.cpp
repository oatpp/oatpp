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

#include "PriorityStreamScheduler.hpp"

oatpp::web::server::http2::PriorityStreamScheduler::PriorityStreamScheduler(std::shared_ptr<data::stream::OutputStream> stream) {

}

void oatpp::web::server::http2::PriorityStreamScheduler::lock(v_uint32 priority) {
  m_queuemutex.lock();
  auto plsl = std::make_shared<PrioritizedLockedSpinLock>(priority);
  if (m_current == nullptr) {
    m_current = plsl;
    m_queuemutex.unlock();
  } else {
    m_queue.push(plsl);
    m_queuemutex.unlock();
    plsl->lock();
  }
}

void oatpp::web::server::http2::PriorityStreamScheduler::unlock() {
  std::lock_guard<std::mutex> lg(m_queuemutex);
  m_current = nullptr;
  if (!m_queue.empty()) {
    m_current = m_queue.top();
    m_queue.pop();
    m_current->unlock();
  }
}

oatpp::v_io_size oatpp::web::server::http2::PriorityStreamScheduler::write(const void *data,
                                                                           v_buff_size count,
                                                                           oatpp::async::Action &action) {
  return m_stream->write(data, count, action);
}


void oatpp::web::server::http2::PriorityStreamScheduler::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_stream->setOutputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode oatpp::web::server::http2::PriorityStreamScheduler::getOutputStreamIOMode() {
  return m_stream->getOutputStreamIOMode();
}

oatpp::data::stream::Context &oatpp::web::server::http2::PriorityStreamScheduler::getOutputStreamContext() {
  return m_stream->getOutputStreamContext();
}
