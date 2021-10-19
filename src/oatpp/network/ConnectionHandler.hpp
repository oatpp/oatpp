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

#ifndef oatpp_network_ConnectionHandler_hpp
#define oatpp_network_ConnectionHandler_hpp

#include "oatpp/core/provider/Provider.hpp"
#include "oatpp/core/data/stream/Stream.hpp"
#include <unordered_map>

namespace oatpp { namespace network {

/**
 * Abstract ConnectionHandler.
 */
class ConnectionHandler {
public:
  /**
   * Convenience typedef for &id:oatpp::data::stream::IOStream;.
   */
  typedef oatpp::data::stream::IOStream IOStream;

  /**
   * Convenience typedef for accompanying parameters of connection handling.
   */
  typedef std::unordered_map<oatpp::String, oatpp::String> ParameterMap;
public:

  /**
   * Virtual Destructor.
   */
  virtual ~ConnectionHandler() = default;

  /**
   * Handle provided connection.
   * @param connectionData - see &id:oatpp::data::stream::IOStream;.
   * @param params - accompanying parameters.
   */
  virtual void handleConnection(const provider::ResourceHandle<IOStream>& connectionData,
                                const std::shared_ptr<const ParameterMap>& params) = 0;

  /**
   * Stop all threads here
   */
  virtual void stop() = 0;
};
  
}}

#endif /* oatpp_network_ConnectionHandler_hpp */
