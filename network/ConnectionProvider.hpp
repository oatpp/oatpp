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

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include <unordered_map>

namespace oatpp { namespace network {

/**
 * Abstract ConnectionProvider.
 * It may be anything that returns oatpp::data::stream::IOStream
 * User of ConnectionProvider should care about IOStream only.
 * All other properties are optional
 */
class ConnectionProvider {
public:
  static const char* const PROPERTY_HOST;
  static const char* const PROPERTY_PORT;
public:
  typedef oatpp::data::stream::IOStream IOStream;
  typedef oatpp::async::Action Action;
  typedef oatpp::async::Action (oatpp::async::AbstractCoroutine::*AsyncCallback)(const std::shared_ptr<IOStream>&);
private:
  std::unordered_map<oatpp::String, oatpp::String> m_properties;
protected:
  /**
   * Set optional property
   */
  void setProperty(const oatpp::String& key, const oatpp::String& value);
public:
  virtual ~ConnectionProvider() {}
  virtual std::shared_ptr<IOStream> getConnection() = 0;
  virtual Action getConnectionAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                    AsyncCallback callback) = 0;
  
  /**
   * Some optional properties that user might want to know.
   * All properties are optional and user should not rely on this
   */
  const std::unordered_map<oatpp::String, oatpp::String>& getProperties();
  
  /**
   * Get optional property
   */
  oatpp::String getProperty(const oatpp::String& key);
  
};
  
/**
 * No properties here. It is just a logical division
 */
class ServerConnectionProvider : public ConnectionProvider {
};

/**
 * No properties here. It is just a logical division
 */
class ClientConnectionProvider : public ConnectionProvider {
};
  
}}

#endif /* oatpp_netword_ConnectionsProvider_hpp */
