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

#ifndef oatpp_web_protocol_websocket_Handshaker_hpp
#define oatpp_web_protocol_websocket_Handshaker_hpp

#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Request.hpp"
#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
class Handshaker {
public:
  static const char* const MAGIC_UUID;
public:
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
  typedef oatpp::web::protocol::http::outgoing::Request OutgoingRequest;
  typedef oatpp::web::protocol::http::incoming::Response IncomingResponse;
  typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
  typedef oatpp::web::protocol::http::Protocol::Headers Headers;
  typedef oatpp::network::server::ConnectionHandler ConnectionHandler;
private:
  static oatpp::String getHeader(const Headers& headers, const oatpp::data::share::StringKeyLabelCI_FAST& key);
public:
  
  static std::shared_ptr<OutgoingResponse> serversideHandshake(const Headers& headers, const std::shared_ptr<ConnectionHandler>& connectionUpgradeHandler);
  
};
  
}}}}

#endif /* oatpp_web_protocol_websocket_Handshaker_hpp */
