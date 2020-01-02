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

#ifndef oatpp_web_protocol_http_stream_ChunkedStream_hpp
#define oatpp_web_protocol_http_stream_ChunkedStream_hpp

#include "oatpp/core/data/buffer/IOBuffer.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace stream {

/**
 * Stream to decode chunked encoded stream.
 */
class ChunkedDecodingStream : public data::stream::InputStream {
private:
  std::shared_ptr<data::stream::InputStream> m_baseStream;
  data::v_io_size m_chunkSize;
  data::v_io_size m_lineSize;   // size in bytes of the "line" string.
  bool m_done;
  v_char8 m_lineBuffer[11];     // 11 - 0xFFFFFFFF + "\r\n" + '\0'. Assume maximum chunk size of 4GBs.
private:
  data::v_io_size readLine(void *buffer, v_buff_size count, async::Action& action);
public:

  /**
   * Constructor.
   * @param baseStream - chunked encoded stream.
   */
  ChunkedDecodingStream(const std::shared_ptr<data::stream::InputStream>& baseStream);

  /**
   * Read decoded bytes.
   * @param buffer
   * @param count
   * @param action
   * @return
   */
  data::v_io_size read(void *buffer, v_buff_size count, async::Action& action) override;

  /**
   * Set stream I/O mode.
   * @throws
   */
  void setInputStreamIOMode(data::stream::IOMode ioMode) override;

  /**
   * Get stream I/O mode.
   * @return
   */
  data::stream::IOMode getInputStreamIOMode() override;

  /**
   * Get stream context.
   * @return - &l:Context;.
   */
  data::stream::Context& getInputStreamContext() override;

};

}}}}}

#endif // oatpp_web_protocol_http_stream_ChunkedStream_hpp
