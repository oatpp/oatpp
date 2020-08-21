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

#include "oatpp/core/Types.hpp"

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <atomic>
#include <thread>

namespace oatpp { namespace network { namespace server {

/**
 * Server calls &id:oatpp::network::ConnectionProvider::getConnection; in the loop and passes obtained Connection
 * to &id:oatpp::network::server::ConnectionHandler;.
 */
class Server : public base::Countable {
private:

  static void mainLoop(Server *instance);
  
  bool setStatus(v_int32 expectedStatus, v_int32 newStatus);
  void setStatus(v_int32 status);

private:

  std::atomic<v_int32> m_status;
  std::thread m_thread;
  std::mutex m_mutex;

  std::shared_ptr<ServerConnectionProvider> m_connectionProvider;
  std::shared_ptr<ConnectionHandler> m_connectionHandler;

  bool m_threaded;
  
public:

  /**
   * Constructor.
   * @param connectionProvider - &id:oatpp::network::ConnectionProvider;.
   * @param connectionHandler - &id:oatpp::network::server::ConnectionHandler;.
   */
  Server(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
         const std::shared_ptr<ConnectionHandler>& connectionHandler);

  virtual ~Server();

 public:

  /**
   * Status constant.
   */
  static const v_int32 STATUS_CREATED;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_STARTING;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_RUNNING;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_STOPPING;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_DONE;

  /**
   * Create shared Server.
   * @param connectionProvider - &id:oatpp::network::ConnectionProvider;.
   * @param connectionHandler - &id:oatpp::network::server::ConnectionHandler;.
   * @return - `std::shared_ptr` to Server.
   */
  static std::shared_ptr<Server> createShared(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
                                              const std::shared_ptr<ConnectionHandler>& connectionHandler){
    return std::make_shared<Server>(connectionProvider, connectionHandler);
  }

  /**
   * Call &id:oatpp::network::ConnectionProvider::getConnection; in the loop and passes obtained Connection
   * to &id:oatpp::network::server::ConnectionHandler;.
   * @param startAsNewThread - Start the server blocking (thread of callee) or non-blocking (own thread)
   */
  void run(bool startAsNewThread = false);

  /**
   * Break server loop.
   * Note: thread can still be blocked on the &l:Server::run (); call as it may be waiting for ConnectionProvider to provide connection.
   */
  void stop();

  /**
   * Get server status.
   * @return - one of:<br>
   * <ul>
   *   <li>&l:Server::STATUS_CREATED;</li>
   *   <li>&l:Server::STATUS_RUNNING;</li>
   *   <li>&l:Server::STATUS_STOPPING;</li>
   *   <li>&l:Server::STATUS_DONE;</li>
   * </ul>
   */
  v_int32 getStatus();
  
};

  
}}}

#endif /* network_server_Server_hpp */
