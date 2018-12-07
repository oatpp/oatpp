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

#include "CommunicationUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
bool CommunicationUtils::considerConnectionKeepAlive(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                                     const std::shared_ptr<protocol::http::outgoing::Response>& response){
  
  if(request) {
    
    /* Set keep-alive to value specified in the client's request, if no Connection header present in response. */
    /* Set keep-alive to value specified in response otherwise */
    auto it = request->headers.find(Header::CONNECTION);
    if(it != request->headers.end() && oatpp::base::StrBuffer::equalsCI_FAST((const char*) it->second.getData(), Header::Value::CONNECTION_KEEP_ALIVE)) {
      if(response->putHeaderIfNotExists(Header::CONNECTION, it->second)){
        return true;
      } else {
        auto outKeepAlive = response->getHeaders().find(Header::CONNECTION);
        return (outKeepAlive != response->getHeaders().end() &&
                oatpp::base::StrBuffer::equalsCI_FAST((const char*)outKeepAlive->second.getData(), Header::Value::CONNECTION_KEEP_ALIVE));
      }
    }
    
    /* If protocol == HTTP/1.1 */
    /* Set HTTP/1.1 default Connection header value (Keep-Alive), if no Connection header present in response. */
    /* Set keep-alive to value specified in response otherwise */
    auto& protocol = request->startingLine.protocol;
    if(protocol.getData() != nullptr && oatpp::base::StrBuffer::equalsCI_FAST((const char*) protocol.getData(), "HTTP/1.1")) {
      if(!response->putHeaderIfNotExists(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE)) {
        auto outKeepAlive = response->getHeaders().find(Header::CONNECTION);
        return (outKeepAlive != response->getHeaders().end() &&
                oatpp::base::StrBuffer::equalsCI_FAST((const char*) outKeepAlive->second.getData(), Header::Value::CONNECTION_KEEP_ALIVE));
      }
      return true;
    }
    
  }
  
  /* If protocol != HTTP/1.1 */
  /* Set default Connection header value (Close), if no Connection header present in response. */
  /* Set keep-alive to value specified in response otherwise */
  if(!response->putHeaderIfNotExists(Header::CONNECTION, Header::Value::CONNECTION_CLOSE)) {
    auto outKeepAlive = response->getHeaders().find(Header::CONNECTION);
    return (outKeepAlive != response->getHeaders().end() &&
            oatpp::base::StrBuffer::equalsCI_FAST((const char*)outKeepAlive->second.getData(), Header::Value::CONNECTION_KEEP_ALIVE));
  }
  
  return false;
  
}
  
}}}}}
