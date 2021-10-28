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

#include "CommunicationUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace utils {
  
void CommunicationUtils::considerConnectionState(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                                 const std::shared_ptr<protocol::http::outgoing::Response>& response,
                                                 ConnectionState& connectionState)
{

  if(connectionState != ConnectionState::ALIVE) {
    return;
  }

  auto outState = response->getHeaders().getAsMemoryLabel<oatpp::data::share::StringKeyLabelCI>(Header::CONNECTION);
  if(outState && outState == Header::Value::CONNECTION_UPGRADE) {
    connectionState = ConnectionState::DELEGATED;
    return;
  }
  
  if(request) {
    /* If the connection header is present in the request and its value isn't keep-alive, then close */
    auto connection = request->getHeaders().getAsMemoryLabel<oatpp::data::share::StringKeyLabelCI>(Header::CONNECTION);
    if(connection) {
      if(connection != Header::Value::CONNECTION_KEEP_ALIVE) {
        connectionState = ConnectionState::CLOSING;
      }
      return;
    }
    
    /* If protocol == HTTP/1.1 */
    /* Set HTTP/1.1 default Connection header value (Keep-Alive), if no Connection header present in response. */
    /* Set keep-alive to value specified in response otherwise */
    auto& protocol = request->getStartingLine().protocol;
    if(protocol && oatpp::utils::String::compareCI_ASCII(protocol.getData(), protocol.getSize(), "HTTP/1.1", 8) == 0) {
      if(outState && outState != Header::Value::CONNECTION_KEEP_ALIVE) {
        connectionState = ConnectionState::CLOSING;
      }
      return;
    }
  }
  
  /* If protocol != HTTP/1.1 */
  /* Set default Connection header value (Close), if no Connection header present in response. */
  /* Set keep-alive to value specified in response otherwise */
  if(!outState || outState != Header::Value::CONNECTION_KEEP_ALIVE) {
    connectionState = ConnectionState::CLOSING;
  }

  return;
  
}

std::shared_ptr<encoding::EncoderProvider>
CommunicationUtils::selectEncoder(const std::shared_ptr<http::incoming::Request>& request,
                                  const std::shared_ptr<http::encoding::ProviderCollection>& providers)
{
  if(providers && request) {

    auto suggested = request->getHeaders().getAsMemoryLabel<oatpp::data::share::StringKeyLabel>(Header::ACCEPT_ENCODING);

    if(suggested) {

      http::HeaderValueData valueData;
      http::Parser::parseHeaderValueData(valueData, suggested, ',');

      return providers->get(valueData.tokens);

    }

  }

  return nullptr;

}
  
}}}}}

