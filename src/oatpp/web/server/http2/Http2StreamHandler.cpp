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

#include "Http2StreamHandler.hpp"

#include <arpa/inet.h>

namespace oatpp { namespace web { namespace server { namespace http2 {

Http2StreamHandler::ConnectionState Http2StreamHandler::handleData(v_uint8 flags,
                                                                   const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                   v_io_size streamPayloadLength) {
  m_state = H2StreamState::PAYLOAD;
  v_uint8 pad = 0;
  if (flags & H2StreamDataFlags::DATA_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength -= pad + 1;
  }
  if (flags & H2StreamDataFlags::DATA_END_STREAM) {
    m_state = H2StreamState::PROCESSING;
  }



  return Http2StreamHandler::ConnectionState::CLOSING;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleHeaders(v_uint8 flags,
                                                                      const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                      v_io_size streamPayloadLength) {
  m_state = H2StreamState::HEADERS;
  m_headerFlags = flags;
  v_uint8 pad = 0;

  if (m_headerFlags & H2StreamHeaderFlags::HEADER_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength -= pad + 1;
  }

  if (m_headerFlags & H2StreamHeaderFlags::HEADER_PRIORITY) {
    stream->readExactSizeDataSimple(&m_dependency, 4);
    stream->readExactSizeDataSimple(&m_weight, 1);
    streamPayloadLength -= 5;
  }

  m_headers = m_hpack->inflate(stream, streamPayloadLength);

  if (pad > 0) {
    oatpp::String paddata((v_buff_size)pad);
    stream->readExactSizeDataSimple((void*)paddata->data(), paddata->size());
  }

  if (m_headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) { // end stream, go to processing
    m_state = H2StreamState::PROCESSING;
  } else if (m_headerFlags & H2StreamHeaderFlags::HEADER_END_STREAM) { // continuation
    m_state = H2StreamState::CONTINUATION;
  } else if (m_headerFlags & H2StreamHeaderFlags::HEADER_END_HEADERS) { // continuation
    m_state = H2StreamState::PAYLOAD;
  }

  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handlePriority(v_uint8 flags,
                                                                       const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                       v_io_size streamPayloadLength) {
  if (streamPayloadLength != 5) {
    // ToDo: stream error (Section 5.4.2) of type FRAME_SIZE_ERROR.
  }
  stream->readExactSizeDataSimple(&m_dependency, 4);
  stream->readExactSizeDataSimple(&m_weight, 1);
  streamPayloadLength -= 5;
  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleResetStream(v_uint8 flags,
                                                                          const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                          v_io_size streamPayloadLength) {
  if (streamPayloadLength != 4) {
    // ToDo: A RST_STREAM frame with a length other than 4 octets MUST be treated
    //   as a connection error (Section 5.4.1) of type FRAME_SIZE_ERROR.
  }
  return Http2StreamHandler::ConnectionState::DEAD;
}


Http2StreamHandler::ConnectionState Http2StreamHandler::handlePushPromise(v_uint8 flags,
                                                                          const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                          v_io_size streamPayloadLength) {
  return Http2StreamHandler::ConnectionState::CLOSING;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleGoAway(v_uint8 flags,
                                                                     const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                     v_io_size streamPayloadLength) {
  m_state = H2StreamState::GOAWAY;
  return Http2StreamHandler::ConnectionState::CLOSING;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleWindowUpdate(v_uint8 flags,
                                                                           const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                           v_io_size streamPayloadLength) {
  if (streamPayloadLength != 4) {
    // ToDo: A WINDOW_UPDATE frame with a length other than 4 octets MUST be
    //   treated as a connection error (Section 5.4.1) of type
    //   FRAME_SIZE_ERROR.
  }
  // https://datatracker.ietf.org/doc/html/rfc7540#section-6.9
  // 1 to 2^31-1 (2,147,483,647)
  v_uint32 increment;
  stream->readExactSizeDataSimple(&increment, 4);
  m_data->resize(m_data->size() + ntohl(increment));

  return Http2StreamHandler::ConnectionState::ALIVE;
}

Http2StreamHandler::ConnectionState Http2StreamHandler::handleContinuation(v_uint8 flags,
                                                                           const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                           v_io_size streamPayloadLength) {

  m_headerFlags |= flags;

  if ((m_headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == H2StreamHeaderFlags::HEADER_END_HEADERS) {
    m_state = H2StreamState::PAYLOAD;
  } else if ((m_headerFlags & (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM))
      == (H2StreamHeaderFlags::HEADER_END_HEADERS | H2StreamHeaderFlags::HEADER_END_STREAM)) {
    m_state = H2StreamState::PROCESSING;
  }
  auto cont = m_hpack->inflate(stream, streamPayloadLength);
  auto all = cont.getAll();
  for (auto &hdr : all) {
    m_headers.put(hdr.first, hdr.second);
  }
  return Http2StreamHandler::ConnectionState::ALIVE;
}

}}}}