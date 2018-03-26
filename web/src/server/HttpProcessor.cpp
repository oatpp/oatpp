//
//  HttpProcessor.cpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "HttpProcessor.hpp"
#include "./HttpError.hpp"

namespace oatpp { namespace web { namespace server {
  
const char* HttpProcessor::RETURN_KEEP_ALIVE = "RETURN_KEEP_ALIVE";
  
bool HttpProcessor::considerConnectionKeepAlive(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                               const std::shared_ptr<protocol::http::outgoing::Response>& response){
  
  auto& inKeepAlive = request->headers->get(protocol::http::Header::CONNECTION, nullptr);
  
  if(!inKeepAlive.isNull() && base::String::equalsCI_FAST(inKeepAlive, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE)) {
    if(response->headers->putIfNotExists(protocol::http::Header::CONNECTION, inKeepAlive)){
      return true;
    } else {
      auto& outKeepAlive = response->headers->get(protocol::http::Header::CONNECTION, nullptr);
      return (!outKeepAlive.isNull() && base::String::equalsCI_FAST(outKeepAlive, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE));
    }
  } else if(!response->headers->putIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE)) {
    auto& outKeepAlive = response->headers->get(protocol::http::Header::CONNECTION, nullptr);
    return (!outKeepAlive.isNull() && base::String::equalsCI_FAST(outKeepAlive, protocol::http::Header::Value::CONNECTION_KEEP_ALIVE));
  }
  
  return false;
  
}

std::shared_ptr<protocol::http::outgoing::Response>
HttpProcessor::processRequest(HttpRouter* router,
                              const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                              const std::shared_ptr<handler::ErrorHandler>& errorHandler,
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
        response = route.processUrl(request);
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
    
  } if(readCount == oatpp::data::stream::IOStream::ERROR_NOTHING_TO_READ) {
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
  return yieldTo(&HttpProcessor::Coroutine::onRequestFormed);
}
  
HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::act() {
  auto readCount = m_connection->read(m_ioBuffer->getData(), m_ioBuffer->getSize());
  if(readCount > 0) {
    return parseRequest((v_int32)readCount);
  } else if(readCount == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN) {
    return waitRetry();
  }
  return abort();
}

HttpProcessor::Coroutine::Action HttpProcessor::Coroutine::onRequestFormed() {
  m_currentResponse = m_currentRoute.processUrl(m_currentRequest); // TODO make async
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
  
}}}
