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

#ifndef oatpp_network_Server_hpp
#define oatpp_network_Server_hpp

#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/Types.hpp"

#include "oatpp/base/Countable.hpp"
#include "oatpp/Environment.hpp"

#include <atomic>
#include <thread>
#include <functional>

namespace oatpp { namespace network {

/**
 * Server calls &id:oatpp::network::ConnectionProvider::get; in the loop and passes obtained Connection
 * to &id:oatpp::network::ConnectionHandler;.
 */
class Server : public base::Countable {
private:

  static void mainLoop(Server *instance);
  void conditionalMainLoop();

  bool setStatus(v_int32 expectedStatus, v_int32 newStatus);
  void setStatus(v_int32 status);

private:

  std::atomic<v_int32> m_status;
  std::function<bool()> m_condition;
  std::thread m_thread;
  std::mutex m_mutex;

  std::shared_ptr<ConnectionProvider> m_connectionProvider;
  std::shared_ptr<ConnectionHandler> m_connectionHandler;

  bool m_threaded;
  
public:

  /**
   * Constructor.
   * @param connectionProvider - &id:oatpp::network::ConnectionProvider;.
   * @param connectionHandler - &id:oatpp::network::ConnectionHandler;.
   */
  Server(const std::shared_ptr<ConnectionProvider>& connectionProvider,
         const std::shared_ptr<ConnectionHandler>& connectionHandler);

  virtual ~Server() override;

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
   * @param connectionHandler - &id:oatpp::network::ConnectionHandler;.
   * @return - `std::shared_ptr` to Server.
   */
  static std::shared_ptr<Server> createShared(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
                                              const std::shared_ptr<ConnectionHandler>& connectionHandler){
    return std::make_shared<Server>(connectionProvider, connectionHandler);
  }

  /**
   * Call &id:oatpp::network::ConnectionProvider::getConnection; in the loop and passes obtained Connection
   * to &id:oatpp::network::ConnectionHandler;.
   * @param conditional - Function that is called every mainloop iteration to check if the server should continue to run <br>
   * Return true to let the server continue, false to shut it down.
   */
  void run(std::function<bool()> conditional = nullptr);

  /**
   * Call &id:oatpp::network::ConnectionProvider::getConnection; in the loop and passes obtained Connection
   * to &id:oatpp::network::ConnectionHandler;.
   * @param startAsNewThread - Start the server blocking (thread of callee) or non-blocking (own thread)
   * @deprecated Deprecated since 1.3.0, will be removed in the next release.
   * The new repository https://github.com/oatpp/oatpp-threaded-starter shows many configurations how to run Oat++ in its own thread.
   * From simple No-Stop to Stop-Simple and ending in Oat++ completely isolated in its own thread-scope.
   * We recommend the Stop-Simple for most applications! You can find it here: https://github.com/oatpp/oatpp-threaded-starter/blob/master/src/App_StopSimple.cpp
   * The other examples are non trivial and highly specialized on specific environments or requirements.
   * Please read the comments carefully and think about the consequences twice.
   * If someone wants to use them please get back to us in an issue in the new repository and we can assist you with them.
   * Again: These examples introduce special conditions and requirements for your code!
   */
  void run(bool startAsNewThread);

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

}}

#endif /* oatpp_network_Server_hpp */
