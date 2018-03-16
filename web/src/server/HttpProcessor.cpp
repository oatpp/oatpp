//
//  HttpProcessor.cpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "HttpProcessor.hpp"
#include "./HttpError.hpp"
#include "../../../network/src/io/AsyncIOStream.hpp"

namespace oatpp { namespace web { namespace server {
  
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
    
  } if(readCount == oatpp::network::io::AsyncIOStream::ERROR_NOTHING_TO_READ) {
    keepAlive = true;
  }
  
  return nullptr;
  
}
  
}}}
