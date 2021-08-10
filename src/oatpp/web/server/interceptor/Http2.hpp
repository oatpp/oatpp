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

#ifndef oatpp_web_server_interceptor_Http2_hpp
#define oatpp_web_server_interceptor_Http2_hpp

#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/server/http2/ConnectionHandler.hpp"

namespace oatpp { namespace web { namespace server { namespace interceptor {

class Http2 : public RequestInterceptor {
  std::shared_ptr<http2::ConnectionHandler> m_handler;
public:
  Http2(std::shared_ptr<http2::ConnectionHandler> handler) : m_handler(std::move(handler)) {};
  static std::shared_ptr<RequestInterceptor> createShared(const std::shared_ptr<http2::ConnectionHandler> &handler) {
    return std::make_shared<interceptor::Http2>(handler);
  }

  std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override;
};

}}}}

#endif // oatpp_web_server_interceptor_AllowCorsGlobal_hpp
