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

#ifndef oatpp_web_protocol_http_incoming_RequestHeadersReader_hpp
#define oatpp_web_protocol_http_incoming_RequestHeadersReader_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

/**
 * Helper class to read http headers of incoming request from stream.
 */
class RequestHeadersReader {
public:
  /**
   * Convenience typedef for &id:oatpp::async::Action;.
   */
  typedef oatpp::async::Action Action;
private:
  static constexpr v_int32 SECTION_END = ('\r' << 24) | ('\n' << 16) | ('\r' << 8) | ('\n');
public:

  /**
   * Result of headers reading and parsing.
   */
  struct Result {
    /**
     * &id:oatpp::web::protocol::http::RequestStartingLine;.
     */
    http::RequestStartingLine startingLine;

    /**
     * &id:oatpp::web::protocol::http::Headers;.
     */
    http::Headers headers;

  };

private:
  data::v_io_size readHeadersSection(data::stream::InputStreamBufferedProxy* stream, Result& result);
private:
  v_int32 m_readChunkSize;
  v_int32 m_maxHeadersSize;
  oatpp::data::stream::BufferOutputStream* m_bufferStream;
public:

  /**
   * Constructor.
   * @param readChunkSize
   * @param maxHeadersSize
   */
  RequestHeadersReader(oatpp::data::stream::BufferOutputStream* bufferStream,
                       v_int32 readChunkSize = 2048,
                       v_int32 maxHeadersSize = 4096)
    : m_bufferStream(bufferStream)
    , m_readChunkSize(readChunkSize)
    , m_maxHeadersSize(maxHeadersSize)
  {}

  /**
   * Read and parse http headers from stream.
   * @param stream - &id:oatpp::data::stream::InputStreamBufferedProxy;.
   * @param error - out parameter &id:oatpp::web::protocol::ProtocolError::Info;.
   * @return - &l:RequestHeadersReader::Result;.
   */
  Result readHeaders(data::stream::InputStreamBufferedProxy* stream, http::HttpError::Info& error);

  /**
   * Read and parse http headers from stream in asynchronous manner.
   * @param stream - `std::shared_ptr` to &id:oatpp::data::stream::InputStreamBufferedProxy;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const RequestHeadersReader::Result&> readHeadersAsync(const std::shared_ptr<data::stream::InputStreamBufferedProxy>& stream);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_RequestHeadersReader_hpp */
