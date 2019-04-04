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

    /**
     * This value represents starting position in buffer used to read data from stream for the last read operation.
     */
    v_int32 bufferPosStart;

    /**
     * This value represents end position in buffer used to read data from stream for the last read operation.
     */
    v_int32 bufferPosEnd;
  };

private:
  data::v_io_size readHeadersSection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                                     oatpp::data::stream::OutputStream* bufferStream,
                                     Result& result);
private:
  p_char8 m_buffer;
  v_int32 m_bufferSize;
  v_int32 m_maxHeadersSize;
public:

  /**
   * Constructor.
   * @param buffer - buffer to use to read data from stream.
   * @param bufferSize - buffer size.
   * @param maxHeadersSize - maximum allowed size in bytes of http headers section.
   */
  RequestHeadersReader(void* buffer, v_int32 bufferSize, v_int32 maxHeadersSize)
    : m_buffer((p_char8) buffer)
    , m_bufferSize(bufferSize)
    , m_maxHeadersSize(maxHeadersSize)
  {}

  /**
   * Read and parse http headers from stream.
   * @param connection - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   * @param error - out parameter &id:oatpp::web::protocol::ProtocolError::Info;.
   * @return - &l:RequestHeadersReader::Result;.
   */
  Result readHeaders(const std::shared_ptr<oatpp::data::stream::IOStream>& connection, http::HttpError::Info& error);

  /**
   * Read and parse http headers from stream in asynchronous manner.
   * @param connection - `std::shared_ptr` to &id:oatpp::data::stream::IOStream;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const RequestHeadersReader::Result&> readHeadersAsync(const std::shared_ptr<oatpp::data::stream::IOStream>& connection);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_RequestHeadersReader_hpp */
