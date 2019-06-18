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

#include "Server.hpp"

#include <thread>
#include <chrono>

namespace oatpp { namespace network { namespace server {
  
const v_int32 Server::STATUS_CREATED = 0;
const v_int32 Server::STATUS_RUNNING = 1;
const v_int32 Server::STATUS_STOPPING = 2;
const v_int32 Server::STATUS_DONE = 3;

Server::Server(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
               const std::shared_ptr<ConnectionHandler>& connectionHandler)
  : m_status(STATUS_CREATED)
  , m_connectionProvider(connectionProvider)
  , m_connectionHandler(connectionHandler)
{}

void Server::mainLoop(){
  
  setStatus(STATUS_CREATED, STATUS_RUNNING);

  std::shared_ptr<const std::unordered_map<oatpp::String, oatpp::String>> params;

  while(getStatus() == STATUS_RUNNING) {
    
    auto connection = m_connectionProvider->getConnection();
    
    if (connection) {
      if(getStatus() == STATUS_RUNNING){
        m_connectionHandler->handleConnection(connection, params /* null params */);
      } else {
        OATPP_LOGD("Server", "Already stopped. Closing connection...");
      }
    }
    
  }
  
  setStatus(STATUS_DONE);
  
}

void Server::run(){
  mainLoop();
}
  
void Server::stop(){
  setStatus(STATUS_STOPPING);
}

bool Server::setStatus(v_int32 expectedStatus, v_int32 newStatus){
  v_int32 expected = expectedStatus;
  return m_status.compare_exchange_weak(expected, newStatus);
}
  
void Server::setStatus(v_int32 status){
  m_status.store(status);
}
  
v_int32 Server::getStatus(){
  return m_status.load();
}


}}}
