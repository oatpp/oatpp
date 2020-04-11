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

#ifndef oatpp_network_ConnectionPool_hpp
#define oatpp_network_ConnectionPool_hpp

#include "ConnectionProvider.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/async/CoroutineWaitList.hpp"

#include <list>
#include <condition_variable>
#include <chrono>

namespace oatpp { namespace network {

/**
 * Connection Pool.
 */
class ConnectionPool {
public:
  typedef oatpp::data::stream::IOStream IOStream;
private:

  struct ConnectionHandle {
    std::shared_ptr<IOStream> connection;
    v_int64 timestamp;
  };

public:

  class Pool : private oatpp::async::CoroutineWaitList::Listener, public oatpp::base::Countable {
    friend ConnectionPool;
  private:
    void onNewItem(oatpp::async::CoroutineWaitList& list) override;
  private:
    oatpp::async::CoroutineWaitList waitList;
    std::condition_variable condition;
    std::mutex lock;
    std::list<ConnectionHandle> connections;
    v_int64 size = 0;
    v_int64 maxConnections;
    v_int64 maxConnectionTTL;
  private:
    std::atomic<bool> isOpen;
  public:
    Pool(v_int64 pMaxConnections, v_int64 pMmaxConnectionTTL)
      : maxConnections(pMaxConnections)
      , maxConnectionTTL(pMmaxConnectionTTL)
      , isOpen(true)
    {}
  };

public:

  /**
   * Wrapper over &id:oatpp::data::stream::IOStream;.
   * Will acquire connection from the pool on initialization and will return connection to the pool on destruction.
   */
  class ConnectionWrapper : public oatpp::data::stream::IOStream {
    friend ConnectionPool;
  private:
    std::shared_ptr<IOStream> m_connection;
    std::shared_ptr<Pool> m_pool;
    bool m_recycleConnection;
  private:
    void invalidate();
  public:

    ConnectionWrapper(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<Pool>& pool);
    ~ConnectionWrapper();

    v_io_size write(const void *buff, v_buff_size count, async::Action& action) override;
    v_io_size read(void *buff, v_buff_size count, async::Action& action) override;

    void setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;
    oatpp::data::stream::IOMode getOutputStreamIOMode() override;

    void setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;
    oatpp::data::stream::IOMode getInputStreamIOMode() override;

    oatpp::data::stream::Context& getOutputStreamContext() override;
    oatpp::data::stream::Context& getInputStreamContext() override;

    /**
     * Check if connection is still valid.
     * @return
     */
    bool isValid();

    /**
     * Get underlying connection.
     * @return
     */
    const std::shared_ptr<IOStream>& getUnderlyingConnection();

  };

private:

  /*
   * Push connection to Pool.
   * @param connection
   * @param inc - [-1, 0, 1].
   * -1 - decrease size (connection invalidated).
   *  0 - size not changed (connection is returned to pool).
   * +1 - inc size (new connection).
   */
  static void pushConnection(Pool* pool, const std::shared_ptr<IOStream>& connection, v_int32 inc);

  /*
   * Get connection from pool.
   * @return
   */
  static std::shared_ptr<IOStream> popConnection_NON_BLOCKING(Pool* pool);

private:
  static void cleanupTask(std::shared_ptr<Pool> pool);
private:
  std::shared_ptr<Pool> m_pool;
  std::shared_ptr<ConnectionProvider> m_connectionProvider;
public:

  /**
   * Constructor.
   * @param connectionProvider - underlying connection provider.
   * @param maxConnections - maximum number of allowed connections in the pool.
   * @param maxConnectionTTL - maximum time that unused connection is allowed to live in the pool.
   */
  ConnectionPool(const std::shared_ptr<ConnectionProvider>& connectionProvider,
                 v_int64 maxConnections,
                 const std::chrono::duration<v_int64, std::micro>& maxConnectionTTL);

  /**
   * Destructor.
   * Calls &l:ConnectionPool::close ();.
   */
  ~ConnectionPool();

  /**
   * Get underlying connection provider.
   * @return - `std::shared_ptr` to &id:oatpp::network::ConnectionProvider;.
   */
  std::shared_ptr<ConnectionProvider> getConnectionProvider();

  /**
   * Get pool instance.
   * @return
   */
  std::shared_ptr<Pool> getPoolInstance();

  /**
   * Get connection.
   * This call will wait until connection is available.
   * @return
   */
  std::shared_ptr<ConnectionWrapper> getConnection();

  /**
   * Get connection in Async manner.
   * @param connectionProvider
   * @param poolInstance
   * @return
   */
  static oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ConnectionWrapper>&>
  getConnectionAsync(const std::shared_ptr<ConnectionProvider>& connectionProvider, const std::shared_ptr<Pool>& poolInstance);

  /**
   * Get connection in Async manner.
   * This call will wait until connection is available.
   * @return
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ConnectionWrapper>&> getConnectionAsync();

  /**
   * Close all connections.
   * All free connections that are currently in the pool are closed immediately.
   * Other connections are closed once returned to the pool.
   */
  void close();

  /**
   * Invalidate connection that was previously obtain by a call to `getConnection()` or `getConnectionAsync()`.
   * @param connection - **MUST** be instance of `&l:ConnectionPool::ConnectionWrapper;` or its subclass.
   */
  void invalidateConnection(const std::shared_ptr<IOStream>& connection);

};

/**
 * &id:oatpp::network::ServerConnectionProvider; based on &l:ConnectionPool;.
 */
class ServerConnectionPool : public ServerConnectionProvider {
private:
  ConnectionPool m_pool;
public:

  /**
   * Constructor.
   * @param connectionProvider - underlying connection provider.
   * @param maxConnections - maximum number of allowed connections in the pool.
   * @param maxConnectionTTL - maximum time that unused connection is allowed to live in the pool.
   */
  ServerConnectionPool(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
                       v_int64 maxConnections,
                       const std::chrono::duration<v_int64, std::micro>& maxConnectionTTL);

  /**
   * Get connection.
   * This call will wait until connection is available.
   * @return
   */
  std::shared_ptr<IOStream> getConnection() override;

  /**
   * Get connection in Async manner.
   * This call will wait until connection is available.
   * @return
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<IOStream>&> getConnectionAsync() override;

  /**
   * Invalidate connection that was previously created by this provider.
   * Ex.: if provider is pool based - you can signal that this connection should not be reused anymore.
   * @param connection
   */
  void invalidateConnection(const std::shared_ptr<IOStream>& connection) override;

  /**
   * Close pool.
   */
  void close() override;

};

/**
 * &id:oatpp::network::ClientConnectionProvider; based on &l:ConnectionPool;.
 */
class ClientConnectionPool : public ClientConnectionProvider {
private:
  ConnectionPool m_pool;
public:

  /**
   * Constructor.
   * @param connectionProvider - underlying connection provider.
   * @param maxConnections - maximum number of allowed connections in the pool.
   * @param maxConnectionTTL - maximum time that unused connection is allowed to live in the pool.
   */
  ClientConnectionPool(const std::shared_ptr<ClientConnectionProvider>& connectionProvider,
                       v_int64 maxConnections,
                       const std::chrono::duration<v_int64, std::micro>& maxConnectionTTL);

  /**
   * Get connection.
   * This call will wait until connection is available.
   * @return
   */
  std::shared_ptr<IOStream> getConnection() override;

  /**
   * Get connection in Async manner.
   * This call will wait until connection is available.
   * @return
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<IOStream>&> getConnectionAsync() override;

  /**
   * Invalidate connection that was previously created by this provider.
   * Ex.: if provider is pool based - you can signal that this connection should not be reused anymore.
   * @param connection
   */
  void invalidateConnection(const std::shared_ptr<IOStream>& connection) override;

  /**
   * Close pool.
   */
  void close() override;

};

}}

#endif // oatpp_network_ConnectionPool_hpp
