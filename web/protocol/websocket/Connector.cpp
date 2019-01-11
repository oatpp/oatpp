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

#include "Connector.hpp"

#include "oatpp/algorithm/SHA1.hpp"
#include "oatpp/encoding/Base64.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
const char* const Connector::MAGIC_UUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  
thread_local std::mt19937 Connector::RANDOM_GENERATOR (std::random_device{}());
thread_local std::uniform_int_distribution<size_t> Connector::RANDOM_DISTRIBUTION (0, 255);
  
oatpp::String Connector::generateKey() {
  v_int32 keySize = 16;
  oatpp::String key(keySize);
  for(v_int32 i = 0; i < keySize; i ++) {
    key->getData()[i] = RANDOM_DISTRIBUTION(RANDOM_GENERATOR);
  }
  return oatpp::encoding::Base64::encode(key);
}
  
oatpp::String Connector::getHeader(const Headers& headers, const oatpp::data::share::StringKeyLabelCI_FAST& key) {
  
  auto it = headers.find(key);
  if(it != headers.end()) {
    return it->second.toString();
  }
  
  return nullptr;
  
}
  
std::shared_ptr<Connector::OutgoingResponse> Connector::serversideHandshake(const Headers& requestHeaders, const std::shared_ptr<ConnectionHandler>& connectionUpgradeHandler) {
  
  auto version = getHeader(requestHeaders, "Sec-WebSocket-Version");
  auto upgrade = getHeader(requestHeaders, oatpp::web::protocol::http::Header::UPGRADE);
  auto connection = getHeader(requestHeaders, oatpp::web::protocol::http::Header::CONNECTION);
  auto key = getHeader(requestHeaders, "Sec-WebSocket-Key");
  
  if(upgrade && connection && version && key && key->getSize() > 0 &&
     upgrade == "websocket" && connection == oatpp::web::protocol::http::Header::Value::CONNECTION_UPGRADE)
  {
    
    if(version == "13") {
     
      auto websocketKey = key + MAGIC_UUID;
      oatpp::algorithm::SHA1 sha1;
      sha1.update(websocketKey);
      auto websocketAccept = oatpp::encoding::Base64::encode(sha1.finalBinary());
      
      auto response = OutgoingResponse::createShared(oatpp::web::protocol::http::Status::CODE_101, nullptr);
      
      response->putHeader(oatpp::web::protocol::http::Header::UPGRADE, "websocket");
      response->putHeader(oatpp::web::protocol::http::Header::CONNECTION, oatpp::web::protocol::http::Header::Value::CONNECTION_UPGRADE);
      response->putHeader("Sec-WebSocket-Accept", websocketAccept);
      response->setConnectionUpgradeHandler(connectionUpgradeHandler);
      
      return response;
      
    }
    
  }
  
  return nullptr;
  
}
  
void Connector::clientsideHandshake(Headers& requestHeaders) {
  requestHeaders[oatpp::web::protocol::http::Header::UPGRADE] = "websocket";
  requestHeaders[oatpp::web::protocol::http::Header::CONNECTION] = oatpp::web::protocol::http::Header::Value::CONNECTION_UPGRADE;
  requestHeaders["Sec-WebSocket-Version"] = "13";
  requestHeaders["Sec-WebSocket-Key"] = generateKey();
}
  
std::shared_ptr<WebSocket> Connector::clientConnect(const Headers& clientHandshakeHeaders, const std::shared_ptr<IncomingResponse>& serverResponse) {
  
  if(serverResponse->getStatusCode() == 101) {
    
    auto& responseHeaders = serverResponse->getHeaders();
    
    auto version = getHeader(responseHeaders, "Sec-WebSocket-Version");
    auto upgrade = getHeader(responseHeaders, oatpp::web::protocol::http::Header::UPGRADE);
    auto connection = getHeader(responseHeaders, oatpp::web::protocol::http::Header::CONNECTION);
    auto websocketAccept = getHeader(responseHeaders, "Sec-WebSocket-Accept");
    
    auto clientKey = getHeader(clientHandshakeHeaders, "Sec-WebSocket-Key");
    
    if(!version && upgrade && connection && websocketAccept && clientKey &&
       upgrade == "websocket" && connection == oatpp::web::protocol::http::Header::Value::CONNECTION_UPGRADE)
    {
      
      auto websocketKey = clientKey + MAGIC_UUID;
      oatpp::algorithm::SHA1 sha1;
      sha1.update(websocketKey);
      auto clientWebsocketAccept = oatpp::encoding::Base64::encode(sha1.finalBinary());
      
      if(clientWebsocketAccept == websocketAccept) {
        return std::make_shared<WebSocket>(serverResponse->getConnection());
      }
      
    }
    
  }
  
  return nullptr;
  
}
  
}}}}
