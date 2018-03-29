/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/base/String.hpp"
#include "oatpp/core/os/io/Library.hpp"

namespace oatpp { namespace network { namespace server {
  
class SimpleTCPConnectionProvider : public base::Controllable, public ServerConnectionProvider {
private:
  oatpp::os::io::Library::v_handle m_serverHandle;
  bool m_nonBlocking;
private:
  oatpp::os::io::Library::v_handle instantiateServer();
public:
  SimpleTCPConnectionProvider(v_word16 port, bool nonBlocking = false)
    : ServerConnectionProvider(port)
    , m_nonBlocking(nonBlocking)
  {
    m_serverHandle = instantiateServer();
  }
public:
  
  static std::shared_ptr<SimpleTCPConnectionProvider> createShared(v_word16 port, bool nonBlocking = false){
    return std::shared_ptr<SimpleTCPConnectionProvider>(new SimpleTCPConnectionProvider(port, nonBlocking));
  }
  
  std::shared_ptr<IOStream> getConnection() override;
  
};
  
}}}

#endif /* oatpp_netword_server_SimpleTCPConnectionProvider_hpp */
