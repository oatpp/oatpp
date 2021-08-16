/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include "Http2.hpp"
#include "oatpp/web/server/http2/Http2DelegatedConnectionHandler.hpp"

namespace oatpp { namespace web { namespace server { namespace interceptor {

std::shared_ptr<protocol::http::outgoing::Response> Http2::intercept(const std::shared_ptr<IncomingRequest> &request) {

  // 0x505249202a20485454502f322e300d0a0d0a534d0d0a0d0a
  // PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n
  const auto &line = request->getStartingLine();
  if (line.method == "PRI" && line.path == "*" && line.protocol == "HTTP/2.0") {
    auto response = OutgoingResponse::createShared(oatpp::web::protocol::http::Status::CODE_101, nullptr);
    response->putHeader(oatpp::web::protocol::http::Header::CONNECTION,
                        oatpp::web::protocol::http::Header::Value::CONNECTION_UPGRADE);
    response->setConnectionUpgradeHandler(m_handler);
    // read "SM\r\n\r\n" from the buffer
    std::string trailing(6, (char)0);
    request->getBodyStream()->readSimple((void*)trailing.data(), trailing.size());

    // now store the first frames header
    std::shared_ptr<std::string> firstFrame = std::make_shared<std::string>(8, (char)0);
    request->getBodyStream()->readSimple((void*)firstFrame->data(), firstFrame->size());

    // we just need to read 1 frame worth of data. This should be an settings frame.
    p_uint8 dataptr = (p_uint8)firstFrame->data();
    v_uint32 payloadlen = ((*dataptr) << 16) | (*(dataptr + 1) << 8) | (*(dataptr + 2));

    firstFrame->resize(8 + payloadlen, 0);
    request->getBodyStream()->readExactSizeDataSimple((void*)(firstFrame->data()+8), payloadlen);
    auto map = std::make_shared<server::http2::Http2DelegatedConnectionHandler::ParameterMap>();
    map->emplace("h2frame", firstFrame);
    response->setConnectionUpgradeParameters(map);
    return response;
  }

  auto upgrade = request->getHeader(protocol::http::Header::UPGRADE);
  if (upgrade) {
    if (upgrade == "h2c") {
      auto response = OutgoingResponse::createShared(oatpp::web::protocol::http::Status::CODE_101, nullptr);
      response->putHeader(oatpp::web::protocol::http::Header::UPGRADE, "h2c");
      response->putHeader(oatpp::web::protocol::http::Header::CONNECTION,
                          oatpp::web::protocol::http::Header::Value::CONNECTION_UPGRADE);
      response->setConnectionUpgradeHandler(m_handler);
      return response;
    }
  }

  return nullptr;

}

}}}}
