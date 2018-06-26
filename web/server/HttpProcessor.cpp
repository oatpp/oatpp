/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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
#include "./HttpError.hpp"

namespace oatpp { namespace web { namespace server {
  
const char* HttpProcessor::RETURN_KEEP_ALIVE = "RETURN_KEEP_ALIVE";
  
bool HttpProcessor::considerConnectionKeepAlive(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                               const std::shared_ptr<protocol::http::outgoing::Response>& response){
  
  if(request) {
    auto& inKeepAlive = request->headers->get(protocol::http::Header::CONNECTION, nullptr);
    
    if(inKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(inKeepAlive.get(), protocol::http::Header::Value::CONNECTION_KEEP_ALIVE)) {
      if(response->headers->putIfNotExists(protocol::http::Header::CONNECTION, inKeepAlive)){
        return true;
      } else {
        auto& outKeepAlive = response->headers->get(protocol::http::Header::CONNECTION, nullptr);
        return (outKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(outKeepAlive.get(), protocol::http::Header::Value::CONNECTION_KEEP_ALIVE));
      }
    }
  }
  
  if(!response->headers->putIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE)) {
    auto& outKeepAlive = response->headers->get(protocol::http::Header::CONNECTION, nullptr);
    return (outKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(outKeepAlive.get(), protocol::http::Header::Value::CONNECTION_KEEP_ALIVE));
  }
  
  return false;
  
}

std::shared_ptr<protocol::http::outgoing::Response>
HttpProcessor::processRequest(HttpRouter* router,
                              const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                              const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                              RequestInterceptors* requestInterceptors,
                              void* buffer,
                              v_int32 bufferSize,
                              const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream,
                              bool& keepAlive) {
  
  keepAlive = false;
  auto readCount = connection->read(buffer, bufferSize);
  if(readCount > 0) {
    
    oatpp::parser::ParsingCaret caret((p_char8)buffer, bufferSize);
    auto line = protocol::http::Protocol::parseRequestStartingLine(caret);
    
    if(!line){
      return errorHandler->handleError(protocol::http::Status::CODE_400, "Can't read starting line");
    }
    
    auto route = router->getRoute(line->method, line->path);
    
    if(!route.isNull()) {
      
      oatpp::web::protocol::http::Status error;
      auto headers = protocol::http::Protocol::parseHeaders(caret, error);
      
      if(error.code != 0){
        return errorHandler->handleError(error, " Can't parse headers");
      }
      
      auto bodyStream = inStream;
      bodyStream->setBufferPosition(caret.getPosition(), (v_int32) readCount);
      
      auto request = protocol::http::incoming::Request::createShared(line, route.matchMap, headers, bodyStream);
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
          response = route.processUrl(request);
        }
      } catch (HttpError& error) {
        return errorHandler->handleError(error.getStatus(), error.getMessage());
      } catch (std::exception& error) {
        return errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
      } catch (...) {
        return errorHandler->handleError(protocol::http::Status::CODE_500, "Unknown error");
      }
      
      response->headers->putIfNotExists(protocol::http::Header::SERVER,
                                        protocol::http::Header::Value::SERVER);
      
      keepAlive = HttpProcessor::considerConnectionKeepAlive(request, response);
      return response;
      
    } else {
      return errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
    }
    
  } if(readCount == oatpp::data::stream::IOStream::ERROR_IO_NOTHING_TO_READ) {
    keepAlive = true;
  }
  
  return nullptr;
  
}
  
// HttpProcessor::Coroutine
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::parseRequest(v_int32 readCount) {
  
  oatpp::parser::ParsingCaret caret((p_char8) m_ioBuffer->getData(), readCount);
  auto line = protocol::http::Protocol::parseRequestStartingLine(caret);
  
  if(!line){
    m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_400, "Can't read starting line");
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
  }
  
  m_currentRoute = m_router->getRoute(line->method, line->path);
  
  if(m_currentRoute.isNull()) {
    m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
  }
  
  oatpp::web::protocol::http::Status error;
  auto headers = protocol::http::Protocol::parseHeaders(caret, error);
  
  if(error.code != 0){
    m_currentResponse = m_errorHandler->handleError(error, " Can't parse headers");
    return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
  }
  
  auto bodyStream = m_inStream;
  bodyStream->setBufferPosition(caret.getPosition(), readCount);
  
  m_currentRequest = protocol::http::incoming::Request::createShared(line, m_currentRoute.matchMap, headers, bodyStream);
  
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
  auto readCount = m_connection->read(m_ioBuffer->getData(), m_ioBuffer->getSize());
  if(readCount > 0) {
    m_currentResponse = nullptr;
    return parseRequest((v_int32)readCount);
  } else if(readCount == oatpp::data::stream::IOStream::ERROR_IO_WAIT_RETRY) {
    return waitRetry();
  } else if(readCount == oatpp::data::stream::IOStream::ERROR_IO_RETRY) {
    return repeat();
  }
  return abort();
}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestFormed() {
  HttpRouter::BranchRouter::UrlSubscriber::AsyncCallback callback =
  static_cast<HttpRouter::BranchRouter::UrlSubscriber::AsyncCallback>(&HttpProcessor::Coroutine::onResponse);
  return m_currentRoute.processUrlAsync(this, callback, m_currentRequest);
}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onResponse(const std::shared_ptr<protocol::http::outgoing::Response>& response) {
  m_currentResponse = response;
  return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onResponseFormed() {
  
  m_currentResponse->headers->putIfNotExists(protocol::http::Header::SERVER,
                                             protocol::http::Header::Value::SERVER);
  m_keepAlive = HttpProcessor::considerConnectionKeepAlive(m_currentRequest, m_currentResponse);
  m_outStream->setBufferPosition(0, 0);
  return m_currentResponse->sendAsync(this,
                                      m_outStream->flushAsync(
                                                              this,
                                                              yieldTo(&HttpProcessor::Coroutine::onRequestDone)),
                                      m_outStream);
  
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestDone() {
  if(m_keepAlive) {
    return yieldTo(&HttpProcessor::Coroutine::act);
  }
  return abort();
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::handleError(const oatpp::async::Error& error) {
  if(m_currentResponse) {
    if(error.isExceptionThrown) {
      OATPP_LOGD("Server", "Unhandled exception. Dropping connection");
    } else {
      OATPP_LOGD("Server", "Unhandled error. '%s'. Dropping connection", error.message);
    }
    return abort();
  }
  if (error.isExceptionThrown) {
    try{
      throw;
    } catch (HttpError& error) {
      m_currentResponse = m_errorHandler->handleError(error.getStatus(), error.getMessage());
    } catch (std::exception& error) {
      m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_500, error.what());
    } catch (...) {
      m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_500, "Unknown error");
    }
  } else {
    m_currentResponse = m_errorHandler->handleError(protocol::http::Status::CODE_500, error.message);
  }
  return yieldTo(&HttpProcessor::Coroutine::onResponseFormed);
}
  
}}}
