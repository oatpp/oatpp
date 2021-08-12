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

#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"
#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 { namespace processing {

Components::Components(const std::shared_ptr<HttpRouter> &pRouter,
                       const std::shared_ptr<protocol::http::encoding::ProviderCollection> &pContentEncodingProviders,
                       const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder> &pBodyDecoder,
                       const std::shared_ptr<handler::ErrorHandler> &pErrorHandler,
                       const RequestInterceptors &pRequestInterceptors,
                       const ResponseInterceptors &pResponseInterceptors,
                       const std::shared_ptr<Config> &pConfig)
    : router(pRouter),
      contentEncodingProviders(pContentEncodingProviders),
      bodyDecoder(pBodyDecoder),
      errorHandler(pErrorHandler),
      requestInterceptors(pRequestInterceptors),
      responseInterceptors(pResponseInterceptors),
      config(pConfig) {}

Components::Components(const std::shared_ptr<HttpRouter> &pRouter)
    : Components(pRouter,
                 nullptr,
                 std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(),
                 handler::DefaultErrorHandler::createShared(),
                 {},
                 {},
                 std::make_shared<Config>()) {}

Components::Components(const std::shared_ptr<HttpRouter> &pRouter,
                       const std::shared_ptr<Config> &pConfig)
    : Components(pRouter,
                 nullptr,
                 std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(),
                 handler::DefaultErrorHandler::createShared(),
                 {},
                 {},
                 pConfig) {}

}}}}}
