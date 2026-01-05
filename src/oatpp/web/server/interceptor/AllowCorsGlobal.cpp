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

#include "AllowCorsGlobal.hpp"

namespace oatpp { namespace web { namespace server { namespace interceptor {

std::shared_ptr<protocol::http::outgoing::Response> AllowOptionsGlobal::intercept(const std::shared_ptr<IncomingRequest> &request) {

  const auto &line = request->getStartingLine();

  if (line.method == "OPTIONS") {
    return OutgoingResponse::createShared(protocol::http::Status::CODE_204, nullptr);
  }

  return nullptr;

}

AllowCorsGlobal::AllowCorsGlobal(const oatpp::String &origin,
                                 const oatpp::String &methods,
                                 const oatpp::String &headers,
                                 const oatpp::String &maxAge)
  : m_allowedOrigins({origin})
  , m_methods(methods)
  , m_headers(headers)
  , m_maxAge(maxAge)
{}

AllowCorsGlobal::AllowCorsGlobal(const std::unordered_set<oatpp::String>& allowedOrigins,
                                 const oatpp::String &methods,
                                 const oatpp::String &headers,
                                 const oatpp::String &maxAge)
  : m_allowedOrigins(allowedOrigins)
  , m_methods(methods)
  , m_headers(headers)
  , m_maxAge(maxAge)
{
  if(allowedOrigins.empty()) {
    throw std::runtime_error("[oatpp::web::server::interceptor::AllowCorsGlobal::AllowCorsGlobal()]: Error - allowed origins set is empty");
  }
}

std::shared_ptr<protocol::http::outgoing::Response> AllowCorsGlobal::intercept(const std::shared_ptr<IncomingRequest>& request,
                                                                               const std::shared_ptr<OutgoingResponse>& response)
{
  if(m_allowedOrigins.size() == 1) {
    response->putHeaderIfNotExists(protocol::http::Header::CORS_ORIGIN, *m_allowedOrigins.begin());
  } else {
    auto origin = request->getHeader(protocol::http::Header::ORIGIN);
    if(origin && m_allowedOrigins.find(origin) != m_allowedOrigins.end()) {
      response->putHeaderIfNotExists(protocol::http::Header::CORS_ORIGIN, origin);
    }
  }
  response->putHeaderIfNotExists(protocol::http::Header::CORS_METHODS, m_methods);
  response->putHeaderIfNotExists(protocol::http::Header::CORS_HEADERS, m_headers);
  response->putHeaderIfNotExists(protocol::http::Header::CORS_MAX_AGE, m_maxAge);
  return response;
}

}}}}
