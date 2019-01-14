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
  
std::shared_ptr<WebSocketConnector::Connection> WebSocketConnector::connect(const oatpp::String& path) {
  
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
  
oatpp::async::Action WebSocketConnector::connectAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                      AsyncCallback callback,
                                                      const oatpp::String& path)
{
  
  class ConnectCoroutine : public oatpp::async::CoroutineWithResult<ConnectCoroutine, const std::shared_ptr<Connection>&> {
  private:
    std::shared_ptr<oatpp::network::ClientConnectionProvider> m_connectionProvider;
    HttpRequestExecutor m_requestExecutor;
    oatpp::String m_path;
    std::shared_ptr<Connection> m_connection;
    protocol::websocket::Handshaker::Headers m_handshakeHeaders;
  public:
    
    ConnectCoroutine(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider,
                     const HttpRequestExecutor& requestExecutor,
                     const oatpp::String path)
      : m_connectionProvider(connectionProvider)
      , m_requestExecutor(requestExecutor)
      , m_path(path)
    {}
    
    Action act() override {
      oatpp::network::ClientConnectionProvider::AsyncCallback callback =
      static_cast<oatpp::network::ClientConnectionProvider::AsyncCallback>(&ConnectCoroutine::onConnected);
      return m_connectionProvider->getConnectionAsync(this, callback);
    }
    
    Action onConnected(const std::shared_ptr<Connection>& connection) {
      
      m_connection = connection;
      
      auto connectionHandle = std::make_shared<HttpRequestExecutor::HttpConnectionHandle>(m_connection);
      m_handshakeHeaders.clear();
      protocol::websocket::Handshaker::clientsideHandshake(m_handshakeHeaders);
      
      oatpp::web::client::RequestExecutor::AsyncCallback callback =
      static_cast<oatpp::web::client::RequestExecutor::AsyncCallback>(&ConnectCoroutine::onServerResponse);
      return m_requestExecutor.executeAsync(this, callback, "GET", m_path, m_handshakeHeaders, nullptr, connectionHandle);
      
    }
    
    Action onServerResponse(const std::shared_ptr<protocol::http::incoming::Response>& response) {
      auto res = protocol::websocket::Handshaker::clientsideConfirmHandshake(m_handshakeHeaders, response);
      if(res == protocol::websocket::Handshaker::STATUS_OK) {
        return _return(m_connection);
      } else if(res == protocol::websocket::Handshaker::STATUS_SERVER_ERROR) {
        throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAsync(){ConnectCoroutine}]: Server responded with invalid code");
      } else if(res == protocol::websocket::Handshaker::STATUS_SERVER_WRONG_KEY) {
        throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAsync(){ConnectCoroutine}]: Server wrong handshake key");
      } else if(res == protocol::websocket::Handshaker::STATUS_UNKNOWN_PROTOCOL_SUGGESTED) {
        throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAsync(){ConnectCoroutine}]: Server response contains unexpected headers");
      } else {
        throw std::runtime_error("[oatpp::web::client::WebSocketConnector::connectAsync(){ConnectCoroutine}]: Unknown error");
      }
    }
    
  };
  
  return parentCoroutine->startCoroutineForResult<ConnectCoroutine>(callback, m_connectionProvider, m_requestExecutor, path);
  
}
  
}}}
