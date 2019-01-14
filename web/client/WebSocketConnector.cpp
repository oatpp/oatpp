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

#include "WebSocketConnector.hpp"

#include "oatpp/web/protocol/websocket/Handshaker.hpp"

namespace oatpp { namespace web { namespace client {
  
std::shared_ptr<WebSocketConnector::IOStream> WebSocketConnector::connectAndHandshake(const oatpp::String& path) {
  
  auto connection = m_connectionProvider->getConnection();
  auto connectionHandle = std::make_shared<HttpRequestExecutor::HttpConnectionHandle>(connection);
  
  protocol::websocket::Handshaker::Headers headers;
  protocol::websocket::Handshaker::clientsideHandshake(headers);
  
  auto response = m_requestExecutor.execute("GET", path, headers, nullptr, connectionHandle);
  auto res = protocol::websocket::Handshaker::clientsideConfirmHandshake(headers, response);
  if(res == protocol::websocket::Handshaker::STATUS_OK) {
    return connection;
  } else if(res == protocol::websocket::Handshaker::STATUS_SERVER_ERROR) {
    throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAndHandshake()]: Server responded with invalid code");
  } else if(res == protocol::websocket::Handshaker::STATUS_SERVER_WRONG_KEY) {
    throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAndHandshake()]: Server wrong handshake key");
  } else if(res == protocol::websocket::Handshaker::STATUS_UNKNOWN_PROTOCOL_SUGGESTED) {
    throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAndHandshake()]: Server response contains unexpected headers");
  } else {
    throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAndHandshake()]: Unknown error");
  }
  
}
  
WebSocketConnector::WebSocket WebSocketConnector::connect(const oatpp::String& path) {
  return WebSocket(connectAndHandshake(path), true);
}
  
std::shared_ptr<WebSocketConnector::WebSocket> WebSocketConnector::connectShared(const oatpp::String& path) {
  return std::make_shared<WebSocketConnector::WebSocket>(connectAndHandshake(path), true);
}
  
}}}
