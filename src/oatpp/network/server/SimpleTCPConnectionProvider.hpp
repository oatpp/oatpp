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
#include "oatpp/core/Types.hpp"
#include "oatpp/core/os/io/Library.hpp"

namespace oatpp { namespace network { namespace server {
  
class SimpleTCPConnectionProvider : public base::Controllable, public ServerConnectionProvider {
private:
  v_word16 m_port;
  bool m_nonBlocking;
  oatpp::os::io::Library::v_handle m_serverHandle;
private:
  oatpp::os::io::Library::v_handle instantiateServer();
public:
  SimpleTCPConnectionProvider(v_word16 port, bool nonBlocking = false);
public:
  
  static std::shared_ptr<SimpleTCPConnectionProvider> createShared(v_word16 port, bool nonBlocking = false){
    return std::make_shared<SimpleTCPConnectionProvider>(port, nonBlocking);
  }
  
  ~SimpleTCPConnectionProvider() {
    oatpp::os::io::Library::handle_close(m_serverHandle);
  }
  
  std::shared_ptr<IOStream> getConnection() override;
  
  Action getConnectionAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                            AsyncCallback callback) override {
    /**
     *  No need to implement this.
     *  For Asynchronous IO in oatpp it is considered to be a good practice
     *  to accept connections in a seperate thread with the blocking accept()
     *  and then process connections in Asynchronous manner with non-blocking read/write
     *
     *  It may be implemented later
     */
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::getConnectionAsync()] not implemented.");
  }
  
  v_word16 getPort(){
    return m_port;
  }
  
};
  
}}}

#endif /* oatpp_netword_server_SimpleTCPConnectionProvider_hpp */
