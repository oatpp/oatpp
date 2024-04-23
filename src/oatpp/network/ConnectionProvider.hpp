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

#ifndef oatpp_network_ConnectionProvider_hpp
#define oatpp_network_ConnectionProvider_hpp

#include "oatpp/data/share/MemoryLabel.hpp"
#include "oatpp/data/stream/Stream.hpp"
#include "oatpp/provider/Provider.hpp"

#include <unordered_map>

namespace oatpp { namespace network {

/**
 * Abstract ConnectionProvider. <br>
 * Basically it returns whatever stream (&id:oatpp::data::stream::IOStream;). <br>
 * User of ConnectionProvider should care about IOStream only.
 * All other properties are optional.
 */
class ConnectionProvider : public provider::Provider<data::stream::IOStream> {
public:

  /**
   * Predefined property key for HOST.
   */
  static const char* const PROPERTY_HOST;

  /**
   * Predefined property key for PORT.
   */
  static const char* const PROPERTY_PORT;

};
  
/**
 * No properties here. It is just a logical division
 */
class ServerConnectionProvider : virtual public ConnectionProvider {
};

/**
 * No properties here. It is just a logical division
 */
class ClientConnectionProvider : virtual public ConnectionProvider {
};
  
}}

#endif /* oatpp_network_ConnectionProvider_hpp */
