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

#ifndef oatpp_web_protocol_http_outgoing_Response_hpp
#define oatpp_web_protocol_http_outgoing_Response_hpp

#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/network/server/ConnectionHandler.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class Response : public oatpp::base::Controllable {
public:
  typedef http::Protocol::Headers Headers;
public:
  OBJECT_POOL(Outgoing_Response_Pool, Response, 32)
  SHARED_OBJECT_POOL(Shared_Outgoing_Response_Pool, Response, 32)
private:
  Status m_status;
  Headers m_headers;
  std::shared_ptr<Body> m_body;
  std::shared_ptr<oatpp::network::server::ConnectionHandler> m_connectionUpgradeHandler;
public:
  Response(const Status& status,
           const std::shared_ptr<Body>& body)
    : m_status(status)
    , m_body(body)
  {}
public:
  
  static std::shared_ptr<Response> createShared(const Status& status,
                                                const std::shared_ptr<Body>& body) {
    return Shared_Outgoing_Response_Pool::allocateShared(status, body);
  }
  
  const Status& getStatus() const {
    return m_status;
  }
  
  Headers& getHeaders() {
    return m_headers;
  }
  
  void putHeader(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
    m_headers[key] = value;
  }
  
  bool putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()) {
      m_headers.insert({key, value});
      return true;
    }
    return false;
  }
  
  void setConnectionUpgradeHandler(const std::shared_ptr<oatpp::network::server::ConnectionHandler>& handler) {
    m_connectionUpgradeHandler = handler;
  }
  
  std::shared_ptr<oatpp::network::server::ConnectionHandler> getConnectionUpgradeHandler() {
    return m_connectionUpgradeHandler;
  }
  
  void send(const std::shared_ptr<data::stream::OutputStream>& stream);
  
  oatpp::async::Action sendAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                  const oatpp::async::Action& actionOnFinish,
                                  const std::shared_ptr<data::stream::OutputStream>& stream);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Response_hpp */
