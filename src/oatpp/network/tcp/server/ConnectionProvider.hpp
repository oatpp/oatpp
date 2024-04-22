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

#ifndef oatpp_network_tcp_server_ConnectionProvider_hpp
#define oatpp_network_tcp_server_ConnectionProvider_hpp

#include "oatpp/network/Address.hpp"
#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/network/tcp/Connection.hpp"
#include "oatpp/network/tcp/ConnectionConfigurer.hpp"

#include "oatpp/Types.hpp"

namespace oatpp { namespace network { namespace tcp { namespace server {

/**
 * Simple provider of TCP connections.
 */
class ConnectionProvider : public ServerConnectionProvider {
private:

  class ConnectionInvalidator : public provider::Invalidator<data::stream::IOStream> {
  public:

    void invalidate(const std::shared_ptr<data::stream::IOStream>& connection) override;

  };

public:

  /**
   * Connection with extra data - ex.: peer address.
   */
  class ExtendedConnection : public oatpp::network::tcp::Connection {
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
  std::shared_ptr<ConnectionInvalidator> m_invalidator;
  network::Address m_address;
  std::atomic<bool> m_closed;
  oatpp::v_io_handle m_serverHandle;
  bool m_useExtendedConnections;
  std::shared_ptr<ConnectionConfigurer> m_connectionConfigurer;
private:
  oatpp::v_io_handle instantiateServer();
private:
  void prepareConnectionHandle(oatpp::v_io_handle handle);
  provider::ResourceHandle<data::stream::IOStream> getDefaultConnection();
  provider::ResourceHandle<data::stream::IOStream> getExtendedConnection();
public:

  /**
   * Constructor.
   * @param address - &id:oatpp::network::Address;.
   * @param useExtendedConnections - set `true` to use &l:ConnectionProvider::ExtendedConnection;.
   * `false` to use &id:oatpp::network::tcp::Connection;.
   */
  ConnectionProvider(const network::Address& address, bool useExtendedConnections = false);

public:

  /**
   * Create shared ConnectionProvider.
   * @param address - &id:oatpp::network::Address;.
   * @param useExtendedConnections - set `true` to use &l:ConnectionProvider::ExtendedConnection;.
   * `false` to use &id:oatpp::network::tcp::Connection;.
   * @return - `std::shared_ptr` to ConnectionProvider.
   */
  static std::shared_ptr<ConnectionProvider> createShared(const network::Address& address, bool useExtendedConnections = false){
    return std::make_shared<ConnectionProvider>(address, useExtendedConnections);
  }

  /**
   * Set connection configurer.
   * @param connectionConfigurer
   */
  void setConnectionConfigurer(const std::shared_ptr<ConnectionConfigurer>& connectionConfigurer);

  /**
   * Virtual destructor.
   */
  ~ConnectionProvider() override;

  /**
   * Close accept-socket.
   */
  void stop() override;

  /**
   * Get incoming connection.
   * @return &id:oatpp::data::stream::IOStream;.
   */
  provider::ResourceHandle<data::stream::IOStream> get() override;

  /**
   * No need to implement this.<br>
   * For Asynchronous IO in oatpp it is considered to be a good practice
   * to accept connections in a seperate thread with the blocking accept()
   * and then process connections in Asynchronous manner with non-blocking read/write.
   * <br>
   * *It may be implemented later*
   */
  oatpp::async::CoroutineStarterForResult<const provider::ResourceHandle<data::stream::IOStream>&> getAsync() override {
    /*
     *  No need to implement this.
     *  For Asynchronous IO in oatpp it is considered to be a good practice
     *  to accept connections in a seperate thread with the blocking accept()
     *  and then process connections in Asynchronous manner with non-blocking read/write
     *
     *  It may be implemented later
     */
    throw std::runtime_error("[oatpp::network::tcp::server::ConnectionProvider::getAsync()]: Error. Not implemented.");
  }

  /**
   * Get address - &id:oatpp::network::Address;.
   * @return
   */
  const network::Address& getAddress() const {
    return m_address;
  }
  
};
  
}}}}

#endif /* oatpp_netword_tcp_server_ConnectionProvider_hpp */
