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

#include "ConnectionMonitor.hpp"

#include <chrono>
#include <thread>

namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionMonitor::ConnectionProxy

ConnectionMonitor::ConnectionProxy::ConnectionProxy(const std::shared_ptr<ConnectionProvider>& connectionProvider,
                                                    const std::shared_ptr<data::stream::IOStream>& connection)
  : m_connectionProvider(connectionProvider)
  , m_connection(connection)
{}

ConnectionMonitor::ConnectionProxy::~ConnectionProxy() {

}

v_io_size ConnectionMonitor::ConnectionProxy::read(void *buffer, v_buff_size count, async::Action& action) {
  return m_connection->read(buffer, count, action);
}

v_io_size ConnectionMonitor::ConnectionProxy::write(const void *data, v_buff_size count, async::Action& action) {
  return m_connection->write(data, count, action);
}

void ConnectionMonitor::ConnectionProxy::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_connection->setInputStreamIOMode(ioMode);
}
data::stream::IOMode ConnectionMonitor::ConnectionProxy::getInputStreamIOMode() {
  return m_connection->getInputStreamIOMode();
}
data::stream::Context& ConnectionMonitor::ConnectionProxy::getInputStreamContext() {
  return m_connection->getInputStreamContext();
}

void ConnectionMonitor::ConnectionProxy::setOutputStreamIOMode(data::stream::IOMode ioMode) {
  m_connection->setOutputStreamIOMode(ioMode);
}

data::stream::IOMode ConnectionMonitor::ConnectionProxy::getOutputStreamIOMode() {
  return m_connection->getOutputStreamIOMode();
}

data::stream::Context& ConnectionMonitor::ConnectionProxy::getOutputStreamContext() {
  return m_connection->getOutputStreamContext();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionMonitor

void ConnectionMonitor::monitorTask(std::shared_ptr<ConnectionMonitor> monitor) {

  while(monitor->m_running) {

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

}

}}