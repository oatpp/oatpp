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

#ifndef oatpp_netword_client_SimpleTCPConnectionProvider_hpp
#define oatpp_netword_client_SimpleTCPConnectionProvider_hpp

#include "../ConnectionProvider.hpp"

#include "../../../../oatpp-lib/core/src/data/stream/Stream.hpp"
#include "../../../../oatpp-lib/core/src/base/String.hpp"

namespace oatpp { namespace network { namespace client {
  
class SimpleTCPConnectionProvider : public base::Controllable, public ClientConnectionProvider {
public:
  SimpleTCPConnectionProvider(const oatpp::base::String::SharedWrapper& host, v_int32 port)
    : ClientConnectionProvider(host, port)
  {}
public:
  
  static std::shared_ptr<SimpleTCPConnectionProvider>
  createShared(const oatpp::base::String::SharedWrapper& host, v_int32 port){
    return std::shared_ptr<SimpleTCPConnectionProvider>(new SimpleTCPConnectionProvider(host, port));
  }
  
  std::shared_ptr<IOStream> getConnection() override;
  
};
  
}}}

#endif /* oatpp_netword_client_SimpleTCPConnectionProvider_hpp */
