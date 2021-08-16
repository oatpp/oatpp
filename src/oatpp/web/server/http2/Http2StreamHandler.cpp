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

#include <arpa/inet.h>
#include <thread>

#include "Http2StreamHandler.hpp"

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

const char* Http2StreamHandler::TAG = "oatpp::web::server::http2::Http2StreamHandler";

Http2StreamHandler::ConnectionState Http2StreamHandler::handleData(v_uint8 flags,
                                                                   const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                   v_io_size streamPayloadLength) {
  m_state = H2StreamState::PAYLOAD;
  v_uint8 pad = 0;
  if (flags & H2StreamDataFlags::DATA_PADDED) {
    stream->readExactSizeDataSimple(&pad, 1);
    streamPayloadLength--;
  }

  stream->readExactSizeDataSimple(m_data.getData() + m_data.getCurrentPosition(), streamPayloadLength);

  if (pad > 0) {
    m_data.setCurrentPosition(m_data.getCurrentPosition() - pad);
  }

  if (flags & H2StreamDataFlags::DATA_END_STREAM) {
    m_state = H2StreamState::PROCESSING;
    std::thread processingThread(&Http2StreamHandler::process, this);
    processingThread.detach();
  }

  return Http2StreamHandler::ConnectionState::ALIVE;
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
    OATPP_LOGD(TAG, "Stream and headers finished, start processing.");
    m_state = H2StreamState::PROCESSING;
    std::thread processingThread(&Http2StreamHandler::process, this);
    processingThread.detach();
  } else if (m_headerFlags & H2StreamHeaderFlags::HEADER_END_STREAM) { // continuation
    OATPP_LOGD(TAG, "Stream finished, headers not, awaiting continuation.");
    m_state = H2StreamState::CONTINUATION;
  } else if (m_headerFlags & H2StreamHeaderFlags::HEADER_END_HEADERS) { // continuation
    m_state = H2StreamState::PAYLOAD;
    OATPP_LOGD(TAG, "Headers finished, stream not, awaiting data.");
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
  m_data.reserveBytesUpfront(ntohl(increment));

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

void Http2StreamHandler::process() {

  protocol::http::RequestStartingLine startingLine;
  startingLine.protocol = "HTTP/2.0";
  startingLine.path = m_headers.get(protocol::http2::Header::PATH);
  startingLine.method = m_headers.get(protocol::http2::Header::METHOD);

  auto bodyStream = std::make_shared<data::stream::BufferInputStream>(nullptr, m_data.getData(), m_data.getCurrentPosition());
  auto request = protocol::http::incoming::Request::createShared(nullptr, startingLine, m_headers, bodyStream, m_components->bodyDecoder);

  ConnectionState connectionState;
  std::shared_ptr<protocol::http::outgoing::Response> response;

  try{

    // ToDo: Interceptors

    auto route = m_components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);

    if(!route) {

      data::stream::BufferOutputStream ss;
      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
         << "', URL: '" << request->getStartingLine().path.toString() << "'";

      connectionState = ConnectionState::CLOSING;
      response = m_components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
    }

    request->setPathVariables(route.getMatchMap());
    response = route.getEndpoint()->handle(request);

  } catch (oatpp::web::protocol::http::HttpError& error) {
    response = m_components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
    connectionState = ConnectionState::CLOSING;
  } catch (std::exception& error) {
    response = m_components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    connectionState = ConnectionState::CLOSING;
  } catch (...) {
    response = m_components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
    connectionState = ConnectionState::CLOSING;
  }

  auto headers = response->getHeaders();
  headers.putIfNotExists(protocol::http2::Header::STATUS, utils::conversion::uint32ToStr(response->getStatus().code));
  auto headerBlocks = m_hpack->deflate(headers, m_flow); // ToDo: Framesize != FlowSize

  if (headerBlocks.size() == 1) {
    auto &block = headerBlocks.front();
    protocol::http2::Frame::Header hdr(block.size(), protocol::http2::Frame::Header::Flags::Header::HEADER_END_HEADERS | (response->getBody()->getKnownSize() == 0 ? protocol::http2::Frame::Header::Flags::Header::HEADER_END_STREAM : 0), protocol::http2::Frame::Header::HEADERS, m_streamId);
    m_output->lock(m_weight);
    hdr.writeToStream(m_output.get());
    m_output->unlock();
  } else {
    auto it = headerBlocks.begin();
    auto last = --headerBlocks.end();
    {
      protocol::http2::Frame::Header hdr(it->size(), 0, protocol::http2::Frame::Header::HEADERS, m_streamId);
      m_output->lock(m_weight);
      hdr.writeToStream(m_output.get());
      m_output->unlock();
    }
    while(it != last) {
      protocol::http2::Frame::Header hdr(it->size(), 0, protocol::http2::Frame::Header::CONTINUATION, m_streamId);
      m_output->lock(m_weight);
      hdr.writeToStream(m_output.get());
      m_output->unlock();
      ++it;
    }
    {
      protocol::http2::Frame::Header hdr(it->size(), protocol::http2::Frame::Header::Flags::Header::HEADER_END_HEADERS | (response->getBody()->getKnownSize() == 0 ? protocol::http2::Frame::Header::Flags::Header::HEADER_END_STREAM : 0), protocol::http2::Frame::Header::CONTINUATION, m_streamId);
      m_output->lock(m_weight);
      hdr.writeToStream(m_output.get());
      m_output->unlock();
    }
  }

  // ToDo: Segmentation/Framing
  if (response->getBody()->getKnownSize() > 0 && response->getBody()->getKnownData() != nullptr) {
    protocol::http2::Frame::Header hdr(response->getBody()->getKnownSize(), 0, protocol::http2::Frame::Header::DATA, m_streamId);
    m_output->lock(m_weight);
    hdr.writeToStream(m_output.get());
    m_output->writeSimple(response->getBody()->getKnownData(), response->getBody()->getKnownSize());
    m_output->unlock();
  }
  m_state = H2StreamState::INIT;
}

}}}}