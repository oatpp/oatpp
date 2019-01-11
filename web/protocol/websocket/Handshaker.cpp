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

#include "Handshaker.hpp"

#include "oatpp/algorithm/SHA1.hpp"
#include "oatpp/encoding/Base64.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
const char* const Handshaker::MAGIC_UUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  
oatpp::String Handshaker::getHeader(const Headers& headers, const oatpp::data::share::StringKeyLabelCI_FAST& key) {
  
  auto it = headers.find(key);
  if(it != headers.end()) {
    return it->second.toString();
  }
  
  return nullptr;
  
}
  
std::shared_ptr<Handshaker::OutgoingResponse> Handshaker::serversideHandshake(const Headers& headers, const std::shared_ptr<ConnectionHandler>& connectionUpgradeHandler) {
  
  auto version = getHeader(headers, "Sec-WebSocket-Version");
  auto upgrade = getHeader(headers, oatpp::web::protocol::http::Header::UPGRADE);
  auto connection = getHeader(headers, oatpp::web::protocol::http::Header::CONNECTION);
  auto key = getHeader(headers, "Sec-WebSocket-Key");
  
  if(upgrade && connection && version && key && key->getSize() > 0 &&
     upgrade == "websocket" && connection == "Upgrade")
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
  
}}}}
