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

#ifndef oatpp_web_server_http2_Http2ProcessingComponents_hpp
#define oatpp_web_server_http2_Http2ProcessingComponents_hpp

#include "oatpp/core/Types.hpp"

#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"

#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/protocol/http/encoding/ProviderCollection.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"


namespace oatpp { namespace web { namespace server { namespace http2 { namespace processing {

/**
 * Resource config per connection.
 */
struct Config {

  /**
   * Buffer used to read headers in request. Initial size of the buffer.
   */
  v_buff_size headersInBufferInitial = 2048;

  /**
   * Buffer used to write headers in response. Initial size of the buffer.
   */
  v_buff_size headersOutBufferInitial = 2048;

  /**
   * Size of the chunk used for iterative-read of headers.
   */
  v_buff_size headersReaderChunkSize = 2048;

  /**
   * Maximum allowed size of requests headers. The overall size of all headers in the request.
   */
  v_buff_size headersReaderMaxSize = 4096;

};

/**
 * Collection of components needed to serve http-connection.
 */
struct Components {
  typedef std::list<std::shared_ptr<web::server::interceptor::RequestInterceptor>> RequestInterceptors;
  typedef std::list<std::shared_ptr<web::server::interceptor::ResponseInterceptor>> ResponseInterceptors;

  /**
   * Constructor.
   * @param pRouter
   * @param pContentEncodingProviders
   * @param pBodyDecoder
   * @param pErrorHandler
   * @param pRequestInterceptors
   * @param pConfig
   */
  Components(const std::shared_ptr<HttpRouter>& pRouter,
             const std::shared_ptr<protocol::http::encoding::ProviderCollection>& pContentEncodingProviders,
             const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& pBodyDecoder,
             const std::shared_ptr<handler::ErrorHandler>& pErrorHandler,
             const RequestInterceptors& pRequestInterceptors,
             const ResponseInterceptors& pResponseInterceptors,
             const std::shared_ptr<Config>& pConfig);

  /**
   * Constructor.
   * @param pRouter
   */
  Components(const std::shared_ptr<HttpRouter>& pRouter);

  /**
   * Constructor.
   * @param pRouter
   * @param pConfig
   */
  Components(const std::shared_ptr<HttpRouter>& pRouter, const std::shared_ptr<Config>& pConfig);

  /**
   * Router to route incoming requests. &id:oatpp::web::server::HttpRouter;.
   */
  std::shared_ptr<HttpRouter> router;

  /**
   * Content-encoding providers. &id:oatpp::web::protocol::encoding::ProviderCollection;.
   */
  std::shared_ptr<protocol::http::encoding::ProviderCollection> contentEncodingProviders;

  /**
   * Body decoder. &id:oatpp::web::protocol::http::incoming::BodyDecoder;.
   */
  std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder> bodyDecoder;

  /**
   * Error handler. &id:oatpp::web::server::handler::ErrorHandler;.
   */
  std::shared_ptr<handler::ErrorHandler> errorHandler;

  /**
   * Collection of request interceptors. &id:oatpp::web::server::interceptor::RequestInterceptor;.
   */
  RequestInterceptors requestInterceptors;

  /**
   * Collection of request interceptors. &id:oatpp::web::server::interceptor::ResponseInterceptor;.
   */
  ResponseInterceptors responseInterceptors;

  /**
   * Resource allocation config. &l:HttpProcessor::Config;.
   */
  std::shared_ptr<Config> config;


};

}}}}}

#endif //oatpp_web_server_http2_Http2ProcessingComponents_hpp
