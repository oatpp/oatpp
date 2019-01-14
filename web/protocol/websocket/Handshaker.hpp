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

#include "./WebSocket.hpp"

#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Request.hpp"
#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

#include <random>

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
class Handshaker {
public:
  static const char* const MAGIC_UUID;
public:
  static constexpr v_int32 STATUS_OK = 0;
  static constexpr v_int32 STATUS_SERVER_ERROR = 1; // Server response-code != 101
  static constexpr v_int32 STATUS_SERVER_WRONG_KEY = 2; // Server response "Sec-WebSocket-Accept" header is wrong
  static constexpr v_int32 STATUS_UNKNOWN_PROTOCOL_SUGGESTED = 3; // Server's response contains unexpected headers values
private:
  /* Random used to generate "Sec-WebSocket-Key" */
  static thread_local std::mt19937 RANDOM_GENERATOR;
  static thread_local std::uniform_int_distribution<size_t> RANDOM_DISTRIBUTION;
public:
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
  typedef oatpp::web::protocol::http::incoming::Response IncomingResponse;
  typedef oatpp::web::protocol::http::Protocol::Headers Headers;
  typedef oatpp::network::server::ConnectionHandler ConnectionHandler;
private:
  static oatpp::String generateKey();
  static oatpp::String getHeader(const Headers& headers, const oatpp::data::share::StringKeyLabelCI_FAST& key);
public:
  
  /**
   * Prepare OutgoingResponse as for websocket-handshake based on requestHeaders.
   */
  static std::shared_ptr<OutgoingResponse> serversideHandshake(const Headers& requestHeaders, const std::shared_ptr<ConnectionHandler>& connectionUpgradeHandler);
  
  /**
   * Prepare requestHeaders for clientside websocket-handshake request
   */
  static void clientsideHandshake(Headers& requestHeaders);
  
  /**
   * Check if client's handshake corresponds to server's handshake
   * returns one of values
   * (STATUS_OK=0, STATUS_SERVER_ERROR=1, STATUS_SERVER_WRONG_KEY=2, STATUS_UNKNOWN_PROTOCOL_SUGGESTED=3);
   */
  static v_int32 clientsideConfirmHandshake(const Headers& clientHandshakeHeaders, const std::shared_ptr<IncomingResponse>& serverResponse);
  
};
  
}}}}

#endif /* oatpp_web_protocol_websocket_Handshaker_hpp */
