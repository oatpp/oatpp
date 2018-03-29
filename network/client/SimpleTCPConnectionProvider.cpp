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

#include "./SimpleTCPConnectionProvider.hpp"

#include "oatpp/network/Connection.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include "oatpp/test/Checker.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace oatpp { namespace network { namespace client {
  
std::shared_ptr<oatpp::data::stream::IOStream> SimpleTCPConnectionProvider::getConnection(){
  
  struct hostent* host = gethostbyname((const char*) m_host->getData());
  struct sockaddr_in client;
  
  if ((host == NULL) || (host->h_addr == NULL)) {
    OATPP_LOGD("SimpleTCPConnectionProvider", "Error retrieving DNS information.");
    return nullptr;
  }
  
  bzero(&client, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_port = htons(m_port);
  memcpy(&client.sin_addr, host->h_addr, host->h_length);
  
  oatpp::os::io::Library::v_handle clientHandle = socket(AF_INET, SOCK_STREAM, 0);
  
  if (clientHandle < 0) {
    OATPP_LOGD("SimpleTCPConnectionProvider", "Error creating socket.");
    return nullptr;
  }
  
  if (connect(clientHandle, (struct sockaddr *)&client, sizeof(client)) < 0 ) {
    oatpp::os::io::Library::handle_close(clientHandle);
    OATPP_LOGD("SimpleTCPConnectionProvider", "Could not connect");
    return nullptr;
  }
  
  return oatpp::network::Connection::createShared(clientHandle);
  
}

}}}
