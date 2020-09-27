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

#ifndef oatpp_netword_tcp_client_ConnectionProvider_hpp
#define oatpp_netword_tcp_client_ConnectionProvider_hpp

#include "oatpp/network/Address.hpp"

#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace network { namespace tcp { namespace client {

/**
 * Simple provider of clinet TCP connections.
 */
class ConnectionProvider : public base::Countable, public ClientConnectionProvider {
protected:
  network::Address m_address;
public:
  /**
   * Constructor.
   * @param address - &id:oatpp::network::Address;.
   */
  ConnectionProvider(const network::Address& address);
public:

  /**
   * Create shared client ConnectionProvider.
   * @param address - &id:oatpp::network::Address;.
   * @return - `std::shared_ptr` to ConnectionProvider.
   */
  static std::shared_ptr<ConnectionProvider> createShared(const network::Address& address){
    return std::make_shared<ConnectionProvider>(address);
  }

  /**
   * Implements &id:oatpp::provider::Provider::stop;. Here does nothing.
   */
  void stop() override {
    // DO NOTHING
  }

  /**
   * Get connection.
   * @return - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   */
  std::shared_ptr<data::stream::IOStream> get() override;

  /**
   * Get connection in asynchronous manner.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<data::stream::IOStream>&> getAsync() override;

  /**
   * Call shutdown read and write on an underlying file descriptor.
   * `connection` **MUST** be an object previously obtained from **THIS** connection provider.
   * @param connection
   */
  void invalidate(const std::shared_ptr<data::stream::IOStream>& connection) override;

  /**
   * Get address - &id:oatpp::network::Address;.
   * @return
   */
  const network::Address& getAddress() const {
    return m_address;
  }
  
};
  
}}}}

#endif /* oatpp_netword_tcp_client_ConnectionProvider_hpp */
