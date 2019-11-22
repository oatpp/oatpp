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

#ifndef oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp
#define oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp

#include "./Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Implementation of &id:oatpp::web::protocol::http::outgoing::Body; class.
 * Implements functionality to use &id::oatpp::data::stream::ChunkedBuffer; as data source for http body.
 */
class ChunkedBufferBody : public oatpp::base::Countable, public Body, public std::enable_shared_from_this<ChunkedBufferBody> {
public:
  OBJECT_POOL(Http_Outgoing_ChunkedBufferBody_Pool, ChunkedBufferBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_ChunkedBufferBody_Pool, ChunkedBufferBody, 32)
protected:
  std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_buffer;
public:
  /**
   * Constructor.
   * @param buffer - &id::oatpp::data::stream::ChunkedBuffer;.
   */
  ChunkedBufferBody(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer);
public:

  /**
   * Create shared ChunkedBufferBody.
   * @param buffer - &id::oatpp::data::stream::ChunkedBuffer;.
   * @return - `std::shared_ptr` to ChunkedBufferBody.
   */
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer);

  /**
   * Add `Content-Length` header.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  void declareHeaders(Headers& headers) noexcept override;

  /**
   * Write body data to stream.
   * @param stream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  void writeToStream(OutputStream* stream) noexcept override;

  /**
   * Start &l:ChunkedBufferBody::WriteToStreamCoroutine; to write buffer data to stream.
   * @param stream - &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) override;

  /**
   * Return known size of the body.
   * @return - `v_buff_size`.
   */
  v_buff_size getKnownSize() override;
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp */
