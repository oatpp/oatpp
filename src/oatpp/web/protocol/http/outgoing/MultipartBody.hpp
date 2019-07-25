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

#ifndef oatpp_web_protocol_http_outgoing_MultipartBody_hpp
#define oatpp_web_protocol_http_outgoing_MultipartBody_hpp

#include "./Body.hpp"
#include "./ChunkedBody.hpp"

#include "oatpp/web/mime/multipart/Multipart.hpp"

#include "oatpp/core/data/stream/BufferInputStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

class MultipartBody : public ChunkedBody {
public:

  /**
   * Convenience typedef for &id:oatpp::web::mime::multipart::Multipart;.
   */
  typedef oatpp::web::mime::multipart::Multipart Multipart;

  /**
   * Convenience typedef for &id:oatpp::web::mime::multipart::Part;.
   */
  typedef oatpp::web::mime::multipart::Part Part;
private:

  /*
   * Convenience typedef for &id:oatpp::data::stream::ReadCallback;.
   */
  typedef oatpp::data::stream::ReadCallback ReadCallback;

  /*
   * Convenience typedef for &id:oatpp::data::stream::AsyncReadCallback;.
   */
  typedef oatpp::data::stream::AsyncReadCallback AsyncReadCallback;

private:

  static constexpr v_int32 STATE_BOUNDARY = 0;
  static constexpr v_int32 STATE_HEADERS = 1;
  static constexpr v_int32 STATE_BODY = 2;
  static constexpr v_int32 STATE_ROUND = 3; // number of possible states. used to round the state.
  static constexpr v_int32 STATE_FINISHED = 4;

private:
  static data::v_io_size readBoundary(const std::shared_ptr<Multipart>& multipart,
                                      std::list<std::shared_ptr<Part>>::const_iterator& iterator,
                                      data::stream::BufferInputStream& readStream,
                                      void *buffer,
                                      data::v_io_size count);

  static data::v_io_size readHeaders(const std::shared_ptr<Multipart>& multipart,
                                     std::list<std::shared_ptr<Part>>::const_iterator& iterator,
                                     data::stream::BufferInputStream& readStream,
                                     void *buffer,
                                     data::v_io_size count);
private:

  class MultipartReadCallback : public ReadCallback {
  private:
    std::shared_ptr<Multipart> m_multipart;
    std::list<std::shared_ptr<Part>>::const_iterator m_iterator;
    v_int32 m_state;
    oatpp::data::stream::BufferInputStream m_readStream;
  private:
    data::v_io_size readBody(void *buffer, data::v_io_size count);
  public:

    MultipartReadCallback(const std::shared_ptr<Multipart>& multipart);

    data::v_io_size read(void *buffer, data::v_io_size count) override;

  };

private:

  class AsyncMultipartReadCallback : public AsyncReadCallback {
  private:
    std::shared_ptr<Multipart> m_multipart;
    std::list<std::shared_ptr<Part>>::const_iterator m_iterator;
    v_int32 m_state;
    oatpp::data::stream::BufferInputStream m_readStream;
  public:

    AsyncMultipartReadCallback(const std::shared_ptr<Multipart>& multipart);

    oatpp::async::Action readAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                         oatpp::data::stream::AsyncInlineReadData& inlineData,
                                         oatpp::async::Action&& nextAction) override;

  };

private:
  std::shared_ptr<Multipart> m_multipart;
public:

  /**
   * Constructor.
   * @param multipart - multipart object.
   */

  /**
   * Constructor.
   * @param multipart - multipart object.
   * @param chunkBufferSize - buffer used for chunks in the `Transfer-Encoding: chunked` body.
   */
  MultipartBody(const std::shared_ptr<Multipart>& multipart, data::v_io_size chunkBufferSize = 4096);

  /**
   * Declare `Transfer-Encoding: chunked`, `Content-Type: multipart/<type>` header.
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

#endif // oatpp_web_protocol_http_outgoing_MultipartBody_hpp
