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
#include "oatpp/web/server/http2/ConnectionHandler.hpp"

namespace oatpp { namespace web { namespace server { namespace interceptor {

std::shared_ptr<protocol::http::outgoing::Response> Http2::intercept(const std::shared_ptr<IncomingRequest> &request) {

  const auto &line = request->getStartingLine();

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
