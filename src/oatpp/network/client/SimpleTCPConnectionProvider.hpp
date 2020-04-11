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

#ifndef oatpp_netword_client_SimpleTCPConnectionProvider_hpp
#define oatpp_netword_client_SimpleTCPConnectionProvider_hpp

#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace network { namespace client {

/**
 * Simple provider of clinet TCP connections.
 */
class SimpleTCPConnectionProvider : public base::Countable, public ClientConnectionProvider {
protected:
  oatpp::String m_host;
  v_uint16 m_port;
public:
  /**
   * Constructor.
   * @param host - host name without schema and port. Ex.: "oatpp.io", "127.0.0.1", "localhost".
   * @param port - server port.
   */
  SimpleTCPConnectionProvider(const oatpp::String& host, v_uint16 port);
public:

  /**
   * Create shared client SimpleTCPConnectionProvider.
   * @param host - host name without schema and port. Ex.: "oatpp.io", "127.0.0.1", "localhost".
   * @param port - server port.
   * @return - `std::shared_ptr` to SimpleTCPConnectionProvider.
   */
  static std::shared_ptr<SimpleTCPConnectionProvider> createShared(const oatpp::String& host, v_uint16 port){
    return std::make_shared<SimpleTCPConnectionProvider>(host, port);
  }

  /**
   * Implements &id:oatpp::network::ConnectionProvider::close;. Here does nothing.
   */
  void close() override {
    // DO NOTHING
  }

  /**
   * Get connection.
   * @return - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   */
  std::shared_ptr<IOStream> getConnection() override;

  /**
   * Get connection in asynchronous manner.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::data::stream::IOStream>&> getConnectionAsync() override;

  /**
   * Call shutdown read and write on an underlying file descriptor.
   * `connection` **MUST** be an object previously obtained from **THIS** connection provider.
   * @param connection
   */
  void invalidateConnection(const std::shared_ptr<IOStream>& connection) override;

  /**
   * Get host name.
   * @return - host name.
   */
  oatpp::String getHost() {
    return m_host;
  }

  /**
   * Get port.
   * @return - port.
   */
  v_uint16 getPort(){
    return m_port;
  }
  
};
  
}}}

#endif /* oatpp_netword_client_SimpleTCPConnectionProvider_hpp */
