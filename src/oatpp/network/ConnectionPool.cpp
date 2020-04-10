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

#include <thread>
#include <chrono>

namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionPool::Pool

void ConnectionPool::Pool::onNewItem(oatpp::async::CoroutineWaitList& list) {

  std::lock_guard<std::mutex> lockGuard(lock);

  if(!isOpen) {
    list.notifyAll();
    return;
  }

  if(size < maxConnections || connections.size() > 0) {
    list.notifyFirst();
  }

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

v_io_size ConnectionPool::ConnectionWrapper::write(const void *buff, v_buff_size count, async::Action& action) {
  return m_connection->write(buff, count, action);
}

v_io_size ConnectionPool::ConnectionWrapper::read(void *buff, v_buff_size count, async::Action& action) {
  return m_connection->read(buff, count, action);
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

oatpp::data::stream::Context& ConnectionPool::ConnectionWrapper::getOutputStreamContext() {
  return m_connection->getOutputStreamContext();
}

oatpp::data::stream::Context& ConnectionPool::ConnectionWrapper::getInputStreamContext() {
  return m_connection->getInputStreamContext();
}

void ConnectionPool::ConnectionWrapper::invalidate() {
  m_recycleConnection = false;
}

bool ConnectionPool::ConnectionWrapper::isValid() {
  return m_connection && m_recycleConnection;
}

const std::shared_ptr<data::stream::IOStream>& ConnectionPool::ConnectionWrapper::getUnderlyingConnection() {
  return m_connection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionPool

ConnectionPool::ConnectionPool(const std::shared_ptr<ConnectionProvider>& connectionProvider,
               v_int64 maxConnections,
               const std::chrono::duration<v_int64, std::micro>& maxConnectionTTL)
  : m_pool(std::make_shared<Pool>(maxConnections, maxConnectionTTL.count()))
  , m_connectionProvider(connectionProvider)
{

  std::thread poolCleanupTask(cleanupTask, m_pool);
  poolCleanupTask.detach();

}

ConnectionPool::~ConnectionPool() {
  close();
}

void ConnectionPool::cleanupTask(std::shared_ptr<Pool> pool) {

  while(pool->isOpen) {

    {

      std::lock_guard<std::mutex> lock(pool->lock);
      auto ticks = oatpp::base::Environment::getMicroTickCount();

      std::list<ConnectionHandle>::iterator i = pool->connections.begin();
      while (i != pool->connections.end()) {

        if(ticks - i->timestamp > pool->maxConnectionTTL) {
          i = pool->connections.erase(i);
          pool->size --;
        } else {
          i ++;
        }

      }

    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  }

}

void ConnectionPool::pushConnection(Pool* pool, const std::shared_ptr<IOStream>& connection, v_int32 inc) {

  {
    std::lock_guard<std::mutex> lock(pool->lock);

    if(!pool->isOpen) {
      pool->size --;
      return;
    }

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
  pool->waitList.notifyFirst();

}

std::shared_ptr<ConnectionPool::IOStream> ConnectionPool::popConnection_NON_BLOCKING(Pool* pool) {
  auto result = pool->connections.front();
  pool->connections.pop_front();
  return result.connection;
}

std::shared_ptr<ConnectionProvider> ConnectionPool::getConnectionProvider() {
  return m_connectionProvider;
}

std::shared_ptr<ConnectionPool::Pool> ConnectionPool::getPoolInstance() {
  return m_pool;
}

std::shared_ptr<ConnectionPool::ConnectionWrapper> ConnectionPool::getConnection() {

  {

    std::unique_lock<std::mutex> lock(m_pool->lock);

    while (m_pool->isOpen && m_pool->size >= m_pool->maxConnections && m_pool->connections.size() == 0) {
      m_pool->condition.wait(lock);
    }

    if(!m_pool->isOpen) {
      return nullptr;
    }

    if (m_pool->connections.size() > 0) {
      return std::make_shared<ConnectionWrapper>(popConnection_NON_BLOCKING(m_pool.get()), m_pool);
    } else {
      m_pool->size ++;
    }

  }

  return std::make_shared<ConnectionWrapper>(m_connectionProvider->getConnection(), m_pool);

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ConnectionPool::ConnectionWrapper>&>
ConnectionPool::getConnectionAsync(const std::shared_ptr<ConnectionProvider>& connectionProvider, const std::shared_ptr<Pool>& poolInstance) {

  class ConnectionCoroutine : public oatpp::async::CoroutineWithResult<ConnectionCoroutine, const std::shared_ptr<ConnectionPool::ConnectionWrapper>&> {
  private:
    std::shared_ptr<Pool> m_pool;
    std::shared_ptr<ConnectionProvider> m_connectionProvider;
    bool m_wasInc;
  public:

    ConnectionCoroutine(const std::shared_ptr<Pool>& pool,
                        const std::shared_ptr<ConnectionProvider>& connectionProvider)
      : m_pool(pool)
      , m_connectionProvider(connectionProvider)
      , m_wasInc(false)
    {}

    Action act() override {

      {
        /* Careful!!! Using non-async lock */
        std::unique_lock<std::mutex> lock(m_pool->lock);

        if (m_pool->isOpen && m_pool->size >= m_pool->maxConnections && m_pool->connections.size() == 0) {
          lock.unlock();
          return Action::createWaitListAction(&m_pool->waitList);
        }

        if(!m_pool->isOpen) {
          lock.unlock();
          return _return(nullptr);
        }

        if (m_pool->connections.size() > 0) {
          auto connection = std::make_shared<ConnectionWrapper>(popConnection_NON_BLOCKING(m_pool.get()), m_pool);
          lock.unlock();
          return _return(connection);
        } else {
          m_pool->size ++;
          m_wasInc = true;
        }

      }

      return m_connectionProvider->getConnectionAsync().callbackTo(&ConnectionCoroutine::onConnection);

    }

    Action onConnection(const std::shared_ptr<IOStream>& connection) {
      return _return(std::make_shared<ConnectionWrapper>(connection, m_pool));
    }

    Action handleError(oatpp::async::Error* error) override {
      if(m_wasInc) {
        /* Careful!!! Using non-async lock */
        std::lock_guard<std::mutex> lock(m_pool->lock);
        m_pool->size --;
      }
      return error;
    }

  };

  return ConnectionCoroutine::startForResult(poolInstance, connectionProvider);

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ConnectionPool::ConnectionWrapper>&> ConnectionPool::getConnectionAsync() {
  return getConnectionAsync(m_connectionProvider, m_pool);
}

void ConnectionPool::close() {

  {
    std::lock_guard<std::mutex> lock(m_pool->lock);

    if (!m_pool->isOpen) {
      return;
    }

    m_pool->isOpen = false;
    auto size = m_pool->connections.size();
    m_pool->connections.clear();
    m_pool->size -= size;
  }

  m_pool->condition.notify_all();
  m_pool->waitList.notifyAll();

}

void ConnectionPool::invalidateConnection(const std::shared_ptr<IOStream>& connection) {
  auto wrapper = std::static_pointer_cast<ConnectionPool::ConnectionWrapper>(connection);
  wrapper->invalidate();
  auto c = wrapper->getUnderlyingConnection();
  m_connectionProvider->invalidateConnection(c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ServerConnectionPool

ServerConnectionPool::ServerConnectionPool(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
                                           v_int64 maxConnections,
                                           const std::chrono::duration<v_int64, std::micro>& maxConnectionTTL)
  : m_pool(connectionProvider, maxConnections, maxConnectionTTL)
{

  setProperty(PROPERTY_HOST, connectionProvider->getProperty(PROPERTY_HOST).toString());
  setProperty(PROPERTY_PORT, connectionProvider->getProperty(PROPERTY_PORT).toString());

}

std::shared_ptr<oatpp::data::stream::IOStream> ServerConnectionPool::getConnection() {
  return m_pool.getConnection();
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::data::stream::IOStream>&> ServerConnectionPool::getConnectionAsync() {

  class ConnectionCoroutine : public oatpp::async::CoroutineWithResult<ConnectionCoroutine, const std::shared_ptr<IOStream>&> {
  private:
    std::shared_ptr<ConnectionPool::Pool> m_pool;
    std::shared_ptr<ConnectionProvider> m_connectionProvider;
  public:

    ConnectionCoroutine(const std::shared_ptr<ConnectionPool::Pool>& pool,
                        const std::shared_ptr<ConnectionProvider>& connectionProvider)
      : m_pool(pool)
      , m_connectionProvider(connectionProvider)
    {}

    Action act() override {
      return ConnectionPool::getConnectionAsync(m_connectionProvider, m_pool).callbackTo(&ConnectionCoroutine::onConnection);
    }

    Action onConnection(const std::shared_ptr<ConnectionPool::ConnectionWrapper>& connectionWrapper) {
      return _return(connectionWrapper);
    }


  };

  return ConnectionCoroutine::startForResult(m_pool.getPoolInstance(), m_pool.getConnectionProvider());

}

void ServerConnectionPool::invalidateConnection(const std::shared_ptr<IOStream>& connection) {
  m_pool.invalidateConnection(connection);
}

void ServerConnectionPool::close() {
  m_pool.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ClientConnectionPool

ClientConnectionPool::ClientConnectionPool(const std::shared_ptr<ClientConnectionProvider>& connectionProvider,
                                           v_int64 maxConnections,
                                           const std::chrono::duration<v_int64, std::micro>& maxConnectionTTL)
  : m_pool(connectionProvider, maxConnections, maxConnectionTTL)
{

  setProperty(PROPERTY_HOST, connectionProvider->getProperty(PROPERTY_HOST).toString());
  setProperty(PROPERTY_PORT, connectionProvider->getProperty(PROPERTY_PORT).toString());

}

std::shared_ptr<oatpp::data::stream::IOStream> ClientConnectionPool::getConnection() {
  return m_pool.getConnection();
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::data::stream::IOStream>&> ClientConnectionPool::getConnectionAsync() {

  class ConnectionCoroutine : public oatpp::async::CoroutineWithResult<ConnectionCoroutine, const std::shared_ptr<IOStream>&> {
  private:
    std::shared_ptr<ConnectionPool::Pool> m_pool;
    std::shared_ptr<ConnectionProvider> m_connectionProvider;
  public:

    ConnectionCoroutine(const std::shared_ptr<ConnectionPool::Pool>& pool,
                        const std::shared_ptr<ConnectionProvider>& connectionProvider)
      : m_pool(pool)
      , m_connectionProvider(connectionProvider)
    {}

    Action act() override {
      return ConnectionPool::getConnectionAsync(m_connectionProvider, m_pool).callbackTo(&ConnectionCoroutine::onConnection);
    }

    Action onConnection(const std::shared_ptr<ConnectionPool::ConnectionWrapper>& connectionWrapper) {
      return _return(connectionWrapper);
    }


  };

  return ConnectionCoroutine::startForResult(m_pool.getPoolInstance(), m_pool.getConnectionProvider());

}

void ClientConnectionPool::invalidateConnection(const std::shared_ptr<IOStream>& connection) {
  m_pool.invalidateConnection(connection);
}

void ClientConnectionPool::close() {
  m_pool.close();
}

}}
