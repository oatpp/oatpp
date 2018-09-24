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
  
  /* Set keep-alive to value specified in the client's request, if no Connection header present in response. */
  /* Set keep-alive to value specified in response otherwise */
  if(request) {
    auto& inKeepAlive = request->headers->get(String(Header::CONNECTION, false), nullptr);
    
    if(inKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(inKeepAlive.get(), Header::Value::CONNECTION_KEEP_ALIVE)) {
      if(response->headers->putIfNotExists(String(Header::CONNECTION, false), inKeepAlive)){
        return true;
      } else {
        auto& outKeepAlive = response->headers->get(Header::CONNECTION, nullptr);
        return (outKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(outKeepAlive.get(), Header::Value::CONNECTION_KEEP_ALIVE));
      }
    }
  }
  
  /* If protocol == HTTP/1.1 */
  /* Set HTTP/1.1 default Connection header value (Keep-Alive), if no Connection header present in response. */
  /* Set keep-alive to value specified in response otherwise */
  String& protocol = request->startingLine->protocol;
  if(protocol && oatpp::base::StrBuffer::equalsCI_FAST(protocol.get(), "HTTP/1.1")) {
    if(!response->headers->putIfNotExists(String(Header::CONNECTION, false), String(Header::Value::CONNECTION_KEEP_ALIVE, false))) {
      auto& outKeepAlive = response->headers->get(String(Header::CONNECTION, false), nullptr);
      return (outKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(outKeepAlive.get(), Header::Value::CONNECTION_KEEP_ALIVE));
    }
    return true;
  }
  
  /* If protocol != HTTP/1.1 */
  /* Set default Connection header value (Close), if no Connection header present in response. */
  /* Set keep-alive to value specified in response otherwise */
  if(!response->headers->putIfNotExists(String(Header::CONNECTION, false), String(Header::Value::CONNECTION_CLOSE, false))) {
    auto& outKeepAlive = response->headers->get(String(Header::CONNECTION, false), nullptr);
    return (outKeepAlive && oatpp::base::StrBuffer::equalsCI_FAST(outKeepAlive.get(), Header::Value::CONNECTION_KEEP_ALIVE));
  }
  
  return false;
  
}
  
}}}}}
