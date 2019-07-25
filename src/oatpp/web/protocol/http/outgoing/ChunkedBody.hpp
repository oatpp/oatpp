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

#ifndef oatpp_web_protocol_http_outgoing_ChunkedBody_hpp
#define oatpp_web_protocol_http_outgoing_ChunkedBody_hpp

#include "./Body.hpp"

#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Body with `Transfer-Encoding: chunked`.
 */
class ChunkedBody : public oatpp::base::Countable, public Body, public std::enable_shared_from_this<ChunkedBody>  {
public:
  /**
   * Convenience typedef for &id:oatpp::data::stream::ReadCallback;.
   */
  typedef oatpp::data::stream::ReadCallback ReadCallback;

  /**
   * Convenience typedef for &id:oatpp::data::stream::AsyncReadCallback;.
   */
  typedef oatpp::data::stream::AsyncReadCallback AsyncReadCallback;
private:
  bool writeData(OutputStream* stream, const void* data, data::v_io_size size);
private:
  std::shared_ptr<ReadCallback> m_readCallback;
  std::shared_ptr<AsyncReadCallback> m_asyncReadCallback;
  p_char8 m_buffer;
  data::v_io_size m_bufferSize;
public:

  /**
   * Constructor. Must set either `ReadCallback` or `AsyncReadCallback`.
   * @param readCallback - &id:oatpp::data::stream::ReadCallback;.
   * @param asyncReadCallback - &id:oatpp::data::stream::AsyncReadCallback;.
   * @param chunkBufferSize - max size of the chunk. Will allocate buffer of size `chunkBufferSize`.
   */
  ChunkedBody(const std::shared_ptr<ReadCallback>& readCallback,
              const std::shared_ptr<AsyncReadCallback>& asyncReadCallback,
              data::v_io_size chunkBufferSize);

  /**
   * virtual destructor.
   */
  ~ChunkedBody();

  /**
   * Declare `Transfer-Encoding: chunked` header.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  void declareHeaders(Headers& headers) noexcept override;

  /**
   * Write body data to stream.
   * @param stream - pointer to &id:oatpp::data::stream::OutputStream;.
   */
  void writeToStream(OutputStream* stream) noexcept override;

  /**
   * Write body data to stream in asynchronous manner.
   * @param stream - &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) override;

};

}}}}}

#endif // oatpp_web_protocol_http_outgoing_ChunkedBody_hpp
