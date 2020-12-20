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

#include "HttpProcessor.hpp"

#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace server {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Components

HttpProcessor::Components::Components(const std::shared_ptr<HttpRouter>& pRouter,
                                      const std::shared_ptr<protocol::http::encoding::ProviderCollection>& pContentEncodingProviders,
                                      const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& pBodyDecoder,
                                      const std::shared_ptr<handler::ErrorHandler>& pErrorHandler,
                                      const RequestInterceptors& pRequestInterceptors,
                                      const ResponseInterceptors& pResponseInterceptors,
                                      const std::shared_ptr<Config>& pConfig)
  : router(pRouter)
  , contentEncodingProviders(pContentEncodingProviders)
  , bodyDecoder(pBodyDecoder)
  , errorHandler(pErrorHandler)
  , requestInterceptors(pRequestInterceptors)
  , responseInterceptors(pResponseInterceptors)
  , config(pConfig)
{}

HttpProcessor::Components::Components(const std::shared_ptr<HttpRouter>& pRouter)
  : Components(pRouter,
               nullptr,
               std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(),
               handler::DefaultErrorHandler::createShared(),
               {},
               {},
               std::make_shared<Config>())
{}

HttpProcessor::Components::Components(const std::shared_ptr<HttpRouter>& pRouter, const std::shared_ptr<Config>& pConfig)
  : Components(pRouter,
               nullptr,
               std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(),
               handler::DefaultErrorHandler::createShared(),
               {},
               {},
               pConfig)
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other

HttpProcessor::ProcessingResources::ProcessingResources(const std::shared_ptr<Components>& pComponents,
                                                        const std::shared_ptr<oatpp::data::stream::IOStream>& pConnection)
  : components(pComponents)
  , connection(pConnection)
  , headersInBuffer(components->config->headersInBufferInitial)
  , headersOutBuffer(components->config->headersOutBufferInitial)
  , headersReader(&headersInBuffer, components->config->headersReaderChunkSize, components->config->headersReaderMaxSize)
  , inStream(data::stream::InputStreamBufferedProxy::createShared(connection, base::StrBuffer::createShared(data::buffer::IOBuffer::BUFFER_SIZE)))
{}

std::shared_ptr<protocol::http::outgoing::Response>
HttpProcessor::processNextRequest(ProcessingResources& resources,
                                  const std::shared_ptr<protocol::http::incoming::Request>& request,
                                  ConnectionState& connectionState)
{

  std::shared_ptr<protocol::http::outgoing::Response> response;

  try{

    for(auto& interceptor : resources.components->requestInterceptors) {
      response = interceptor->intercept(request);
      if(response) {
        return response;
      }
    }

    auto route = resources.components->router->getRoute(request->getStartingLine().method, request->getStartingLine().path);

    if(!route) {

      data::stream::BufferOutputStream ss;
      ss << "No mapping for HTTP-method: '" << request->getStartingLine().method.toString()
      << "', URL: '" << request->getStartingLine().path.toString() << "'";

      connectionState = ConnectionState::CLOSING;
      return resources.components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());

    }

    request->setPathVariables(route.getMatchMap());
    return route.getEndpoint()->handle(request);

  } catch (oatpp::web::protocol::http::HttpError& error) {
    response = resources.components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
    connectionState = ConnectionState::CLOSING;
  } catch (std::exception& error) {
    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    connectionState = ConnectionState::CLOSING;
  } catch (...) {
    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unhandled Error");
    connectionState = ConnectionState::CLOSING;
  }

  return response;

}

HttpProcessor::ConnectionState HttpProcessor::processNextRequest(ProcessingResources& resources) {

  oatpp::web::protocol::http::HttpError::Info error;
  auto headersReadResult = resources.headersReader.readHeaders(resources.inStream.get(), error);

  if(error.ioStatus <= 0) {
    return ConnectionState::DEAD;
  }

  ConnectionState connectionState = ConnectionState::ALIVE;
  std::shared_ptr<protocol::http::incoming::Request> request;
  std::shared_ptr<protocol::http::outgoing::Response> response;

  if(error.status.code != 0) {
    response = resources.components->errorHandler->handleError(error.status, "Invalid Request Headers");
    connectionState = ConnectionState::CLOSING;
  } else {

    request = protocol::http::incoming::Request::createShared(resources.connection,
                                                              headersReadResult.startingLine,
                                                              headersReadResult.headers,
                                                              resources.inStream,
                                                              resources.components->bodyDecoder);

    response = processNextRequest(resources, request, connectionState);

    try {

      for (auto& interceptor : resources.components->responseInterceptors) {
        response = interceptor->intercept(request, response);
        if (!response) {
          response = resources.components->errorHandler->handleError(
            protocol::http::Status::CODE_500,
            "Response Interceptor returned an Invalid Response - 'null'"
          );
          connectionState = ConnectionState::CLOSING;
        }
      }

    } catch (...) {
      response = resources.components->errorHandler->handleError(
        protocol::http::Status::CODE_500,
        "Unhandled Error in Response Interceptor"
      );
      connectionState = ConnectionState::CLOSING;
    }

    response->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
    protocol::http::utils::CommunicationUtils::considerConnectionState(request, response, connectionState);

    switch(connectionState) {

      case ConnectionState::ALIVE :
        response->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE);
        break;

      case ConnectionState::CLOSING:
      case ConnectionState::DEAD:
        response->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE);
        break;

      case ConnectionState::DELEGATED: {
        auto handler = response->getConnectionUpgradeHandler();
        if(handler) {
          handler->handleConnection(resources.connection, response->getConnectionUpgradeParameters());
          connectionState = ConnectionState::DELEGATED;
        } else {
          OATPP_LOGW("[oatpp::web::server::HttpProcessor::processNextRequest()]", "Warning. ConnectionUpgradeHandler not set!");
          connectionState = ConnectionState::CLOSING;
        }
        break;
      }

    }

  }

  auto contentEncoderProvider =
    protocol::http::utils::CommunicationUtils::selectEncoder(request, resources.components->contentEncodingProviders);

  response->send(resources.connection.get(), &resources.headersOutBuffer, contentEncoderProvider.get());

  return connectionState;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Task

HttpProcessor::Task::Task(const std::shared_ptr<Components>& components,
                          const std::shared_ptr<oatpp::data::stream::IOStream>& connection)
  : m_components(components)
  , m_connection(connection)
{}

void HttpProcessor::Task::run(){

  m_connection->initContexts();

  ProcessingResources resources(m_components, m_connection);

  ConnectionState connectionState;

  try {

    do {

      connectionState = HttpProcessor::processNextRequest(resources);

    } while (connectionState == ConnectionState::ALIVE);

  } catch (...) {
    // DO NOTHING
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HttpProcessor::Coroutine

HttpProcessor::Coroutine::Coroutine(const std::shared_ptr<Components>& components,
                                    const std::shared_ptr<oatpp::data::stream::IOStream>& connection)
  : m_components(components)
  , m_connection(connection)
  , m_headersInBuffer(components->config->headersInBufferInitial)
  , m_headersReader(&m_headersInBuffer, components->config->headersReaderChunkSize, components->config->headersReaderMaxSize)
  , m_headersOutBuffer(std::make_shared<oatpp::data::stream::BufferOutputStream>(components->config->headersOutBufferInitial))
  , m_inStream(data::stream::InputStreamBufferedProxy::createShared(m_connection, base::StrBuffer::createShared(data::buffer::IOBuffer::BUFFER_SIZE)))
  , m_connectionState(ConnectionState::ALIVE)
{}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::act() {
  return m_connection->initContextsAsync().next(yieldTo(&HttpProcessor::Coroutine::parseHeaders));
}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::parseHeaders() {
  return m_headersReader.readHeadersAsync(m_inStream).callbackTo(&HttpProcessor::Coroutine::onHeadersParsed);
}

oatpp::async::Action HttpProcessor::Coroutine::onHeadersParsed(const RequestHeadersReader::Result& headersReadResult) {

  m_currentRequest = protocol::http::incoming::Request::createShared(m_connection,
                                                                     headersReadResult.startingLine,
                                                                     headersReadResult.headers,
                                                                     m_inStream,
                                                                     m_components->bodyDecoder);

  for(auto& interceptor : m_components->requestInterceptors) {
    m_currentResponse = interceptor->intercept(m_currentRequest);
    if(m_currentResponse) {
      return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
    }
  }

  m_currentRoute = m_components->router->getRoute(headersReadResult.startingLine.method.toString(), headersReadResult.startingLine.path.toString());

  if(!m_currentRoute) {

    data::stream::BufferOutputStream ss;
    ss << "No mapping for HTTP-method: '" << headersReadResult.startingLine.method.toString()
       << "', URL: '" << headersReadResult.startingLine.path.toString() << "'";
    m_currentResponse = m_components->errorHandler->handleError(protocol::http::Status::CODE_404, ss.toString());
    m_connectionState = ConnectionState::CLOSING;
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
  }

  m_currentRequest->setPathVariables(m_currentRoute.getMatchMap());

  return yieldTo(&HttpProcessor::Coroutine::onRequestFormed);

}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestFormed() {
  return m_currentRoute.getEndpoint()->handleAsync(m_currentRequest).callbackTo(&HttpProcessor::Coroutine::onResponse);
}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onResponse(const std::shared_ptr<protocol::http::outgoing::Response>& response) {
  m_currentResponse = response;
  return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onResponseFormed() {

  for(auto& interceptor : m_components->responseInterceptors) {
    m_currentResponse = interceptor->intercept(m_currentRequest, m_currentResponse);
    if(!m_currentResponse) {
      m_currentResponse = m_components->errorHandler->handleError(
        protocol::http::Status::CODE_500,
        "Response Interceptor returned an Invalid Response - 'null'"
      );
    }
  }

  m_currentResponse->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
  oatpp::web::protocol::http::utils::CommunicationUtils::considerConnectionState(m_currentRequest, m_currentResponse, m_connectionState);

  switch(m_connectionState) {

    case ConnectionState::ALIVE :
      m_currentResponse->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE);
      break;

    case ConnectionState::CLOSING:
    case ConnectionState::DEAD:
      m_currentResponse->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE);
      break;

    case ConnectionState::DELEGATED: {
      auto handler = m_currentResponse->getConnectionUpgradeHandler();
      if(handler) {
        handler->handleConnection(m_connection, m_currentResponse->getConnectionUpgradeParameters());
        m_connectionState = ConnectionState::DELEGATED;
      } else {
        OATPP_LOGW("[oatpp::web::server::HttpProcessor::Coroutine::onResponseFormed()]", "Warning. ConnectionUpgradeHandler not set!");
        m_connectionState = ConnectionState::CLOSING;
      }
      break;
    }

  }

  auto contentEncoderProvider =
    protocol::http::utils::CommunicationUtils::selectEncoder(m_currentRequest, m_components->contentEncodingProviders);

  return protocol::http::outgoing::Response::sendAsync(m_currentResponse, m_connection, m_headersOutBuffer, contentEncoderProvider)
         .next(yieldTo(&HttpProcessor::Coroutine::onRequestDone));

}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestDone() {
  
  if(m_connectionState == ConnectionState::ALIVE) {
    return yieldTo(&HttpProcessor::Coroutine::parseHeaders);
  }
  
  return finish();

}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::handleError(Error* error) {

  if(error) {

    if(error->is<oatpp::AsyncIOError>()) {
      auto aioe = static_cast<oatpp::AsyncIOError*>(error);
      if(aioe->getCode() == oatpp::IOError::BROKEN_PIPE) {
        return aioe; // do not report BROKEN_PIPE error
      }
    }

    if(m_currentResponse) {
      //OATPP_LOGE("[oatpp::web::server::HttpProcessor::Coroutine::handleError()]", "Unhandled error. '%s'. Dropping connection", error->what());
      return error;
    }

    m_currentResponse = m_components->errorHandler->handleError(protocol::http::Status::CODE_500, error->what());
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);

  }

  return error;

}
  
}}}
