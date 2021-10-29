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

#include "ConnectionProvider.hpp"

#include <chrono>

namespace oatpp { namespace network { namespace virtual_ { namespace server {

void ConnectionProvider::ConnectionInvalidator::invalidate(const std::shared_ptr<data::stream::IOStream>& connection) {
  auto socket = std::static_pointer_cast<Socket>(connection);
  socket->close();
}

ConnectionProvider::ConnectionProvider(const std::shared_ptr<virtual_::Interface>& _interface)
  : m_invalidator(std::make_shared<ConnectionInvalidator>())
  , m_interface(_interface)
  , m_listenerLock(_interface->bind())
  , m_open(true)
  , m_maxAvailableToRead(-1)
  , m_maxAvailableToWrite(-1)
{
  setProperty(PROPERTY_HOST, m_interface->getName());
  setProperty(PROPERTY_PORT, "0");
}

std::shared_ptr<ConnectionProvider> ConnectionProvider::createShared(const std::shared_ptr<virtual_::Interface>& _interface) {
  return std::make_shared<ConnectionProvider>(_interface);
}

void ConnectionProvider::setSocketMaxAvailableToReadWrtie(v_io_size maxToRead, v_io_size maxToWrite) {
  m_maxAvailableToRead = maxToRead;
  m_maxAvailableToWrite = maxToWrite;
}

void ConnectionProvider::stop() {
  m_open = false;
  m_listenerLock.reset();
  m_interface->notifyAcceptors();
}

provider::ResourceHandle<data::stream::IOStream> ConnectionProvider::get() {
  auto socket = m_interface->accept(m_open, std::chrono::milliseconds(500));
  if(socket) {
    socket->setMaxAvailableToReadWrtie(m_maxAvailableToRead, m_maxAvailableToWrite);
  }
  return provider::ResourceHandle<data::stream::IOStream>(socket, m_invalidator);
}

}}}}
