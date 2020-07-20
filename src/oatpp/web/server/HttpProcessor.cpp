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

namespace oatpp { namespace web { namespace server {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Components

HttpProcessor::Components::Components(const std::shared_ptr<HttpRouter>& pRouter,
                                      const std::shared_ptr<protocol::http::encoding::ProviderCollection>& pContentEncodingProviders,
                                      const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& pBodyDecoder,
                                      const std::shared_ptr<handler::ErrorHandler>& pErrorHandler,
                                      const std::shared_ptr<RequestInterceptors>& pRequestInterceptors,
                                      const std::shared_ptr<Config>& pConfig)
  : router(pRouter)
  , contentEncodingProviders(pContentEncodingProviders)
  , bodyDecoder(pBodyDecoder)
  , errorHandler(pErrorHandler)
  , requestInterceptors(pRequestInterceptors)
  , config(pConfig)
{}

HttpProcessor::Components::Components(const std::shared_ptr<HttpRouter>& pRouter)
  : Components(pRouter,
               nullptr,
               std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(),
               handler::DefaultErrorHandler::createShared(),
               std::make_shared<RequestInterceptors>(),
               std::make_shared<Config>())
{}

HttpProcessor::Components::Components(const std::shared_ptr<HttpRouter>& pRouter, const std::shared_ptr<Config>& pConfig)
  : Components(pRouter,
               nullptr,
               std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(),
               handler::DefaultErrorHandler::createShared(),
               std::make_shared<RequestInterceptors>(),
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

bool HttpProcessor::processNextRequest(ProcessingResources& resources) {

  oatpp::web::protocol::http::HttpError::Info error;
  auto headersReadResult = resources.headersReader.readHeaders(resources.inStream.get(), error);

  if(error.status.code != 0) {
    auto response = resources.components->errorHandler->handleError(error.status, "Invalid request headers");
    response->send(resources.connection.get(), &resources.headersOutBuffer, nullptr);
    return false;
  }

  if(error.ioStatus <= 0) {
    return false; // connection is in invalid state. should be dropped
  }

  auto route = resources.components->router->getRoute(headersReadResult.startingLine.method, headersReadResult.startingLine.path);

  if(!route) {
    auto response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
    response->send(resources.connection.get(), &resources.headersOutBuffer, nullptr);
    return false;
  }

  auto request = protocol::http::incoming::Request::createShared(resources.connection,
                                                                 headersReadResult.startingLine,
                                                                 route.matchMap,
                                                                 headersReadResult.headers,
                                                                 resources.inStream,
                                                                 resources.components->bodyDecoder);

  std::shared_ptr<protocol::http::outgoing::Response> response;

  try{

    auto currInterceptor = resources.components->requestInterceptors->getFirstNode();
    while (currInterceptor != nullptr) {
      response = currInterceptor->getData()->intercept(request);
      if(response) {
        break;
      }
      currInterceptor = currInterceptor->getNext();
    }

    if(!response) {
      response = route.getEndpoint()->handle(request);
    }

  } catch (oatpp::web::protocol::http::HttpError& error) {

    response = resources.components->errorHandler->handleError(error.getInfo().status, error.getMessage(), error.getHeaders());
    response->send(resources.connection.get(), &resources.headersOutBuffer, nullptr);
    return false;

  } catch (std::exception& error) {

    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    response->send(resources.connection.get(), &resources.headersOutBuffer, nullptr);
    return false;

  } catch (...) {
    response = resources.components->errorHandler->handleError(protocol::http::Status::CODE_500, "Unknown error");
    response->send(resources.connection.get(), &resources.headersOutBuffer, nullptr);
    return false;
  }

  response->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
  auto connectionState = protocol::http::utils::CommunicationUtils::considerConnectionState(request, response);

  auto contentEncoderProvider =
    protocol::http::utils::CommunicationUtils::selectEncoder(request, resources.components->contentEncodingProviders);

  response->send(resources.connection.get(), &resources.headersOutBuffer, contentEncoderProvider.get());

  switch(connectionState) {

    case protocol::http::utils::CommunicationUtils::CONNECTION_STATE_KEEP_ALIVE: return true;

    case protocol::http::utils::CommunicationUtils::CONNECTION_STATE_UPGRADE: {

      auto handler = response->getConnectionUpgradeHandler();
      if(handler) {
        handler->handleConnection(resources.connection, response->getConnectionUpgradeParameters());
      } else {
        OATPP_LOGW("[oatpp::web::server::HttpProcessor::processNextRequest()]", "Warning. ConnectionUpgradeHandler not set!");
      }

      return false;

    }

  }

  return false;

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

  bool wantContinue;

  try {

    do {

      wantContinue = HttpProcessor::processNextRequest(resources);

    } while (wantContinue);

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
  , m_connectionState(oatpp::web::protocol::http::utils::CommunicationUtils::CONNECTION_STATE_KEEP_ALIVE)
{}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::act() {
  return m_connection->initContextsAsync().next(yieldTo(&HttpProcessor::Coroutine::parseHeaders));
}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::parseHeaders() {
  return m_headersReader.readHeadersAsync(m_inStream).callbackTo(&HttpProcessor::Coroutine::onHeadersParsed);
}

oatpp::async::Action HttpProcessor::Coroutine::onHeadersParsed(const RequestHeadersReader::Result& headersReadResult) {

  m_currentRoute = m_components->router->getRoute(headersReadResult.startingLine.method.toString(), headersReadResult.startingLine.path.toString());

  if(!m_currentRoute) {
    m_currentResponse = m_components->errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
  }

  m_currentRequest = protocol::http::incoming::Request::createShared(m_connection,
                                                                     headersReadResult.startingLine,
                                                                     m_currentRoute.matchMap,
                                                                     headersReadResult.headers,
                                                                     m_inStream,
                                                                     m_components->bodyDecoder);

  auto currInterceptor = m_components->requestInterceptors->getFirstNode();
  while (currInterceptor != nullptr) {
    m_currentResponse = currInterceptor->getData()->intercept(m_currentRequest);
    if(m_currentResponse) {
      return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
    }
    currInterceptor = currInterceptor->getNext();
  }

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

  m_currentResponse->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
  m_connectionState = oatpp::web::protocol::http::utils::CommunicationUtils::considerConnectionState(m_currentRequest, m_currentResponse);

  auto contentEncoderProvider =
    protocol::http::utils::CommunicationUtils::selectEncoder(m_currentRequest, m_components->contentEncodingProviders);

  return protocol::http::outgoing::Response::sendAsync(m_currentResponse, m_connection, m_headersOutBuffer, contentEncoderProvider)
         .next(yieldTo(&HttpProcessor::Coroutine::onRequestDone));

}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestDone() {
  
  if(m_connectionState == oatpp::web::protocol::http::utils::CommunicationUtils::CONNECTION_STATE_KEEP_ALIVE) {
    return yieldTo(&HttpProcessor::Coroutine::parseHeaders);
  }
  
  if(m_connectionState == oatpp::web::protocol::http::utils::CommunicationUtils::CONNECTION_STATE_UPGRADE) {
    auto handler = m_currentResponse->getConnectionUpgradeHandler();
    if(handler) {
      handler->handleConnection(m_connection, m_currentResponse->getConnectionUpgradeParameters());
    } else {
      OATPP_LOGW("[oatpp::web::server::HttpProcessor::Coroutine::onRequestDone()]", "Warning. ConnectionUpgradeHandler not set!");
    }
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
