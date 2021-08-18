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

#include "Http2Processor.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"

#include <arpa/inet.h>

namespace oatpp { namespace web { namespace server { namespace http2 {

const char* Http2Processor::TAG = "oatpp::web::server::http2::Http2Processor";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other

Http2Processor::ProcessingResources::ProcessingResources(const std::shared_ptr<processing::Components> &pComponents,
                                                         const std::shared_ptr<http2::Http2Settings> &pInSettings,
                                                         const std::shared_ptr<http2::Http2Settings> &pOutSettings,
                                                         const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> &pInStream,
                                                         const std::shared_ptr<http2::PriorityStreamScheduler> &pOutStream)
    : components(pComponents)
    , connection(nullptr)
    , outStream(pOutStream)
    , inStream(pInStream)
    , inSettings(pInSettings)
    , outSettings(pOutSettings)
    , h2streams()
    , lastStream(nullptr)
    , highestNonIdleStreamId(0) {
  flow = inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE);
  hpack = protocol::http2::hpack::SimpleHpack::createShared(protocol::http2::hpack::SimpleTable::createShared(inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE)));
//  OATPP_LOGD("oatpp::web::server::http2::Http2Processor::ProcessingResources", "Constructing %p", this);
}

Http2Processor::ProcessingResources::~ProcessingResources() {
//  OATPP_LOGD("oatpp::web::server::http2::Http2Processor::ProcessingResources", "Destructing %p", this);
}

//
//std::shared_ptr<protocol::http::outgoing::Response>
//Http2Processor::processNextRequest(ProcessingResources &resources,
//                                   const std::shared_ptr<protocol::http::incoming::Request> &request,
//                                   ConnectionState &connectionState) {
//
//  std::shared_ptr<protocol::http::outgoing::Response> response;
//
//  try {
//
//    for (auto &interceptor : resources.components->requestInterceptors) {
//      response = interceptor->intercept(request);
//      if (response) {
//        return response;
//      }
//    }
//
//    auto route =
//        resources.components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);
//
//    if (!route) {
//
//      data::stream::BufferOutputStream ss;
//      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
//         << "', URL: '" << request->getStartingLine().path.toString() << "'";
//
//      connectionState = ConnectionState::CLOSING;
//      return resources.components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
//
//    }
//
//    request->setPathVariables(route.getMatchMap());
//    return route.getEndpoint()->handle(request);
//
//  } catch (oatpp::web::protocol::http::HttpError &error) {
//    response =
//        resources.components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
//    connectionState = ConnectionState::CLOSING;
//  } catch (std::exception &error) {
//    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
//    connectionState = ConnectionState::CLOSING;
//  } catch (...) {
//    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
//    connectionState = ConnectionState::CLOSING;
//  }
//
//  return response;
//
//}

v_io_size Http2Processor::sendSettingsFrame(Http2Processor::ProcessingResources &resources) {
  static const Http2Settings defaultSettings;
  data::stream::BufferOutputStream bos(FrameHeader::HeaderSize+(6*6));
  bos.setCurrentPosition(3);
  bos.writeCharSimple(FrameType::SETTINGS);
  bos.writeCharSimple(0);
  v_uint32 streamIdent = 0;
  bos.writeSimple(&streamIdent, 4);

  v_uint16 ident;
  v_uint32 setting;
  if (resources.inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_HEADER_TABLE_SIZE);
    setting = htonl(resources.inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (resources.inSettings->getSetting(Http2Settings::SETTINGS_ENABLE_PUSH) != defaultSettings.getSetting(Http2Settings::SETTINGS_ENABLE_PUSH)) {
    ident = htons(Http2Settings::SETTINGS_ENABLE_PUSH);
    setting = htonl(resources.inSettings->getSetting(Http2Settings::SETTINGS_ENABLE_PUSH));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (resources.inSettings->getSetting(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS) != defaultSettings.getSetting(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS)) {
    ident = htons(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS);
    setting = htonl(resources.inSettings->getSetting(Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (resources.inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE);
    setting = htonl(resources.inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (resources.inSettings->getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_MAX_FRAME_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_MAX_FRAME_SIZE);
    setting = htonl(resources.inSettings->getSetting(Http2Settings::SETTINGS_HEADER_TABLE_SIZE));
    bos.writeSimple(&ident, 2);
    bos.writeSimple(&setting, 4);
  }
  if (resources.inSettings->getSetting(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE) != defaultSettings.getSetting(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE)) {
    ident = htons(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE);
    setting = htonl(resources.inSettings->getSetting(Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE));
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
  resources.outStream->lock(PriorityStreamScheduler::PRIORITY_MAX);
  bos.flushToStream(resources.outStream.get());
  resources.outStream->unlock();
  return 9 + payload;
}

v_io_size Http2Processor::ackSettingsFrame(Http2Processor::ProcessingResources &resources) {
  FrameHeader header(0, FrameHeader::Flags::Settings::SETTINGS_ACK, FrameType::SETTINGS, 0);
  OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, streamId:%lu)", FrameHeader::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId());
  resources.outStream->lock(PriorityStreamScheduler::PRIORITY_MAX);
  header.writeToStream(resources.outStream.get());
  resources.outStream->unlock();

  return FrameHeader::HeaderSize;
}

v_io_size Http2Processor::answerPingFrame(Http2Processor::ProcessingResources &resources) {

  data::stream::BufferOutputStream bos(FrameHeader::HeaderSize+8);
  FrameHeader header(8, FrameHeader::Flags::Ping::PING_ACK, FrameType::PING, 0);
  header.writeToStream(&bos);
  resources.inStream->readExactSizeDataSimple(bos.getData()+FrameHeader::HeaderSize, 8);
  bos.setCurrentPosition(FrameHeader::HeaderSize + 8);

  OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, streamId:%lu)", FrameHeader::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId());
  resources.outStream->lock(PriorityStreamScheduler::PRIORITY_MAX);
  bos.flushToStream(resources.outStream.get());
  resources.outStream->unlock();

  return FrameHeader::HeaderSize+8;
}

v_io_size Http2Processor::sendGoawayFrame(Http2Processor::ProcessingResources &resources,
                                          v_uint32 lastStream,
                                          protocol::http2::error::ErrorCode errorCode) {
  FrameHeader header(8, 0, FrameType::GOAWAY, 0);
  OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, streamId:%lu, ErrorCode:%s(%d))", FrameHeader::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId(), protocol::http2::error::stringRepresentation(errorCode), errorCode);
  lastStream = htonl(lastStream);
  v_uint32 errorCodeU32 = htonl(errorCode);
  resources.outStream->lock(PriorityStreamScheduler::PRIORITY_MAX);
  header.writeToStream(resources.outStream.get());
  resources.outStream->writeExactSizeDataSimple(&lastStream, 4);
  resources.outStream->writeExactSizeDataSimple(&errorCodeU32, 4);
  resources.outStream->unlock();

  return FrameHeader::HeaderSize;
}

v_io_size Http2Processor::sendResetStreamFrame(Http2Processor::ProcessingResources &resources,
                                               v_uint32 stream,
                                               protocol::http2::error::ErrorCode errorCode) {
  FrameHeader header(4, 0, FrameType::RST_STREAM, stream);
  OATPP_LOGD(TAG, "Sending %s (length:%lu, flags:0x%02x, streamId:%lu, errorCode:%s(%d))", FrameHeader::frameTypeStringRepresentation(header.getType()), header.getLength(), header.getFlags(), header.getStreamId(), protocol::http2::error::stringRepresentation(errorCode), errorCode);
  v_uint32 errorCodeU32 = htonl((v_uint32)errorCode);
  resources.outStream->lock(PriorityStreamScheduler::PRIORITY_MAX);
  header.writeToStream(resources.outStream.get());
  resources.outStream->writeExactSizeDataSimple(&errorCodeU32, 4);
  resources.outStream->unlock();

  return FrameHeader::HeaderSize;
}

v_io_size Http2Processor::consumeStream(const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                        v_io_size streamPayloadLength) {
  v_io_size consumed = 0;
  while (consumed < streamPayloadLength) {
    /*
     * ToDo: Discussion: We should consume the frame when its erroneous
     * But what is the best way to do that?
     */
    v_io_size chunk = std::min((v_io_size) 1024, streamPayloadLength - consumed);
    oatpp::String memory(chunk);
    data::buffer::InlineReadData inlinereader((void *) memory->data(), chunk);
    consumed += stream->readExactSizeDataSimple(inlinereader);
  }
  return consumed;
}

std::shared_ptr<Http2StreamHandler> Http2Processor::findOrCreateStream(v_uint32 ident,
                                                                       ProcessingResources &resources) {
  std::shared_ptr<Http2StreamHandler> handler;
  auto handlerentry = resources.h2streams.find(ident);
  if (handlerentry == resources.h2streams.end()) {
    if (ident < resources.highestNonIdleStreamId) {
      throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::findOrCreateStream] Error: Tried to create a new stream with a streamId smaller than the currently highest known streamId");
    }
    handler = std::make_shared<Http2StreamHandler>(ident, resources.outStream, resources.hpack, resources.components, resources.inSettings, resources.outSettings);
    resources.h2streams.insert({ident, handler});
  } else {
    handler = handlerentry->second;
  }
  return handler;
}



Http2Processor::ConnectionState Http2Processor::processNextRequest(ProcessingResources &resources) {

  std::shared_ptr<FrameHeader> header;
  try {
    header = FrameHeader::createShared(resources.inStream);
  } catch (std::runtime_error &e) {
    return ConnectionState::DEAD;
  }

  OATPP_LOGD(TAG, "Received %s (length:%lu, flags:0x%02x, streamId:%lu)", FrameHeader::frameTypeStringRepresentation(header->getType()), header->getLength(), header->getFlags(), header->getStreamId());

  // streamId's have to be uneven numbers
  if ((header->getStreamId() != 0) && (header->getStreamId() & 0x01) == 0) {
    OATPP_LOGE(TAG, "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received even streamId (PROTOCOL_ERROR)");
    sendGoawayFrame(resources, resources.lastStream ? resources.lastStream->getStreamId() : 0, protocol::http2::error::ErrorCode::PROTOCOL_ERROR);
    return ConnectionState::DEAD;
  }

  if (resources.lastStream && (header->getStreamId() != resources.lastStream->getStreamId())) {
    // Headers have to be sent as continous frames of HEADERS, PUSH_PROMISE and CONTINUATION.
    // Only if the last opened stream transitioned beyond its HEADERS stage, other frames are allowed
    if (resources.lastStream->getState() < Http2StreamHandler::H2StreamState::PAYLOAD && resources.lastStream->getState() > Http2StreamHandler::H2StreamState::INIT) {
      OATPP_LOGE(TAG, "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Last opened stream is still in its HEADER state but frame for different stream received (PROTOCOL_ERROR)");
      sendGoawayFrame(resources, resources.lastStream->getStreamId(), protocol::http2::error::ErrorCode::PROTOCOL_ERROR);
      return ConnectionState::DEAD;
    }
  }

  try {
    switch (header->getType()) {

      case FrameType::PING:
        if (header->getStreamId() != 0) {
          consumeStream(resources.inStream, header->getLength());
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received PING frame on stream");
        }
        if (header->getLength() != 8) {
          throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received PING with invalid frame size");
        }
        // if bit(1) (ack) is not set, reply the same payload but with flag-bit(1) set
        if ((header->getFlags() & 0x01) == 0x00) {
          answerPingFrame(resources);
        } else if ((header->getFlags() & 0x01) == 0x01) {
          // all good, just consume
          consumeStream(resources.inStream, header->getLength());
        } else {
          // unknown flags! for now, just consume
          consumeStream(resources.inStream, header->getLength());
        }
        break;

      case FrameType::SETTINGS:
        if (header->getStreamId() != 0) {
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received SETTINGS frame on stream");
        }
        if (header->getFlags() == 0) {
          if (header->getLength() % 6) {
            throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received SETTINGS with invalid frame size");
          }
          for (v_uint32 consumed = 0; consumed < header->getLength(); consumed += 6) {
            v_uint16 ident;
            v_uint32 parameter;
            resources.inStream->readExactSizeDataSimple(&ident, 2);
            resources.inStream->readExactSizeDataSimple(&parameter, 4);
            try {
              resources.outSettings->setSetting((Http2Settings::Identifier) ntohs(ident), ntohl(parameter));
            } catch (protocol::http2::error::connection::ProtocolError &h2pe) {
              throw h2pe;
            } catch (protocol::http2::error::connection::FlowControlError &h2fce) {
              throw h2fce;
            } catch (std::runtime_error &e) {
              OATPP_LOGW(TAG, e.what());
            }
          }
          ackSettingsFrame(resources);
        } else if (header->getFlags() == 0x01 && header->getLength() > 0) {
          throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received SETTINGS ack with payload");
        }
        break;

      case FrameType::GOAWAY:
        if (header->getStreamId() == 0){
          v_uint32 errorCode, lastId, highest = 0;
          resources.inStream->readExactSizeDataSimple(&lastId, 4);
          resources.inStream->readExactSizeDataSimple(&errorCode, 4);
          errorCode = ntohl(errorCode);
          lastId = ntohl(lastId);
          // Consume remaining debug data.
          if (header->getLength() > 8) {
            v_uint32 remaining = header->getLength() - 8;
            while (remaining > 0) {
              v_uint32 chunk = std::min((v_uint32) 2048, remaining);
              v_char8 buf[chunk];
              resources.inStream->readExactSizeDataSimple(buf, chunk);
              remaining -= chunk;
            }
          }
  //        for (auto &handler : resources.h2streams) {
  //          if (handler.first > highest) {
  //            highest = handler.first;
  //          }
  //        }
  //        sendGoawayFrame(resources, highest, 0);
          resources.inStream->setInputStreamIOMode(data::stream::ASYNCHRONOUS);
          while (processNextRequest(resources) != ConnectionState::DEAD) {}
          return ConnectionState::CLOSING;
        } else {
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received GOAWAY on stream.");
        }

      case FrameType::WINDOW_UPDATE:
        if (header->getStreamId() == 0) {
          if (header->getLength() != 4) {
            throw protocol::http2::error::connection::FrameSizeError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received WINDOW_UPDATE with invalid frame size");
          }
          v_uint32 increment;
          resources.inStream->readExactSizeDataSimple(&increment, 4);
          increment = ntohl(increment);
          if (increment == 0) {
            throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Increment of 0");
          }
          OATPP_LOGD(TAG, "Incrementing out-window by %u", increment);
          resources.outSettings->setSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE, resources.outSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE)+increment);
        } else {
          ConnectionState state = delegateToHandler(findOrCreateStream(header->getStreamId(), resources),
                                                    resources.inStream,
                                                    resources,
                                                    *header);
          if (state != ConnectionState::ALIVE) {
            resources.h2streams.erase(header->getStreamId());
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
          throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received stream related frame on stream(0)");
        } else {
          resources.lastStream = findOrCreateStream(header->getStreamId(), resources);
          ConnectionState state = delegateToHandler(resources.lastStream, resources.inStream, resources, *header);
          if (state != ConnectionState::ALIVE) {
            resources.lastStream->clean();
          }
        }
        break;

      default:
        // ToDo: Unknown frame
        consumeStream(resources.inStream, header->getLength());
        break;
    }
  } catch (protocol::http2::error::Error &h2e) {
    OATPP_LOGE(TAG, "%s (%s)", h2e.what(), h2e.getH2ErrorCodeString());
    if (h2e.getH2ErrorScope() == protocol::http2::error::CONNECTION) {
      sendGoawayFrame(resources, header->getStreamId(), h2e.getH2ErrorCode());
      return ConnectionState::DEAD;
    } else {
      sendResetStreamFrame(resources, header->getStreamId(), h2e.getH2ErrorCode());
      return ConnectionState::ALIVE;
    }
  }

  return ConnectionState::ALIVE;
}

Http2Processor::ConnectionState Http2Processor::invalidateConnection(Http2Processor::ProcessingResources &resources) {
  return ConnectionState::DEAD;
}

void Http2Processor::stop(Http2Processor::ProcessingResources &resources) {
  for(auto & h2stream : resources.h2streams) {
    if (h2stream.second != nullptr) {
      h2stream.second->abort();
    }
  }
  for(auto it = resources.h2streams.begin(); it != resources.h2streams.end(); ++it) {
    if (it->second != nullptr) {
      it->second->waitForFinished();
    }
  }
}

Http2Processor::ConnectionState Http2Processor::delegateToHandler(const std::shared_ptr<Http2StreamHandler> &handler,
                                                                  const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                                  Http2Processor::ProcessingResources &resources,
                                                                  FrameHeader &header) {
  ConnectionState state = ConnectionState::CLOSING;

  switch (header.getType()) {
    case FrameType::DATA:

      // Check if the stream is in a state where it would accept data
      // ToDo: Discussion: Should these checks be inside their respective functions?
      if (handler->getState() != Http2StreamHandler::H2StreamState::PAYLOAD) {
        if (handler->getState() >= Http2StreamHandler::H2StreamState::PROCESSING) {
          throw protocol::http2::error::connection::StreamClosed(
              "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received data for stream that is already (half-)closed");
        }
        throw protocol::http2::error::connection::ProtocolError(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received data for stream that is not in payload state");
      }
      state = handler->handleData(header.getFlags(), resources.inStream, header.getLength());
      break;
    case FrameType::HEADERS:
      if (handler->getState() >= Http2StreamHandler::H2StreamState::PROCESSING) {
        throw protocol::http2::error::connection::StreamClosed(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received headers for stream that is already (half-)closed");
      }
      if (handler->getStreamId() > resources.highestNonIdleStreamId) {
        resources.highestNonIdleStreamId = handler->getStreamId();
      }
      state = handler->handleHeaders(header.getFlags(), resources.inStream, header.getLength());
      break;
    case FrameType::PRIORITY:
      if (handler->getState() == Http2StreamHandler::H2StreamState::HEADERS || handler->getState() == Http2StreamHandler::H2StreamState::CONTINUATION) {
        throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received PRIORITY frame while still in header state.");
      }
      state = handler->handlePriority(header.getFlags(), resources.inStream, header.getLength());
      break;
    case FrameType::RST_STREAM:
      if (handler->getState() == Http2StreamHandler::H2StreamState::INIT) {
        throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received RST_STREAM on an idle stream.");
      }
      state = handler->handleResetStream(header.getFlags(), resources.inStream, header.getLength());
      sendResetStreamFrame(resources, handler->getStreamId(), protocol::http2::error::ErrorCode::CANCEL);
      break;
    case FrameType::PUSH_PROMISE:
      state = handler->handlePushPromise(header.getFlags(), resources.inStream, header.getLength());
      break;
    case FrameType::WINDOW_UPDATE:
      if (handler->getState() == Http2StreamHandler::H2StreamState::INIT) {
        throw protocol::http2::error::connection::ProtocolError("[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received WINDOW_UPDATE on an idle stream.");
      }
      state = handler->handleWindowUpdate(header.getFlags(), resources.inStream, header.getLength());
      break;
    case FrameType::CONTINUATION:
      // Check if the stream is in its "headers received" state, i.E. the only state when continued headers should be acceptable
      if (handler->getState() != Http2StreamHandler::H2StreamState::HEADERS && handler->getState() != Http2StreamHandler::H2StreamState::CONTINUATION) {
        consumeStream(resources.inStream, header.getLength());
        throw protocol::http2::error::connection::ProtocolError(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received continued headers for stream that is not in its header state");
      }
      state = handler->handleContinuation(header.getFlags(), resources.inStream, header.getLength());
      break;
    default:
      state = ConnectionState::CLOSING;
      break;
  }

  return state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Task

Http2Processor::Task::Task(const std::shared_ptr<processing::Components> &components,
                           const std::shared_ptr<oatpp::data::stream::IOStream> &connection,
                           std::atomic_long *taskCounter)
    : m_components(components), m_connection(connection), m_counter(taskCounter), m_delegationParameters(nullptr) {
  (*m_counter)++;
}

Http2Processor::Task::Task(const std::shared_ptr<processing::Components> &components,
                           const std::shared_ptr<oatpp::data::stream::IOStream> &connection,
                           std::atomic_long *taskCounter,
                           const std::shared_ptr<const network::ConnectionHandler::ParameterMap> &delegationParameters)
    : m_components(components), m_connection(connection), m_counter(taskCounter), m_delegationParameters(delegationParameters) {
  (*m_counter)++;
}

Http2Processor::Task::Task(const Http2Processor::Task &copy)
    : m_components(copy.m_components), m_connection(copy.m_connection), m_counter(copy.m_counter) {
  (*m_counter)++;
}

Http2Processor::Task::Task(Http2Processor::Task &&move)
    : m_components(std::move(move.m_components)),
      m_connection(std::move(move.m_connection)),
      m_counter(move.m_counter) {
  move.m_counter = nullptr;
}

Http2Processor::Task &Http2Processor::Task::operator=(const Http2Processor::Task &t) {
  if (this != &t) {
    m_components = t.m_components;
    m_connection = t.m_connection;
    m_counter = t.m_counter;
    (*m_counter)++;
  }
  return *this;
}

Http2Processor::Task &Http2Processor::Task::operator=(Http2Processor::Task &&t) {
  m_components = std::move(t.m_components);
  m_connection = std::move(t.m_connection);
  m_counter = t.m_counter;
  t.m_counter = nullptr;
  return *this;
}

void Http2Processor::Task::run() {

  m_connection->initContexts();

  auto inSettings = http2::Http2Settings::createShared();
  auto outSettings = http2::Http2Settings::createShared();
  auto outStream = std::make_shared<http2::PriorityStreamScheduler>(m_connection);
  auto memlabel = std::make_shared<std::string>(inSettings->getSetting(Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE), 0);
  v_io_size writePosition = 0;

  if (m_delegationParameters != nullptr) {
    auto it = m_delegationParameters->find("h2frame");
    if (it != m_delegationParameters->end()) {
      std::memcpy((void*)memlabel->data(), it->second->data(), it->second->size());
      writePosition = it->second->size() + 1;
    }
  }

  auto inStream = data::stream::InputStreamBufferedProxy::createShared(m_connection, memlabel, 0, writePosition, true);

  ProcessingResources resources(m_components, inSettings, outSettings, inStream, outStream);

  ConnectionState connectionState;

  try {

    Http2Processor::sendSettingsFrame(resources);

    do {

      connectionState = Http2Processor::processNextRequest(resources);

    } while (connectionState == ConnectionState::ALIVE || connectionState == ConnectionState::CLOSING);

    Http2Processor::stop(resources);
    Http2Processor::invalidateConnection(resources);

  } catch (...) {
    // DO NOTHING
  }

}
Http2Processor::Task::~Task() {
  if (m_counter != nullptr) {
    (*m_counter)--;
  }
}

}}}}