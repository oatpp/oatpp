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

#ifndef oatpp_web_protocol_http_incoming_ResponseHeadersReader_hpp
#define oatpp_web_protocol_http_incoming_ResponseHeadersReader_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

/**
 * Helper class to read http headers of incoming response from stream.
 */
class ResponseHeadersReader {
public:
  /**
   * Convenience typedef for &id:oatpp::async::Action;.
   */
  typedef oatpp::async::Action Action;
private:
  static constexpr v_uint32 SECTION_END = ('\r' << 24) | ('\n' << 16) | ('\r' << 8) | ('\n');
public:

  /**
   * Result of headers reading and parsing.
   */
  struct Result {
    /**
     * &id:oatpp::web::protocol::http::ResponseStartingLine;.
     */
    http::ResponseStartingLine startingLine;

    /**
     * &id:oatpp::web::protocol::http::Headers;.
     */
    http::Headers headers;

    /**
     * This value represents starting position in buffer used to read data from stream for the last read operation.
     */
    v_buff_size bufferPosStart;

    /**
     * This value represents end position in buffer used to read data from stream for the last read operation.
     */
    v_buff_size bufferPosEnd;
  };
private:

  struct ReadHeadersIteration {
    v_uint32 accumulator = 0;
    v_buff_size progress = 0;
    bool done = false;
  };

private:

  v_io_size readHeadersSectionIterative(ReadHeadersIteration& iteration,
                                              const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                              data::stream::ConsistentOutputStream* bufferStream,
                                              Result& result,
                                              async::Action& action);

private:
  oatpp::data::share::MemoryLabel m_buffer;
  v_buff_size m_maxHeadersSize;
public:

  /**
   * Constructor.
   * @param buffer - buffer to use to read data from stream. &id:oatpp::data::share::MemoryLabel;.
   * @param maxHeadersSize
   */
  ResponseHeadersReader(const oatpp::data::share::MemoryLabel& buffer, v_buff_size maxHeadersSize)
    : m_buffer(buffer)
    , m_maxHeadersSize(maxHeadersSize)
  {}

  /**
   * Read and parse http headers from stream.
   * @param connection - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   * @param error - out parameter &id:oatpp::web::protocol::ProtocolError::Info;.
   * @return - &l:ResponseHeadersReader::Result;.
   */
  Result readHeaders(const std::shared_ptr<oatpp::data::stream::IOStream>& connection, http::HttpError::Info& error);

  /**
   * Read and parse http headers from stream in asynchronous manner.
   * @param connection - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const Result&> readHeadersAsync(const std::shared_ptr<oatpp::data::stream::IOStream>& connection);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_ResponseHeadersReader_hpp */
