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

#ifndef network_server_Server_hpp
#define network_server_Server_hpp

#include "./ConnectionHandler.hpp"

#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/core/concurrency/Runnable.hpp"

#include "oatpp/core/Types.hpp"

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <atomic>

namespace oatpp { namespace network { namespace server {

class Server : public base::Countable, public concurrency::Runnable{
private:

  void mainLoop();
  
  bool setStatus(v_int32 expectedStatus, v_int32 newStatus);
  void setStatus(v_int32 status);

private:
  
  std::atomic<v_int32> m_status;
  
  oatpp::String m_port;
  
  std::shared_ptr<ServerConnectionProvider> m_connectionProvider;
  std::shared_ptr<ConnectionHandler> m_connectionHandler;
  
public:
  
  Server(
    const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
    const std::shared_ptr<ConnectionHandler>& connectionHandler
  )
    : m_status(STATUS_CREATED)
    , m_connectionProvider(connectionProvider)
    , m_connectionHandler(connectionHandler)
  {}
  
public:
  
  static const v_int32 STATUS_CREATED;
  static const v_int32 STATUS_RUNNING;
  static const v_int32 STATUS_STOPPING;
  static const v_int32 STATUS_DONE;
  
  static std::shared_ptr<Server> createShared(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
                                        const std::shared_ptr<ConnectionHandler>& connectionHandler){
    return std::make_shared<Server>(connectionProvider, connectionHandler);
  }
  
  void run() override;
  
  void stop();
  
  v_int32 getStatus();
  
};

  
}}}

#endif /* network_server_Server_hpp */
