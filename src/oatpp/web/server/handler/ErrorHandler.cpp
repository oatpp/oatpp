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

#include "./ErrorHandler.hpp"

#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

namespace oatpp { namespace web { namespace server { namespace handler {

std::shared_ptr<protocol::http::outgoing::Response> ErrorHandler::handleError(const protocol::http::Status& status, const oatpp::String& message) {
  Headers headers;
  return handleError(status, message, headers);
}

std::shared_ptr<protocol::http::outgoing::Response>
DefaultErrorHandler::handleError(const oatpp::web::protocol::http::Status &status, const oatpp::String &message, const Headers& headers){

  data::stream::BufferOutputStream stream;
  stream << "server=" << protocol::http::Header::Value::SERVER << "\n";
  stream << "code=" << status.code << "\n";
  stream << "description=" << status.description << "\n";
  stream << "message=" << message << "\n";
  auto response = protocol::http::outgoing::Response::createShared
      (status, protocol::http::outgoing::BufferBody::createShared(stream.toString()));

  response->putHeader(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
  response->putHeader(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE);

  for(const auto& pair : headers.getAll()) {
    response->putHeader_Unsafe(pair.first, pair.second);
  }

  return response;

}

}}}}
