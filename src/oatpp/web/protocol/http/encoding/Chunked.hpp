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

#ifndef oatpp_web_protocol_http_encoding_Chunked_hpp
#define oatpp_web_protocol_http_encoding_Chunked_hpp

#include "EncoderProvider.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace encoding {

/**
 * Chunked-encoding buffer processor. &id:oatpp::data::buffer::Processor;.
 */
class EncoderChunked : public data::buffer::Processor {
private:
  oatpp::String m_chunkHeader;
  bool m_writeChunkHeader = true;
  bool m_firstChunk = true;
  bool m_finished = false;
  v_io_size m_lastFlush = 0;
public:

  /**
   * If the client is using the input stream to read data and add it to the processor,
   * the client MAY ask the processor for a suggested read size.
   * @return - suggested read size.
   */
  v_io_size suggestInputStreamReadSize() override;

  /**
   * Process data.
   * @param dataIn - data provided by client to processor. Input data. &id:data::buffer::InlineReadData;.
   * Set `dataIn` buffer pointer to `nullptr` to designate the end of input.
   * @param dataOut - data provided to client by processor. Output data. &id:data::buffer::InlineReadData;.
   * @return - &l:Processor::Error;.
   */
  v_int32 iterate(data::buffer::InlineReadData& dataIn, data::buffer::InlineReadData& dataOut) override;

};

/**
 * Chunked-decoding buffer processor. &id:oatpp::data::buffer::Processor;.
 */
class DecoderChunked : public data::buffer::Processor {
public:
  static constexpr v_int32 ERROR_CHUNK_HEADER_TOO_LONG = 100;
private:
  data::stream::BufferOutputStream m_chunkHeaderBuffer;
  v_io_size m_currentChunkSize;
  bool m_firstChunk;
  bool m_finished;
  v_io_size m_lastFlush;
private:
  v_int32 readHeader(data::buffer::InlineReadData& dataIn);
public:

  /**
   * Constructor.
   */
  DecoderChunked();

  /**
   * If the client is using the input stream to read data and add it to the processor,
   * the client MAY ask the processor for a suggested read size.
   * @return - suggested read size.
   */
  v_io_size suggestInputStreamReadSize() override;

  /**
   * Process data.
   * @param dataIn - data provided by client to processor. Input data. &id:data::buffer::InlineReadData;.
   * Set `dataIn` buffer pointer to `nullptr` to designate the end of input.
   * @param dataOut - data provided to client by processor. Output data. &id:data::buffer::InlineReadData;.
   * @return - &l:Processor::Error;.
   */
  v_int32 iterate(data::buffer::InlineReadData& dataIn, data::buffer::InlineReadData& dataOut) override;

};

/**
 * EncoderProvider for "chunked" encoding.
 */
class ChunkedEncoderProvider : public EncoderProvider {
public:

  /**
   * Get encoding name.
   * @return
   */
  oatpp::String getEncodingName() override;

  /**
   * Get &id:oatpp::data::buffer::Processor; for chunked encoding.
   * @return - &id:oatpp::data::buffer::Processor;
   */
  std::shared_ptr<data::buffer::Processor> getProcessor() override;

};

/**
 * EncoderProvider for "chunked" decoding.
 */
class ChunkedDecoderProvider : public EncoderProvider {
public:

  /**
   * Get encoding name.
   * @return
   */
  oatpp::String getEncodingName() override;

  /**
   * Get &id:oatpp::data::buffer::Processor; for chunked decoding.
   * @return - &id:oatpp::data::buffer::Processor;
   */
  std::shared_ptr<data::buffer::Processor> getProcessor() override;

};

}}}}}

#endif //oatpp_web_protocol_http_encoding_Chunked_hpp
