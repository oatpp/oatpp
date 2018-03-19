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

std::shared_ptr<protocol::http::outgoing::Response>
HttpProcessor::getResponse(HttpRouter* router,
                           oatpp::os::io::Library::v_size firstReadCount,
                           const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                           const std::shared_ptr<ConnectionState>& connectionState) {
  
  oatpp::parser::ParsingCaret caret((p_char8) connectionState->ioBuffer->getData(), connectionState->ioBuffer->getSize());
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
    
    auto bodyStream = connectionState->inStream;
    bodyStream->setBufferPosition(caret.getPosition(), (v_int32) firstReadCount);
    
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
    
    connectionState->keepAlive = HttpProcessor::considerConnectionKeepAlive(request, response);
    return response;
    
  } else {
    return errorHandler->handleError(protocol::http::Status::CODE_404, "Current url has no mapping");
  }
}
  
oatpp::async::Action
HttpProcessor::processRequestAsync(HttpRouter* router,
                                   const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                                   const std::shared_ptr<ConnectionState>& connectionState)
{
  
  struct LocaleState {
    LocaleState(const std::shared_ptr<ConnectionState>& pConnectionState)
      : connectionState(pConnectionState)
      , ioBuffer(pConnectionState->ioBuffer->getData())
      , ioBufferSize(pConnectionState->ioBuffer->getSize())
      , retries(0)
    {}
    const std::shared_ptr<ConnectionState>& connectionState;
    void* ioBuffer;
    v_int32 ioBufferSize;
    oatpp::os::io::Library::v_size readCount;
    v_int32 retries;
  };
  
  auto state = std::make_shared<LocaleState>(connectionState);
  
  return oatpp::async::Routine::_do({
    
    [state] {
      
      //static std::atomic<v_int32> maxRetries(0);
      
      state->readCount = state->connectionState->connection->read(state->ioBuffer, state->ioBufferSize);
      if(state->readCount > 0) {
        return oatpp::async::Action(nullptr);
      } else if(state->readCount == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN){
        /*
        state->retries ++;
        if(state->retries > maxRetries){
          maxRetries = state->retries;
          OATPP_LOGD("Retry", "max=%d", maxRetries.load());
        }*/
        return oatpp::async::Action::_wait_retry();
      }
      return oatpp::async::Action::_abort();
    }, nullptr
    
  })._then({
    
    [state, router, errorHandler] {
      
      auto response = getResponse(router, state->readCount, errorHandler, state->connectionState);
      
      return oatpp::async::Routine::_do({
        
        [state, response] {
          state->connectionState->outStream->setBufferPosition(0, 0);
          return response->sendAsync(state->connectionState->outStream);
        }, nullptr
        
      })._then({
        
        [state] {
          state->connectionState->outStream->flush();
          return nullptr;
        }, nullptr
        
      });
      
    }, nullptr
    
  })._then({
    [state] {
      //OATPP_LOGD("Connection Processor", "Connection finished");
      if(state->connectionState->keepAlive){
        //OATPP_LOGD("CP", "try-keep-alive");
        oatpp::async::Error error {RETURN_KEEP_ALIVE, false};
        return oatpp::async::Action(error);
      }
      return oatpp::async::Action(nullptr);
    }, nullptr
  });
  
}
  
}}}
