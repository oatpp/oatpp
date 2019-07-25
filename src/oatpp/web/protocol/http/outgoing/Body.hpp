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

#ifndef oatpp_web_protocol_http_outgoing_Body_hpp
#define oatpp_web_protocol_http_outgoing_Body_hpp

#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/collection/ListMap.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Abstract http outgoing body.
 * You may extend this class in order to implement custom body transferring functionality.
 */
class Body {
protected:
  typedef http::Headers Headers;
  typedef oatpp::data::stream::OutputStream OutputStream;
public:

  /**
   * Virtual destructor.
   */
  virtual ~Body() = default;

  /**
   * Declare headers describing body.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  virtual void declareHeaders(Headers& headers) noexcept = 0;
  
  /**
   * write content to stream
   */

  /**
   * Write body content to stream.
   * @param stream - pointer to &id:oatpp::data::stream::OutputStream;.
   */
  virtual void writeToStream(OutputStream* stream) noexcept = 0;

  /**
   * Same as &l:Body::writeToStream (); but async.
   * @param stream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  virtual oatpp::async::CoroutineStarter writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) = 0;
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Body_hpp */
