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

#include "ConnectionPool.hpp"

namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionPool::Pool

void ConnectionPool::pushConnection(Pool* pool, const std::shared_ptr<IOStream>& connection, v_int32 inc) {

  {
    std::lock_guard<std::mutex> lock(pool->lock);

    if (inc >= 0) {
      pool->connections.push_back({connection, oatpp::base::Environment::getMicroTickCount()});
    } else {
      pool->size --;
    }

    if (inc > 0) {
      pool->size ++;
    }

  }

  pool->condition.notify_one();

}

std::shared_ptr<ConnectionPool::IOStream> ConnectionPool::popConnection_NON_BLOCKING(Pool* pool) {
  auto result = pool->connections.front();
  pool->connections.pop_front();
  return result.connection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionPool::ConnectionWrapper

ConnectionPool::ConnectionWrapper::ConnectionWrapper(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<Pool>& pool)
  : m_connection(connection)
  , m_pool(pool)
  , m_recycleConnection(true)
{}

ConnectionPool::ConnectionWrapper::~ConnectionWrapper() {
  if(m_recycleConnection) {
    ConnectionPool::pushConnection(m_pool.get(), m_connection, 0);
  } else {
    ConnectionPool::pushConnection(m_pool.get(), m_connection, -1);
  }
}

data::v_io_size ConnectionPool::ConnectionWrapper::write(const void *buff, data::v_io_size count) {
  return m_connection->write(buff, count);
}

data::v_io_size ConnectionPool::ConnectionWrapper::read(void *buff, data::v_io_size count) {
  return m_connection->read(buff, count);
}

oatpp::async::Action ConnectionPool::ConnectionWrapper::suggestOutputStreamAction(data::v_io_size ioResult) {
  return m_connection->suggestOutputStreamAction(ioResult);
}

oatpp::async::Action ConnectionPool::ConnectionWrapper::suggestInputStreamAction(data::v_io_size ioResult) {
  return m_connection->suggestInputStreamAction(ioResult);
}

void ConnectionPool::ConnectionWrapper::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  return m_connection->setOutputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode ConnectionPool::ConnectionWrapper::getOutputStreamIOMode() {
  return m_connection->getOutputStreamIOMode();
}

void ConnectionPool::ConnectionWrapper::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  return m_connection->setInputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode ConnectionPool::ConnectionWrapper::getInputStreamIOMode() {
  return m_connection->getInputStreamIOMode();
}

void ConnectionPool::ConnectionWrapper::invalidate() {
  m_recycleConnection = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionPool

std::shared_ptr<ConnectionPool::ConnectionWrapper> ConnectionPool::getConnection() {

  std::unique_lock<std::mutex> lock(m_pool->lock);
  while (m_pool->size >= m_maxConnections && m_pool->connections.size() == 0) {
    m_pool->condition.wait(lock);
  }

  if(m_pool->connections.size() == 0) {
    m_pool->size ++;
    return std::make_shared<ConnectionWrapper>(m_connectionProvider->getConnection(), m_pool);
  }

  return std::make_shared<ConnectionWrapper>(popConnection_NON_BLOCKING(m_pool.get()), m_pool);

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ConnectionPool::ConnectionWrapper>&> ConnectionPool::getConnectionAsync() {

  

}

void ConnectionPool::close() {

}

}}
