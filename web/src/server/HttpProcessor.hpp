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

namespace oatpp { namespace web { namespace server {
  
class HttpProcessor {
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
  
}}}

#endif /* oatpp_web_server_HttpProcessor_hpp */
