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

#include "Http2SessionHandler.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"

#include "oatpp/core/async/worker/IOWorker.hpp"

#include <arpa/inet.h>

namespace oatpp { namespace web { namespace server { namespace http2 {

const char* Http2SessionHandler::TAG = "oatpp::web::server::http2::Http2SessionHandler";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other

Http2SessionHandler::ProcessingResources::ProcessingResources(const std::shared_ptr<processing::Components> &pComponents,
                                                         const std::shared_ptr<http2::Http2Settings> &pInSettings,
                                                         const std::shared_ptr<http2::Http2Settings> &pOutSettings,
                                                         const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> &pInStream,
                                                         const std::shared_ptr<http2::PriorityStreamSchedulerAsync> &pOutStream)
    : components(pComponents)
    , connection(nullptr)
    , outStream(pOutStream)
    , inStream(pInStream)
    , inSettings(pInSettings)
    , outSettings(pOutSettings)
    , h2streams()
    , lastStream(nullptr)
    , highestNonIdleStreamId(0) {
  hpack = protocol::http2::hpack::SimpleHpack::createShared(protocol::http2::hpack::SimpleTable::createShared(inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE)), inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE));
//  OATPP_LOGD("oatpp::web::server::http2::Http2SessionHandler::ProcessingResources", "Constructing %p", this);
}

Http2SessionHandler::ProcessingResources::~ProcessingResources() {
//  OATPP_LOGD("oatpp::web::server::http2::Http2SessionHandler::ProcessingResources", "Destructing %p", this);
}

//
//std::shared_ptr<protocol::http::outgoing::Response>
//Http2SessionHandler::processNextRequest(ProcessingResources &m_resources,
//                                   const std::shared_ptr<protocol::http::incoming::Request> &request,
//                                   ConnectionState &connectionState) {
//
//  std::shared_ptr<protocol::http::outgoing::Response> response;
//
//  try {
//
//    for (auto &interceptor : m_resources->components->requestInterceptors) {
//      response = interceptor->intercept(request);
//      if (response) {
//        return response;
//      }
//    }
//
//    auto route =
//        m_resources->components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);
//
//    if (!route) {
//
//      data::stream::BufferOutputStream ss;
//      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
//         << "', URL: '" << request->getStartingLine().path.toString() << "'";
//
//      connectionState = ConnectionState::CLOSING;
//      return m_resources->components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
//
//    }
//
//    request->setPathVariables(route.getMatchMap());
//    return route.getEndpoint()->handle(request);
//
//  } catch (oatpp::web::protocol::http::HttpError &error) {
//    response =
//        m_resources->components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
//    connectionState = ConnectionState::CLOSING;
//  } catch (std::exception &error) {
//    response = m_resources->components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
//    connectionState = ConnectionState::CLOSING;
//  } catch (...) {
//    response = m_resources->components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
//    connectionState = ConnectionState::CLOSING;
//  }
//
//  return response;
//
//}

async::Action Http2SessionHandler::sendSettingsFrame() {
  static const Http2Settings defaultSettings;
  data::stream::BufferOutputStream bos(FrameHeader::HeaderSize+(6*6));
  bos.setCurrentPosition(3);
  bos.writeCharSimple(FrameType::SETTINGS);
  bos.writeCharSimple(0);
  v_uint32 streamIdent = 0;
  bos.writeSimple(&streamIdent, 4);

  v_uint16 ident;
  v_uint32 setting;
  if (m_resources->inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_HEADER_TABLE_SIZE);
    setting = htonl(m_resources->inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (m_resources->inSettings->getSetting(Http2Settings::SETTINGS_ENABLE_PUSH) != defaultSettings.getSetting(Http2Settings::SETTINGS_ENABLE_PUSH)) {
    ident = htons(Http2Settings::SETTINGS_ENABLE_PUSH);
    setting = htonl(m_resources->inSettings->getSetting(Http2Settings::SETTINGS_ENABLE_PUSH));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (m_resources->inSettings->getSetting(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS) != defaultSettings.getSetting(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS)) {
    ident = htons(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS);
    setting = htonl(m_resources->inSettings->getSetting(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (m_resources->inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE);
    setting = htonl(m_resources->inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (m_resources->inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_MAX_FRAME_SIZE);
    setting = htonl(m_resources->inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (m_resources->inSettings->getSetting(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE);
    setting = htonl(m_resources->inSettings->getSetting(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  v_uint32 payload = bos.getCurrentPosition() - 9;
  bos.setCurrentPosition(0);
  bos.writeCharSimple(((payload >> 16) & 0xff));
  bos.writeCharSimple(((payload >> 8) & 0xff));
  bos.writeCharSimple(((payload) & 0xff));
  bos.setCurrentPosition(payload + FrameHeader::HeaderSize);

  OATPP_LOGD(TAG, "Sending SETTINGS (length:%lu, flags:0x%02x, streamId:%lu)", bos.getCurrentPosition()-FrameHeader::HeaderSize, 0, 0);
  m_resources->outStream->lock(PriorityStreamScheduler::PRIORITY_MAX);
  bos.flushBufferToStream(m_resources->outStream.get());
  m_resources->outStream->unlock();
  return 9 + payload;
}

async::Action Http2SessionHandler::ackSettingsFrame() {
  class SendAckSettingsFrameCoroutine : public SendFrameCoroutine<SendAckSettingsFrameCoroutine> {
   private:
    std::shared_ptr<ProcessingResources> m_resources;
   public:
    SendAckSettingsFrameCoroutine(const std::shared_ptr<ProcessingResources> &resources) : m_resources(resources) {}
    PriorityStreamSchedulerAsync *scheduler() const override {
      return m_resources->outStream.get();
    }
    v_uint8 frameFlags() const override {
      return FrameHeader::Flags::Settings::SETTINGS_ACK;
    }
    FrameType frameType() const override {
      return FrameType::SETTINGS;
    }
  };
  return SendAckSettingsFrameCoroutine::start(m_resources).next(yieldTo(&Http2SessionHandler::nextRequest));
}

async::Action Http2SessionHandler::answerPingFrame() {
  std::shared_ptr<std::string> handle = std::make_shared<std::string>(8, (char)0);

  class SendPingFrameCoroutine : public SendFrameCoroutine<SendPingFrameCoroutine> {
   private:
    std::shared_ptr<ProcessingResources> m_resources;
    data::share::MemoryLabel m_label;
   public:
    const data::share::MemoryLabel *frameData() const override {
      return &m_label;
    }
    PriorityStreamSchedulerAsync *scheduler() const override {
      return m_resources->outStream.get();
    }
    FrameType frameType() const override {
      return FrameType::PING;
    }
    v_uint8 frameFlags() const override {
      return FrameHeader::Flags::Ping::PING_ACK;
    }

    SendPingFrameCoroutine(const std::shared_ptr<ProcessingResources> &resources, const data::share::MemoryLabel &label)
      : m_resources(resources)
      , m_label(label) {}
  };

  data::buffer::InlineReadData readData((void*)handle->data(), handle->size());
  return m_resources->inStream->readSomeDataAsyncInline(
      readData,
      SendPingFrameCoroutine::start(m_resources, handle).next(yieldTo(&Http2SessionHandler::nextRequest))
      );
}

async::Action Http2SessionHandler::sendGoawayFrame(v_uint32 lastStream, protocol::http2::error::ErrorCode errorCode) {
 class SendGoawayFrameCoroutine : public SendFrameCoroutine<SendGoawayFrameCoroutine> {
  private:
   std::shared_ptr<ProcessingResources> m_resources;
   data::share::MemoryLabel m_label;
   v_uint8 buf[8];

  public:
   SendGoawayFrameCoroutine(const std::shared_ptr<ProcessingResources> &resources, v_uint32 lastStream, H2ErrorCode errorCode)
       : m_resources(resources) {
     p_uint8 data = buf;
     *data++ = (lastStream >> 24) & 0xff;
     *data++ = (lastStream >> 16) & 0xff;
     *data++ = (lastStream >> 8) & 0xff;
     *data++ = lastStream & 0xff;
     *data++ = (errorCode >> 24) & 0xff;
     *data++ = (errorCode >> 16) & 0xff;
     *data++ = (errorCode >> 8) & 0xff;
     *data++ = errorCode & 0xff;
   }

   const data::share::MemoryLabel *frameData() const override {
     return &m_label;
   }
   PriorityStreamSchedulerAsync *scheduler() const override {
     return m_resources->outStream.get();
   }
   FrameType frameType() const override {
     return FrameType::GOAWAY;
   }
 };

  return SendGoawayFrameCoroutine::start(m_resources, lastStream, errorCode).next(yieldTo(&Http2SessionHandler::teardown));
}

async::Action Http2SessionHandler::sendResetStreamFrame(v_uint32 stream, protocol::http2::error::ErrorCode errorCode) {
  class SendResetStreamFrameCoroutine : public SendFrameCoroutine<SendResetStreamFrameCoroutine> {
   private:
    std::shared_ptr<ProcessingResources> m_resources;
    data::share::MemoryLabel m_label;
    v_uint32 m_stream;
    v_uint32 m_errorCode;

   public:
    SendResetStreamFrameCoroutine(const std::shared_ptr<ProcessingResources> &resources, v_uint32 stream, H2ErrorCode errorCode)
        : m_resources(resources)
        , m_errorCode(htonl(errorCode))
        , m_label(nullptr, &m_errorCode, 4) {}

    const data::share::MemoryLabel *frameData() const override {
      return &m_label;
    }
    PriorityStreamSchedulerAsync *scheduler() const override {
      return m_resources->outStream.get();
    }
    FrameType frameType() const override {
      return FrameType::RST_STREAM;
    }
    v_uint32 frameStreamId() const override {
      return m_stream;
    }
  };

  return SendResetStreamFrameCoroutine::start(m_resources, stream, errorCode).next(yieldTo(&Http2SessionHandler::nextRequest));
}

async::Action Http2SessionHandler::consumeStream(v_io_size streamPayloadLength, async::Action &&action) {
  class StreamConsumerCoroutine : public async::Coroutine<StreamConsumerCoroutine> {
   private:
    std::shared_ptr<data::stream::InputStream> m_stream;
    v_buff_size m_consume;
    data::buffer::IOBuffer m_buffer;

   public:
    StreamConsumerCoroutine(const std::shared_ptr<data::stream::InputStream> &stream, v_buff_size consume)
      : m_stream(stream)
      , m_consume(consume) {}

    Action act() override {
      if (m_consume > 0) {
        v_io_size chunk = std::min((v_io_size) m_buffer.getSize(), m_consume);
        data::buffer::InlineReadData inlinereader((void *) m_buffer.getData(), chunk);
        m_consume -= chunk;
        return m_stream->readSomeDataAsyncInline(inlinereader, yieldTo(&StreamConsumerCoroutine::act));
      }
      return finish();
    }
  };
  return StreamConsumerCoroutine::start(m_resources->inStream, streamPayloadLength).next(std::forward<async::Action>(action));
}

std::shared_ptr<Http2StreamHandler> Http2SessionHandler::findOrCreateStream(v_uint32 ident, ProcessingResources &m_resources) {
  std::shared_ptr<Http2StreamHandler> handler;
  auto handlerentry = m_resources->h2streams.find(ident);
  if (handlerentry == m_resources->h2streams.end()) {
    if (ident < m_resources->highestNonIdleStreamId) {
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::findOrCreateStream] Error: Tried to create a new stream with a streamId smaller than the currently highest known streamId");
    }
    handler = std::make_shared<Http2StreamHandler>(ident, m_resources->outStream, m_resources->hpack, m_resources->components, m_resources->inSettings, m_resources->outSettings);
    m_resources->h2streams.insert({ident, handler});
  } else {
    handler = handlerentry->second;
  }
  return handler;
}

async::Action Http2SessionHandler::nextRequest() {
  return FrameHeader::readFrameHeaderAsync(m_connection).callbackTo(&Http2SessionHandler::handleFrame);
}

async::Action Http2SessionHandler::handleFrame(const std::shared_ptr<FrameHeader> &header) {

  OATPP_LOGD(TAG, "Received %s (length:%lu, flags:0x%02x, streamId:%lu)", FrameHeader::frameTypeStringRepresentation(header->getType()), header->getLength(), header->getFlags(), header->getStreamId());

  // streamId's have to be uneven numbers
  if ((header->getStreamId() != 0) && (header->getStreamId() & 0x01) == 0) {
    return connectionError(protocol::http2::error::ErrorCode::PROTOCOL_ERROR, "[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received even streamId (PROTOCOL_ERROR)");
  }

  if (m_resources->lastStream && (header->getStreamId() != m_resources->lastStream->getStreamId())) {
    // Headers have to be sent as continous frames of HEADERS, PUSH_PROMISE and CONTINUATION.
    // Only if the last opened stream transitioned beyond its HEADERS stage, other frames are allowed
    if (m_resources->lastStream->getState() < Http2StreamHandler::H2StreamState::PAYLOAD && m_resources->lastStream->getState() > Http2StreamHandler::H2StreamState::INIT) {
      return connectionError(protocol::http2::error::ErrorCode::PROTOCOL_ERROR, "[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Last opened stream is still in its HEADER state but frame for different stream received (PROTOCOL_ERROR)");
    }
  }

  try {
    switch (header->getType()) {

      case FrameType::PING:
        if (header->getStreamId() != 0) {
          consumeStream(header->getLength(), <#initializer#>);
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received PING frame on stream");
        }
        if (header->getLength() != 8) {
          throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received PING with invalid frame size");
        }
        // if bit(1) (ack) is not set, reply the same payload but with flag-bit(1) set
        if ((header->getFlags() & 0x01) == 0x00) {
          return answerPingFrame();
        } else if ((header->getFlags() & 0x01) == 0x01) {
          // all good, just consume
          consumeStream(m_resources->inStream, <#initializer#>);
        } else {
          // unknown flags! for now, just consume
          consumeStream(m_resources->inStream, <#initializer#>);
        }
        break;

      case FrameType::SETTINGS:
        if (header->getStreamId() != 0) {
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received SETTINGS frame on stream");
        }
        if (header->getFlags() == 0) {
          if (header->getLength() % 6) {
            throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received SETTINGS with invalid frame size");
          }
          v_uint32 initialWindowSize = m_resources->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE);
          v_uint32 tableSize = m_resources->outSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE);
          for (v_uint32 consumed = 0; consumed < header->getLength(); consumed += 6) {
            v_uint16 ident;
            v_uint32 parameter;
            m_resources->inStream->readExactSizeDataSimple(&ident, 2);
            m_resources->inStream->readExactSizeDataSimple(&parameter, 4);
            ident = ntohs(ident);
            try {
              m_resources->outSettings->setSetting((Http2Settings::Identifier) ident, ntohl(parameter));
            } catch (protocol::http2::error::connection::ProtocolError &h2pe) {
              throw h2pe;
            } catch (protocol::http2::error::connection::FlowControlError &h2fce) {
              throw h2fce;
            } catch (std::runtime_error &e) {
              OATPP_LOGW(TAG, e.what());
            }
          }
          v_uint32 newInitialWindowSize = m_resources->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE);
          v_uint32 newTableSize = m_resources->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE);
          if (initialWindowSize != newInitialWindowSize) {
            v_int32 change = 0;
            if(initialWindowSize > newInitialWindowSize) {
              change -= (initialWindowSize - newInitialWindowSize);
            } else {
              change = newInitialWindowSize - initialWindowSize;
            }
            for (auto &h2s : m_resources->h2streams) {
              h2s.second->resizeWindow(change);
            }
          }
          if (tableSize != newTableSize) {
            m_resources->hpack->setMaxTableSize(newTableSize);
          }
          ackSettingsFrame(m_resources);
        } else if (header->getFlags() == 0x01 && header->getLength() > 0) {
          throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received SETTINGS ack with payload");
        }
        break;

      case FrameType::GOAWAY:
        if (header->getStreamId() == 0){
          v_uint32 errorCode, lastId, highest = 0;
          m_resources->inStream->readExactSizeDataSimple(&lastId, 4);
          m_resources->inStream->readExactSizeDataSimple(&errorCode, 4);
          errorCode = ntohl(errorCode);
          lastId = ntohl(lastId);
          // Consume remaining debug data.
          if (header->getLength() > 8) {
            v_uint32 remaining = header->getLength() - 8;
            while (remaining > 0) {
              v_uint32 chunk = std::min((v_uint32) 2048, remaining);
              v_char8 buf[chunk];
              m_resources->inStream->readExactSizeDataSimple(buf, chunk);
              remaining -= chunk;
            }
          }
  //        for (auto &handler : m_resources->h2streams) {
  //          if (handler.first > highest) {
  //            highest = handler.first;
  //          }
  //        }
  //        sendGoawayFrame(m_resources, highest, 0);
          m_resources->inStream->setInputStreamIOMode(data::stream::ASYNCHRONOUS);
          while (processNextRequest(m_resources) != ConnectionState::DEAD) {}
          return ConnectionState::CLOSING;
        } else {
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received GOAWAY on stream.");
        }

      case FrameType::WINDOW_UPDATE:
        if (header->getStreamId() == 0) {
          if (header->getLength() != 4) {
            throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received WINDOW_UPDATE with invalid frame size");
          }
          v_uint32 increment;
          m_resources->inStream->readExactSizeDataSimple(&increment, 4);
          increment = ntohl(increment);
          if (increment == 0) {
            throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Increment of 0");
          }
          OATPP_LOGD(TAG, "Incrementing out-window by %u", increment);
          m_resources->outSettings->setSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE, m_resources->outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)+increment);
        } else {
          ConnectionState state = delegateToHandler(findOrCreateStream(header->getStreamId(), m_resources),
                                                    m_resources->inStream,
                                                    m_resources,
                                                    *header);
          if (state != ConnectionState::ALIVE) {
            m_resources->h2streams.erase(header->getStreamId());
          }
        }
        break;

      case FrameType::DATA:
      case FrameType::HEADERS:
      case FrameType::PRIORITY:
      case FrameType::RST_STREAM:
      case FrameType::PUSH_PROMISE:
      case FrameType::CONTINUATION:
        if (header->getStreamId() == 0) {
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received stream related frame on stream(0)");
        } else {
          m_resources->lastStream = findOrCreateStream(header->getStreamId(), m_resources);
          ConnectionState state = delegateToHandler(m_resources->lastStream, m_resources->inStream, m_resources, *header);
          if (state != ConnectionState::ALIVE) {
            m_resources->lastStream->clean();
          }
        }
        break;

      default:
        // ToDo: Unknown frame
        consumeStream(m_resources->inStream, <#initializer#>);
        break;
    }
  } catch (protocol::http2::error::Error &h2e) {
    OATPP_LOGE(TAG, "%s (%s)", h2e.what(), h2e.getH2ErrorCodeString());
    if (h2e.getH2ErrorScope() == protocol::http2::error::CONNECTION) {
      sendGoawayFrame(m_resources, header->getStreamId(), h2e.getH2ErrorCode());
      return ConnectionState::DEAD;
    } else {
      sendResetStreamFrame(m_resources, header->getStreamId(), h2e.getH2ErrorCode());
      return ConnectionState::ALIVE;
    }
  }

  return ConnectionState::ALIVE;
}

Http2SessionHandler::ConnectionState Http2SessionHandler::invalidateConnection(Http2SessionHandler::ProcessingResources &m_resources) {
  return ConnectionState::DEAD;
}

void Http2SessionHandler::stop(Http2SessionHandler::ProcessingResources &m_resources) {
  for(auto & h2stream : m_resources->h2streams) {
    if (h2stream.second != nullptr) {
      h2stream.second->abort();
    }
  }
  for(auto it = m_resources->h2streams.begin(); it != m_resources->h2streams.end(); ++it) {
    if (it->second != nullptr) {
      it->second->waitForFinished();
    }
  }
}

Http2SessionHandler::ConnectionState Http2SessionHandler::delegateToHandler(const std::shared_ptr<Http2StreamHandler> &handler,
                                                                  const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                  Http2SessionHandler::ProcessingResources &m_resources,
                                                                  FrameHeader &header) {
  ConnectionState state = ConnectionState::CLOSING;

  switch (header.getType()) {
    case FrameType::DATA:

      // Check if the stream is in a state where it would accept data
      // ToDo: Discussion: Should these checks be inside their respective functions?
      if (handler->getState() != Http2StreamHandler::H2StreamState::PAYLOAD) {
        if (handler->getState() >= Http2StreamHandler::H2StreamState::PROCESSING) {
          throw protocol::http2::error::connection::StreamClosed(
              "[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received data for stream that is already (half-)closed");
        }
        throw protocol::http2::error::connection::ProtocolError(
            "[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received data for stream that is not in payload state");
      }
      state = handler->handleData(header.getFlags(), m_resources->inStream, header.getLength());
      break;
    case FrameType::HEADERS:
      if (handler->getState() >= Http2StreamHandler::H2StreamState::PROCESSING) {
        throw protocol::http2::error::connection::StreamClosed(
            "[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received headers for stream that is already (half-)closed");
      }
      if (handler->getStreamId() > m_resources->highestNonIdleStreamId) {
        m_resources->highestNonIdleStreamId = handler->getStreamId();
      }
      state = handler->handleHeaders(header.getFlags(), m_resources->inStream, header.getLength());
      break;
    case FrameType::PRIORITY:
      if (handler->getState() == Http2StreamHandler::H2StreamState::HEADERS || handler->getState() == Http2StreamHandler::H2StreamState::CONTINUATION) {
        throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received PRIORITY frame while still in header state.");
      }
      state = handler->handlePriority(header.getFlags(), m_resources->inStream, header.getLength());
      break;
    case FrameType::RST_STREAM:
      if (handler->getState() == Http2StreamHandler::H2StreamState::INIT) {
        throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received RST_STREAM on an idle stream.");
      }
      state = handler->handleResetStream(header.getFlags(), m_resources->inStream, header.getLength());
      sendResetStreamFrame(m_resources, handler->getStreamId(), protocol::http2::error::ErrorCode::CANCEL);
      break;
    case FrameType::PUSH_PROMISE:
      state = handler->handlePushPromise(header.getFlags(), m_resources->inStream, header.getLength());
      break;
    case FrameType::WINDOW_UPDATE:
      if (handler->getState() == Http2StreamHandler::H2StreamState::INIT) {
        throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received WINDOW_UPDATE on an idle stream.");
      }
      state = handler->handleWindowUpdate(header.getFlags(), m_resources->inStream, header.getLength());
      break;
    case FrameType::CONTINUATION:
      // Check if the stream is in its "headers received" state, i.E. the only state when continued headers should be acceptable
      if (handler->getState() != Http2StreamHandler::H2StreamState::HEADERS && handler->getState() != Http2StreamHandler::H2StreamState::CONTINUATION) {
        consumeStream(m_resources->inStream, <#initializer#>);
        throw protocol::http2::error::connection::ProtocolError(
            "[oatpp::web::server::http2::Http2SessionHandler::processNextRequest] Error: Received continued headers for stream that is not in its header state");
      }
      state = handler->handleContinuation(header.getFlags(), m_resources->inStream, header.getLength());
      break;
    default:
      state = ConnectionState::CLOSING;
      break;
  }

  return state;
}

async::Action Http2SessionHandler::connectionError(Http2SessionHandler::H2ErrorCode errorCode) {
  return sendGoawayFrame(0, errorCode);
}

async::Action Http2SessionHandler::connectionError(Http2SessionHandler::H2ErrorCode errorCode,
                                                   const std::string &message) {
  OATPP_LOGE(TAG, "%s: %s", protocol::http2::error::stringRepresentation(errorCode), message.data());
  return connectionError(errorCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup

Http2SessionHandler::Http2SessionHandler(const std::shared_ptr<processing::Components> &components,
                           const std::shared_ptr<oatpp::data::stream::IOStream> &connection,
                           std::atomic_long *taskCounter)
    : Http2SessionHandler(components, connection, taskCounter, nullptr) {}

Http2SessionHandler::Http2SessionHandler(const std::shared_ptr<processing::Components> &components,
                           const std::shared_ptr<oatpp::data::stream::IOStream> &connection,
                           std::atomic_long *sessionCounter,
                           const std::shared_ptr<const network::ConnectionHandler::ParameterMap> &delegationParameters)
    : m_components(components), m_connection(connection), m_counter(sessionCounter) {
  (*m_counter)++;
  m_connection->initContexts();

  auto inSettings = http2::Http2Settings::createShared();
  auto outSettings = http2::Http2Settings::createShared();
  auto outStream = std::make_shared<http2::PriorityStreamScheduler>(m_connection);
  auto memlabel = std::make_shared<std::string>(inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE), 0);
  v_io_size writePosition = 0;

  if (delegationParameters != nullptr) {
    auto it = delegationParameters->find("h2frame");
    if (it != delegationParameters->end()) {
      std::memcpy((void*)memlabel->data(), it->second->data(), it->second->size());
      writePosition = it->second->size() + 1;
    }
  }

  auto inStream = data::stream::InputStreamBufferedProxy::createShared(m_connection, memlabel, 0, writePosition, true);

  m_resources = ProcessingResources::createShared(m_components, inSettings, outSettings, inStream, outStream);
}

Http2SessionHandler::Http2SessionHandler(const Http2SessionHandler &copy)
    : m_components(copy.m_components), m_connection(copy.m_connection), m_counter(copy.m_counter), m_resources(copy.m_resources) {
  (*m_counter)++;
}

Http2SessionHandler::Http2SessionHandler(Http2SessionHandler &&move)
    : m_components(std::move(move.m_components)),
      m_connection(std::move(move.m_connection)),
      m_counter(move.m_counter),
      m_resources(std::move(move.m_resources)) {
  move.m_counter = nullptr;
}

Http2SessionHandler &Http2SessionHandler::operator=(const Http2SessionHandler &t) {
  if (this != &t) {
    m_components = t.m_components;
    m_connection = t.m_connection;
    m_counter = t.m_counter;
    m_resources = t.m_resources;
    (*m_counter)++;
  }
  return *this;
}

Http2SessionHandler &Http2SessionHandler::operator=(Http2SessionHandler &&t) {
  m_components = std::move(t.m_components);
  m_connection = std::move(t.m_connection);
  m_resources = std::move(t.m_resources);
  m_counter = t.m_counter;
  t.m_counter = nullptr;
  return *this;
}

async::Action Http2SessionHandler::act() {
  try {
    return Http2SessionHandler::sendSettingsFrame();
  } catch (...) {
    return finish();
  }
}

async::Action Http2SessionHandler::teardown() {
  return finish();
}

Http2SessionHandler::~Http2SessionHandler() noexcept {
  if (m_counter != nullptr) {
    (*m_counter)--;
  }
}

}}}}