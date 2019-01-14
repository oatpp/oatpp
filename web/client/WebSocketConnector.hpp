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

#ifndef oatpp_web_client_WebSocketConnector_hpp
#define oatpp_web_client_WebSocketConnector_hpp

#include "./HttpRequestExecutor.hpp"
#include "oatpp/web/protocol/websocket/WebSocket.hpp"

namespace oatpp { namespace web { namespace client {
  
class WebSocketConnector {
public:
  typedef protocol::websocket::WebSocket WebSocket;
  typedef oatpp::data::stream::IOStream IOStream;
private:
  std::shared_ptr<oatpp::network::ClientConnectionProvider> m_connectionProvider;
  HttpRequestExecutor m_requestExecutor;
private:
  std::shared_ptr<IOStream> connectAndHandshake(const oatpp::String& path);
public:
  
  WebSocketConnector(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider)
    : m_connectionProvider(connectionProvider)
    , m_requestExecutor(connectionProvider)
  {}
  
public:
  
  static std::shared_ptr<WebSocketConnector> createShared(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider) {
    return std::make_shared<WebSocketConnector>(connectionProvider);
  }
  
  /**
   * Connect to server, do websocket-handshake and return WebSocket
   * (Blocking call)
   */
  WebSocket connect(const oatpp::String& path);
  
  /**
   * Same as connect() but return std::shared_ptr<WebSocket>
   */
  std::shared_ptr<WebSocket> connectShared(const oatpp::String& path);
  
};
  
}}}

#endif /* oatpp_web_client_WebSocketConnector_hpp */
