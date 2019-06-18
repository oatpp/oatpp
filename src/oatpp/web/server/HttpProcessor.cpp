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

namespace oatpp { namespace web { namespace server {

std::shared_ptr<protocol::http::outgoing::Response>
HttpProcessor::processRequest(HttpRouter* router,
                              const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                              const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder,
                              const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                              RequestInterceptors* requestInterceptors,
                              void* buffer,
                              v_int32 bufferSize,
                              const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream,
                              v_int32& connectionState) {
  
  RequestHeadersReader headersReader(buffer, bufferSize, 4096);
  oatpp::web::protocol::http::HttpError::Info error;
  auto headersReadResult = headersReader.readHeaders(connection, error);
  
  if(error.status.code != 0) {
    connectionState = oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_CLOSE;
    return errorHandler->handleError(error.status, "Invalid request headers");
  }
  
  if(error.ioStatus <= 0) {
    connectionState = oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_CLOSE;
    return nullptr; // connection is in invalid state. should be dropped
  }
  
  auto route = router->getRoute(headersReadResult.startingLine.method, headersReadResult.startingLine.path);
  
  if(!route) {
    connectionState = oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_CLOSE;
    return errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
  }
  
  auto& bodyStream = inStream;
  bodyStream->setBufferPosition(headersReadResult.bufferPosStart,
                                headersReadResult.bufferPosEnd,
                                headersReadResult.bufferPosStart != headersReadResult.bufferPosEnd);
  
  auto request = protocol::http::incoming::Request::createShared(headersReadResult.startingLine,
                                                                 route.matchMap,
                                                                 headersReadResult.headers,
                                                                 bodyStream,
                                                                 bodyDecoder);
  
  std::shared_ptr<protocol::http::outgoing::Response> response;
  try{
    auto currInterceptor = requestInterceptors->getFirstNode();
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
    return errorHandler->handleError(error.getInfo().status, error.getMessage());
  } catch (std::exception& error) {
    return errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
  } catch (...) {
    return errorHandler->handleError(protocol::http::Status::CODE_500, "Unknown error");
  }
  
  response->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
  
  connectionState = oatpp::web::protocol::http::outgoing::CommunicationUtils::considerConnectionState(request, response);
  return response;
  
}
  
// HttpProcessor::Coroutine
  
oatpp::async::Action HttpProcessor::Coroutine::onHeadersParsed(const RequestHeadersReader::Result& headersReadResult) {
  
  m_currentRoute = m_router->getRoute(headersReadResult.startingLine.method.toString(), headersReadResult.startingLine.path.toString());
  
  if(!m_currentRoute) {
    m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
  }
  
  auto& bodyStream = m_inStream;
  bodyStream->setBufferPosition(headersReadResult.bufferPosStart,
                                headersReadResult.bufferPosEnd,
                                headersReadResult.bufferPosStart != headersReadResult.bufferPosEnd);
  
  m_currentRequest = protocol::http::incoming::Request::createShared(headersReadResult.startingLine,
                                                                     m_currentRoute.matchMap,
                                                                     headersReadResult.headers,
                                                                     bodyStream,
                                                                     m_bodyDecoder);
  
  auto currInterceptor = m_requestInterceptors->getFirstNode();
  while (currInterceptor != nullptr) {
    m_currentResponse = currInterceptor->getData()->intercept(m_currentRequest);
    if(m_currentResponse) {
      return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
    }
    currInterceptor = currInterceptor->getNext();
  }
  
  return yieldTo(&HttpProcessor::Coroutine::onRequestFormed);
  
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::act() {
  RequestHeadersReader headersReader(m_ioBuffer->getData(), m_ioBuffer->getSize(), 4096);
  return headersReader.readHeadersAsync(m_connection).callbackTo(&HttpProcessor::Coroutine::onHeadersParsed);
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
  m_connectionState = oatpp::web::protocol::http::outgoing::CommunicationUtils::considerConnectionState(m_currentRequest, m_currentResponse);
  m_outStream->setBufferPosition(0, 0, false);
  return m_currentResponse->sendAsync(m_outStream).next(m_outStream->flushAsync()).next(yieldTo(&HttpProcessor::Coroutine::onRequestDone));
  
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestDone() {
  
  if(m_connectionState == oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_KEEP_ALIVE) {
    return yieldTo(&HttpProcessor::Coroutine::act);
  }
  
  if(m_connectionState == oatpp::web::protocol::http::outgoing::CommunicationUtils::CONNECTION_STATE_UPGRADE) {
    auto handler = m_currentResponse->getConnectionUpgradeHandler();
    if(handler) {
      handler->handleConnection(m_connection, m_currentResponse->getConnectionUpgradeParameters());
    } else {
      OATPP_LOGD("[oatpp::web::server::HttpProcessor::Coroutine::onRequestDone()]", "Warning. ConnectionUpgradeHandler not set!");
    }
  }
  
  return finish();
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::handleError(const std::shared_ptr<const Error>& error) {

  if(error) {

    if(error->is<oatpp::data::AsyncIOError>()) {
      auto aioe = static_cast<const oatpp::data::AsyncIOError*>(error.get());
      if(aioe->getCode() == oatpp::data::IOError::BROKEN_PIPE) {
        return propagateError(); // do not report BROKEN_PIPE error
      }
    }

    if(m_currentResponse) {
      OATPP_LOGE("[oatpp::web::server::HttpProcessor::Coroutine::handleError()]", "Unhandled error. '%s'. Dropping connection", error->what());
      return propagateError();
    }

    m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_500, error->what());
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);

  }

  return propagateError();

}
  
}}}
