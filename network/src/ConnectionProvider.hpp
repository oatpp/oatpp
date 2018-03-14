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

#ifndef oatpp_netword_ConnectionsProvider_hpp
#define oatpp_netword_ConnectionsProvider_hpp

#include "../../../oatpp-lib/core/src/data/stream/Stream.hpp"

namespace oatpp { namespace network {
  
class ServerConnectionProvider {
public:
  typedef oatpp::data::stream::IOStream IOStream;
protected:
  v_word16 m_port;
public:
  
  ServerConnectionProvider(v_word16 port)
    : m_port(port)
  {}
  
  virtual std::shared_ptr<IOStream> getConnection() = 0;
  
  v_word16 getPort(){
    return m_port;
  }
  
};
  
class ClientConnectionProvider {
public:
  typedef oatpp::data::stream::IOStream IOStream;
protected:
  oatpp::base::String::PtrWrapper m_host;
  v_word16 m_port;
public:
  
  ClientConnectionProvider(const oatpp::base::String::PtrWrapper& host, v_word16 port)
    : m_host(host)
    , m_port(port)
  {}
  
  virtual std::shared_ptr<IOStream> getConnection() = 0;
  
  oatpp::base::String::PtrWrapper getHost() {
    return m_host;
  }
  
  v_word16 getPort(){
    return m_port;
  }
  
};
  
}}

#endif /* oatpp_netword_ConnectionsProvider_hpp */
