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

#ifndef oatpp_network_virtual__client_ConnectionProvider_hpp
#define oatpp_network_virtual__client_ConnectionProvider_hpp

#include "oatpp/network/virtual_/Interface.hpp"
#include "oatpp/network/ConnectionProvider.hpp"

namespace oatpp { namespace network { namespace virtual_ { namespace client {

/**
 * Provider of "virtual" connections for client.
 * See &id:oatpp::network::virtual_::Interface;, &id:oatpp::network::virtual_::Socket; <br>
 * Extends &id:oatpp::network::ClientConnectionProvider;.
 */
class ConnectionProvider : public oatpp::network::ClientConnectionProvider {
private:

  class ConnectionInvalidator : public provider::Invalidator<data::stream::IOStream> {
  public:

    void invalidate(const std::shared_ptr<data::stream::IOStream>& connection) override;

  };

private:
  std::shared_ptr<ConnectionInvalidator> m_invalidator;
private:
  std::shared_ptr<virtual_::Interface> m_interface;
  v_io_size m_maxAvailableToRead;
  v_io_size m_maxAvailableToWrite;
public:

  /**
   * Constructor.
   * @param interface - &id:oatpp::network::virtual_::Interface;.
   */
  ConnectionProvider(const std::shared_ptr<virtual_::Interface>& _interface);

  /**
   * Create shared ConnectionProvider.
   * @param interface - &id:oatpp::network::virtual_::Interface;.
   * @return - `std::shared_ptr` to ConnectionProvider.
   */
  static std::shared_ptr<ConnectionProvider> createShared(const std::shared_ptr<virtual_::Interface>& _interface);

  /**
   * Limit the available amount of bytes to read from socket and limit the available amount of bytes to write to socket. <br>
   * This method is used for testing purposes only.<br>
   * @param maxToRead - maximum available amount of bytes to read.
   * @param maxToWrite - maximum available amount of bytes to write.
   */
  void setSocketMaxAvailableToReadWrtie(v_io_size maxToRead, v_io_size maxToWrite) {
    m_maxAvailableToRead = maxToRead;
    m_maxAvailableToWrite = maxToWrite;
  }

  /**
   * Implementation of &id:oatpp::provider::Provider::Stop; method.
   */
  void stop() override;

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

};
  
}}}}

#endif /* oatpp_network_virtual__client_ConnectionProvider_hpp */
