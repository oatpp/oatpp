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

#include "ConnectionProviderSwitch.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace network {

ConnectionProviderSwitch::ConnectionProviderSwitch(const std::shared_ptr<ConnectionProvider>& provider)
  : m_provider(provider)
{}

void ConnectionProviderSwitch::resetProvider(const std::shared_ptr<ConnectionProvider>& provider) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_provider = provider;
  m_properties = provider->getProperties();
}

std::shared_ptr<ConnectionProvider> ConnectionProviderSwitch::getCurrentProvider() {

  std::shared_ptr<ConnectionProvider> provider;

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    provider = m_provider;
  }

  if(!provider) {
    const char* const TAG = "[oatpp::network::ConnectionProviderSwitch::getCurrentProvider()]";
    const char* const msg = "Error. Can't provide connection. There is no provider set.";
    OATPP_LOGe(TAG, msg)
    throw std::runtime_error(std::string(TAG) + ": " + msg);
  }

  return provider;

}

provider::ResourceHandle<data::stream::IOStream> ConnectionProviderSwitch::get() {
  return getCurrentProvider()->get();
}


oatpp::async::CoroutineStarterForResult<const provider::ResourceHandle<data::stream::IOStream>&> ConnectionProviderSwitch::getAsync() {
  return getCurrentProvider()->getAsync();
}


void ConnectionProviderSwitch::stop() {
  return getCurrentProvider()->stop();
}

}}
