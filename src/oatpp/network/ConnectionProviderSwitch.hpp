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

#ifndef oatpp_network_ConnectionProviderSwitch_hpp
#define oatpp_network_ConnectionProviderSwitch_hpp

#include "ConnectionProvider.hpp"
#include <mutex>

namespace oatpp { namespace network {

/**
 * ConnectionProviderSwitch can be used to hot-change connection providers.
 * Ex.: to hot-reload server certificate.
 */
class ConnectionProviderSwitch : public ServerConnectionProvider, public ClientConnectionProvider {
private:
  std::shared_ptr<ConnectionProvider> getCurrentProvider();
private:
  std::shared_ptr<ConnectionProvider> m_provider;
  std::mutex m_mutex;
public:

  /**
   * Default constructor.
   */
  ConnectionProviderSwitch() = default;

  /**
   * Constructor.
   * @param provider
   */
  ConnectionProviderSwitch(const std::shared_ptr<ConnectionProvider>& provider);

  /**
   * Reset current provider.
   * @param provider
   */
  void resetProvider(const std::shared_ptr<ConnectionProvider>& provider);

  /**
   * Get new connection.
   * @return &id:oatpp::data::stream::IOStream;.
   */
  provider::ResourceHandle<data::stream::IOStream> get() override;

  /**
   * Get new connection.
   * @return &id:oatpp::data::stream::IOStream;.
   */
  oatpp::async::CoroutineStarterForResult<const provider::ResourceHandle<data::stream::IOStream>&> getAsync() override;

  /**
   * Stop current provider.
   */
  void stop() override;

};

}}

#endif //oatpp_network_ConnectionProviderSwitch_hpp
