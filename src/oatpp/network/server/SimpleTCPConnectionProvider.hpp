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

#ifndef oatpp_netword_server_SimpleTCPConnectionProvider_hpp
#define oatpp_netword_server_SimpleTCPConnectionProvider_hpp

#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/network/Connection.hpp"

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace network { namespace server {

/**
 * Simple provider of TCP connections.
 */
class SimpleTCPConnectionProvider : public base::Countable, public ServerConnectionProvider {
public:

  /**
   * Connection with extra data - ex.: peer address.
   */
  class ExtendedConnection : public oatpp::network::Connection {
  public:

    static const char* const PROPERTY_PEER_ADDRESS;
    static const char* const PROPERTY_PEER_ADDRESS_FORMAT;
    static const char* const PROPERTY_PEER_PORT;

  protected:
    data::stream::DefaultInitializedContext m_context;
  public:

    /**
     * Constructor.
     * @param handle - &id:oatpp::v_io_handle;.
     * @param properties - &id:oatpp::data::stream::Context::Properties;.
     */
    ExtendedConnection(v_io_handle handle, data::stream::Context::Properties&& properties);

    /**
     * Get output stream context.
     * @return - &id:oatpp::data::stream::Context;.
     */
    oatpp::data::stream::Context& getOutputStreamContext() override;

    /**
     * Get input stream context. <br>
     * @return - &id:oatpp::data::stream::Context;.
     */
    oatpp::data::stream::Context& getInputStreamContext() override;

  };

private:
  v_uint16 m_port;
  std::atomic<bool> m_closed;
  oatpp::v_io_handle m_serverHandle;
  bool m_useExtendedConnections;
private:
  oatpp::v_io_handle instantiateServer();
private:
  bool prepareConnectionHandle(oatpp::v_io_handle handle);
  std::shared_ptr<IOStream> getDefaultConnection();
  std::shared_ptr<IOStream> getExtendedConnection();
public:

  /**
   * Constructor.
   * @param port - port to listen for incoming connections.
   * @param useExtendedConnections - set `true` to use &l:SimpleTCPConnectionProvider::ExtendedConnection;.
   * `false` to use &id:oatpp::network::Connection;.
   */
  SimpleTCPConnectionProvider(v_uint16 port, bool useExtendedConnections = false);

  /**
   * Constructor.
   * @param host - host name without schema and port. Ex.: "oatpp.io", "127.0.0.1", "localhost".
   * @param port - port to listen for incoming connections.
   * @param useExtendedConnections - set `true` to use &l:SimpleTCPConnectionProvider::ExtendedConnection;.
   * `false` to use &id:oatpp::network::Connection;.
   */
  SimpleTCPConnectionProvider(const oatpp::String& host, v_uint16 port, bool useExtendedConnections = false);
public:

  /**
   * Create shared SimpleTCPConnectionProvider.
   * @param port - port to listen for incoming connections.
   * @param port
   * @return - `std::shared_ptr` to SimpleTCPConnectionProvider.
   */
  static std::shared_ptr<SimpleTCPConnectionProvider> createShared(v_uint16 port, bool useExtendedConnections = false){
    return std::make_shared<SimpleTCPConnectionProvider>(port, useExtendedConnections);
  }

  /**
   * Create shared SimpleTCPConnectionProvider.
   * @param host - host name without schema and port. Ex.: "oatpp.io", "127.0.0.1", "localhost".
   * @param port - port to listen for incoming connections.
   * @param port
   * @return - `std::shared_ptr` to SimpleTCPConnectionProvider.
   */
    static std::shared_ptr<SimpleTCPConnectionProvider> createShared(const oatpp::String& host, v_uint16 port, bool useExtendedConnections = false){
        return std::make_shared<SimpleTCPConnectionProvider>(host, port, useExtendedConnections);
    }

  /**
   * Virtual destructor.
   */
  ~SimpleTCPConnectionProvider();

  /**
   * Close accept-socket.
   */
  void close() override;

  /**
   * Get incoming connection.
   * @return &id:oatpp::data::stream::IOStream;.
   */
  std::shared_ptr<IOStream> getConnection() override;

  /**
   * No need to implement this.<br>
   * For Asynchronous IO in oatpp it is considered to be a good practice
   * to accept connections in a seperate thread with the blocking accept()
   * and then process connections in Asynchronous manner with non-blocking read/write.
   * <br>
   * *It may be implemented later*
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::data::stream::IOStream>&> getConnectionAsync() override {
    /*
     *  No need to implement this.
     *  For Asynchronous IO in oatpp it is considered to be a good practice
     *  to accept connections in a seperate thread with the blocking accept()
     *  and then process connections in Asynchronous manner with non-blocking read/write
     *
     *  It may be implemented later
     */
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::getConnectionAsync()]: Error. Not implemented.");
  }

  /**
   * Call shutdown read and write on an underlying file descriptor.
   * `connection` **MUST** be an object previously obtained from **THIS** connection provider.
   * @param connection
   */
  void invalidateConnection(const std::shared_ptr<IOStream>& connection) override;

  /**
   * Get port.
   * @return
   */
  v_uint16 getPort(){
    return m_port;
  }
  
};
  
}}}

#endif /* oatpp_netword_server_SimpleTCPConnectionProvider_hpp */
