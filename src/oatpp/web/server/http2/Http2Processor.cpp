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

#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other

Http2Processor::ProcessingResources::ProcessingResources(const std::shared_ptr<http2::processing::Components> &pComponents,
                                                         const std::shared_ptr<oatpp::data::stream::IOStream> &pConnection)
    : components(pComponents),
      connection(pConnection),
      headersInBuffer(components->config->headersInBufferInitial),
      headersOutBuffer(components->config->headersOutBufferInitial),
      inStream(data::stream::InputStreamBufferedProxy::createShared(connection,
                                                                    std::make_shared<std::string>(data::buffer::IOBuffer::BUFFER_SIZE,
                                                                                                  0))),
      hpack(std::make_shared<protocol::http2::hpack::SimpleHpack>(std::make_shared<protocol::http2::hpack::SimpleTable>(
          1024))) {}

std::shared_ptr<protocol::http::outgoing::Response>
Http2Processor::processNextRequest(ProcessingResources &resources,
                                   const std::shared_ptr<protocol::http::incoming::Request> &request,
                                   ConnectionState &connectionState) {

  std::shared_ptr<protocol::http::outgoing::Response> response;

  try {

    for (auto &interceptor : resources.components->requestInterceptors) {
      response = interceptor->intercept(request);
      if (response) {
        return response;
      }
    }

    auto route =
        resources.components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);

    if (!route) {

      data::stream::BufferOutputStream ss;
      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
         << "', URL: '" << request->getStartingLine().path.toString() << "'";

      connectionState = ConnectionState::CLOSING;
      return resources.components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());

    }

    request->setPathVariables(route.getMatchMap());
    return route.getEndpoint()->handle(request);

  } catch (oatpp::web::protocol::http::HttpError &error) {
    response =
        resources.components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
    connectionState = ConnectionState::CLOSING;
  } catch (std::exception &error) {
    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    connectionState = ConnectionState::CLOSING;
  } catch (...) {
    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
    connectionState = ConnectionState::CLOSING;
  }

  return response;

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

Http2Processor::ConnectionState Http2Processor::processNextRequest(ProcessingResources &resources) {

  async::Action action;

  std::vector<v_uint8> data(9);
  data::buffer::InlineReadData inlineData(data.data(), 9);

  resources.inStream->readExactSizeDataSimple(inlineData);
  p_uint8 dataptr = data.data();
  v_uint32 payloadlen = (*dataptr) | ((*dataptr + 1) << 8) | ((*dataptr + 2) << 16);
  dataptr += 3;
  FrameType type = (FrameType) *dataptr++;
  v_uint8 flags = *dataptr++;
  v_uint32 streamident = ((*dataptr) & 0x7f) | ((*dataptr + 1) << 8) | ((*dataptr + 2) << 16) | ((*dataptr + 3) << 24);

  if (type == PING) {
    if (streamident != 0) {
      // connection error!
      // https://datatracker.ietf.org/doc/html/rfc7540#section-5.4.1
    }
    if (payloadlen != 8) {
      // connection error!
      // https://datatracker.ietf.org/doc/html/rfc7540#section-5.4.1
    }
    if (flags == 0x00) {
      // ToDo Priorized output lock
      data.resize(9+8);
      inlineData = data::buffer::InlineReadData(data.data()+9, 8);
      resources.inStream->readExactSizeDataSimple(inlineData);
      resources.connection->writeSimple(data.data(), data.size());
      return ConnectionState::ALIVE;
    }
  }

  std::shared_ptr<Http2StreamHandler> handler;
  auto handlerentry = resources.h2streams.find(streamident);
  if (handlerentry == resources.h2streams.end()) {
    handler = std::make_shared<Http2StreamHandler>(streamident, resources.hpack, resources.components);
    resources.h2streams.insert({streamident, handler});
  } else {
    handler = handlerentry->second;
  }

  ConnectionState state;

  switch (type) {
    case DATA:
      // Check if the stream is in a state where it would accept data
      // ToDo: Discussion: Should these checks be inside their respective functions?
      if (handler->getState() <= Http2StreamHandler::H2StreamState::INIT) {
        consumeStream(resources.inStream, payloadlen);
        throw std::runtime_error(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received data for stream that has not received any headers");
      }
      if (handler->getState() >= Http2StreamHandler::H2StreamState::GOAWAY) {
        consumeStream(resources.inStream, payloadlen);
        throw std::runtime_error(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received data for stream that is already at goaway");
      }
      state = handler->handleData(flags, resources.inStream, payloadlen);
      break;
    case HEADERS:
      // Check if the stream is in its initial state, i.E. the only state when headers should be acceptable
      // ToDo: Discussion: Should these checks be inside their respective functions?
      if (handler->getState() != Http2StreamHandler::H2StreamState::INIT) {
        consumeStream(resources.inStream, payloadlen);
        throw std::runtime_error(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received headers for stream that is not in its init state");
      }
      state = handler->handleHeaders(flags, resources.inStream, payloadlen);
      break;
    case PRIORITY:
      state = handler->handlePriority(flags, resources.inStream, payloadlen);
      break;
    case RST_STREAM:
      state = handler->handleResetStream(flags, resources.inStream, payloadlen);
      break;
    case SETTINGS:
      state = handler->handleSettings(flags, resources.inStream, payloadlen);
      break;
    case PUSH_PROMISE:
      state = handler->handlePushPromise(flags, resources.inStream, payloadlen);
      break;
    case GOAWAY:
      state = handler->handleGoAway(flags, resources.inStream, payloadlen);
      break;
    case WINDOW_UPDATE:
      state = handler->handleWindowUpdate(flags, resources.inStream, payloadlen);
      break;
    case CONTINUATION:
      // Check if the stream is in its "headers received" state, i.E. the only state when continued headers should be acceptable
      if (handler->getState() != Http2StreamHandler::H2StreamState::HEADERS) {
        consumeStream(resources.inStream, payloadlen);
        throw std::runtime_error(
            "[oatpp::web::server::http2::Http2Processor::processNextRequest] Error: Received continued headers for stream that is not in its header state");
      }
      state = handler->handleContinuation(flags, resources.inStream, payloadlen);
      break;
  }

  if (state != ConnectionState::ALIVE) {
    resources.h2streams.erase(streamident);
  }

  return ConnectionState::ALIVE;


//  oatpp::web::protocol::http::HttpError::Info error;
//  auto headersReadResult = resources.headersReader.readHeaders(resources.inStream.get(), error);
//
//  if(error.ioStatus <= 0) {
//    return ConnectionState::DEAD;
//  }
//
//  ConnectionState connectionState = ConnectionState::ALIVE;
//  std::shared_ptr<protocol::http::incoming::Request> request;
//  std::shared_ptr<protocol::http::outgoing::Response> response;
//
//  if(error.status.code != 0) {
//    response = resources.components->errorHandler->handleError(error.status, "Invalid Request Headers");
//    connectionState = ConnectionState::CLOSING;
//  } else {
//
//    request = protocol::http::incoming::Request::createShared(resources.connection,
//                                                              headersReadResult.startingLine,
//                                                              headersReadResult.headers,
//                                                              resources.inStream,
//                                                              resources.components->bodyDecoder);
//
//    response = processNextRequest(resources, request, connectionState);
//
//    try {
//
//      for (auto& interceptor : resources.components->responseInterceptors) {
//        response = interceptor->intercept(request, response);
//        if (!response) {
//          response = resources.components->errorHandler->handleError(
//            protocol::http::Status::CODE_500,
//            "Response Interceptor returned an Invalid Response - 'null'"
//          );
//          connectionState = ConnectionState::CLOSING;
//        }
//      }
//
//    } catch (...) {
//      response = resources.components->errorHandler->handleError(
//        protocol::http::Status::CODE_500,
//        "Unhandled Error in Response Interceptor"
//      );
//      connectionState = ConnectionState::CLOSING;
//    }
//
//    response->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
//    protocol::http::utils::CommunicationUtils::considerConnectionState(request, response, connectionState);
//
//    switch(connectionState) {
//
//      case ConnectionState::ALIVE :
//        response->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE);
//        break;
//
//      case ConnectionState::CLOSING:
//      case ConnectionState::DEAD:
//        response->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE);
//        break;
//
//      case ConnectionState::DELEGATED: {
//        auto handler = response->getConnectionUpgradeHandler();
//        if(handler) {
//          handler->handleConnection(resources.connection, response->getConnectionUpgradeParameters());
//          connectionState = ConnectionState::DELEGATED;
//        } else {
//          OATPP_LOGW("[oatpp::web::server::Http2Processor::processNextRequest()]", "Warning. ConnectionUpgradeHandler not set!");
//          connectionState = ConnectionState::CLOSING;
//        }
//        break;
//      }
//
//    }
//
//  }
//
//  auto contentEncoderProvider =
//    protocol::http::utils::CommunicationUtils::selectEncoder(request, resources.components->contentEncodingProviders);
//
//  response->send(resources.connection.get(), &resources.headersOutBuffer, contentEncoderProvider.get());
//
//  return connectionState;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Task

Http2Processor::Task::Task(const std::shared_ptr<Components> &components,
                           const std::shared_ptr<oatpp::data::stream::IOStream> &connection,
                           std::atomic_long *taskCounter)
    : m_components(components), m_connection(connection), m_counter(taskCounter) {
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

  ProcessingResources resources(m_components, m_connection);

  ConnectionState connectionState;

  try {

    do {

      connectionState = Http2Processor::processNextRequest(resources);

    } while (connectionState == ConnectionState::ALIVE);

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