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

#include <list>
#include <condition_variable>

namespace oatpp { namespace network {

class ConnectionPool {
public:
  typedef oatpp::data::stream::IOStream IOStream;
private:

  struct ConnectionHandle {
    std::shared_ptr<IOStream> connection;
    v_int64 timestamp;
  };

private:

  class Pool {
    friend ConnectionPool;
  private:
    std::condition_variable condition;
    std::mutex lock;
    std::list<ConnectionHandle> connections;
    v_int64 size = 0;
  };

public:

  class ConnectionWrapper : public oatpp::data::stream::IOStream {
  private:
    std::shared_ptr<IOStream> m_connection;
    std::shared_ptr<Pool> m_pool;
    bool m_recycleConnection;
  public:

    ConnectionWrapper(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<Pool>& pool);
    ~ConnectionWrapper();

    data::v_io_size write(const void *buff, data::v_io_size count) override;
    data::v_io_size read(void *buff, data::v_io_size count) override;

    oatpp::async::Action suggestOutputStreamAction(data::v_io_size ioResult) override;
    oatpp::async::Action suggestInputStreamAction(data::v_io_size ioResult) override;

    void setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;
    oatpp::data::stream::IOMode getOutputStreamIOMode() override;

    void setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;
    oatpp::data::stream::IOMode getInputStreamIOMode() override;

    void invalidate();

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
  std::shared_ptr<Pool> m_pool;
  std::shared_ptr<ConnectionProvider> m_connectionProvider;
  v_int64 m_maxConnections;
  v_int64 m_maxConnectionTTL;
public:

  /**
   * Constructor.
   * @param connectionProvider
   */
  ConnectionPool(const std::shared_ptr<ConnectionProvider>& connectionProvider,
                 v_int64 maxConnections,
                 v_int64 maxConnectionTTL)
    : m_pool(std::make_shared<Pool>())
    , m_connectionProvider(connectionProvider)
    , m_maxConnections(maxConnections)
    , m_maxConnectionTTL(maxConnectionTTL)
  {}

  /**
   * Get connection.
   * This call will wait until connection is available.
   * @return
   */
  std::shared_ptr<ConnectionWrapper> getConnection();

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

};

}}

#endif // oatpp_network_ConnectionPool_hpp
