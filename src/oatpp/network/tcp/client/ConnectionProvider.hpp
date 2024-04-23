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
#include "oatpp/provider/Invalidator.hpp"
#include "oatpp/Types.hpp"

namespace oatpp { namespace network { namespace tcp { namespace client {

/**
 * Simple provider of clinet TCP connections.
 */
class ConnectionProvider : public ClientConnectionProvider {
private:

  class ConnectionInvalidator : public provider::Invalidator<data::stream::IOStream> {
  public:

    void invalidate(const std::shared_ptr<data::stream::IOStream>& connection) override;

  };

private:
  std::shared_ptr<ConnectionInvalidator> m_invalidator;
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
   * Implements &id:oatpp::provider::TestProvider::stop;. Here does nothing.
   */
  void stop() override {
    // DO NOTHING
  }

  /**
   * Get connection.
   * @return - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   */
  provider::ResourceHandle<data::stream::IOStream> get() override;

  /**
   * Get connection in asynchronous manner.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const provider::ResourceHandle<data::stream::IOStream>&> getAsync() override;

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
