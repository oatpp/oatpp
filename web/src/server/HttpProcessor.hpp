//
//  HttpProcessor.hpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_web_server_HttpProcessor_hpp
#define oatpp_web_server_HttpProcessor_hpp

#include "./HttpRouter.hpp"

#include "./handler/ErrorHandler.hpp"

#include "../protocol/http/incoming/Request.hpp"
#include "../protocol/http/outgoing/Response.hpp"

#include "../../../../oatpp-lib/core/src/data/stream/StreamBufferedProxy.hpp"

#include "../../../../oatpp-lib/core/src/async/Processor.hpp"
#include "../../../../oatpp-lib/core/src/async/Processor2.hpp"

namespace oatpp { namespace web { namespace server {
  
class HttpProcessor {
public:
  static const char* RETURN_KEEP_ALIVE;
public:
  
  class ConnectionState {
  public:
    SHARED_OBJECT_POOL(ConnectionState_Pool, ConnectionState, 32)
  public:
    
    static std::shared_ptr<ConnectionState> createShared(){
      return ConnectionState_Pool::allocateShared();
    }
    
    std::shared_ptr<oatpp::data::stream::IOStream> connection;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> ioBuffer;
    std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy> outStream;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> inStream;
    bool keepAlive = true;
    
  };
  
private:
  
  static oatpp::async::Action
  getResponseAsync(HttpRouter* router,
                   oatpp::os::io::Library::v_size firstReadCount,
                   const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                   const std::shared_ptr<ConnectionState>& connectionState,
                   std::shared_ptr<protocol::http::outgoing::Response>& response);
  
public:
  static bool considerConnectionKeepAlive(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                          const std::shared_ptr<protocol::http::outgoing::Response>& response);
  
  static std::shared_ptr<protocol::http::outgoing::Response>
  processRequest(HttpRouter* router,
                 const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                 const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                 void* buffer,
                 v_int32 bufferSize,
                 const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream,
                 bool& keepAlive);
  
};
  
class HttpProcessor2 : public oatpp::async::Coroutine<HttpProcessor2> {
public:
  
  class ConnectionState {
  public:
    SHARED_OBJECT_POOL(ConnectionState_Pool, ConnectionState, 32)
  public:
    
    static std::shared_ptr<ConnectionState> createShared(){
      return ConnectionState_Pool::allocateShared();
    }
    
    std::shared_ptr<oatpp::data::stream::IOStream> connection;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> ioBuffer;
    std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy> outStream;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> inStream;
    
  };
  
private:
  Action2 parseRequest(v_int32 readCount);
private:
  HttpRouter* m_router;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
  std::shared_ptr<oatpp::data::buffer::IOBuffer> m_ioBuffer;
  std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy> m_outStream;
  std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> m_inStream;
  bool m_keepAlive;
private:
  oatpp::web::server::HttpRouter::BranchRouter::Route m_currentRoute;
  std::shared_ptr<protocol::http::incoming::Request> m_currentRequest;
  std::shared_ptr<protocol::http::outgoing::Response> m_currentResponse;
public:
  
  HttpProcessor2(HttpRouter* router,
                 const std::shared_ptr<handler::ErrorHandler>& errorHandler,
                 const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                 const std::shared_ptr<oatpp::data::buffer::IOBuffer>& ioBuffer,
                 const std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy>& outStream,
                 const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& inStream)
    : m_router(router)
    , m_errorHandler(errorHandler)
    , m_connection(connection)
    , m_ioBuffer(ioBuffer)
    , m_outStream(outStream)
    , m_inStream(inStream)
    , m_keepAlive(true)
  {}
  
  Action2 act() override;
  
  Action2 onRequestFormed();
  Action2 onResponseFormed();
  Action2 doFlush();
  Action2 onRequestDone();
  
};
  
}}}

#endif /* oatpp_web_server_HttpProcessor_hpp */
