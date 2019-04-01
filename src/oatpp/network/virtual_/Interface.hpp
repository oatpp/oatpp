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

#ifndef oatpp_network_virtual__Interface_hpp
#define oatpp_network_virtual__Interface_hpp

#include "./Socket.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

namespace oatpp { namespace network { namespace virtual_ {

/**
 * "Virtual" Interface provides functionality for accepting "virtual" connections.
 * "virtual" connection is represented by &id:oatpp::network::virtual_::Socket;.
 */
class Interface : public oatpp::base::Countable {
public:

  /**
   * "Future" for &id:oatpp::network::virtual_::Socket;.
   */
  class ConnectionSubmission {
  private:
    std::shared_ptr<Socket> m_socket;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_valid;
  public:

    /**
     * Constructor.
     */
    ConnectionSubmission() : m_valid(true) {}

    /**
     * Set socket to be returned in call to &l:Interface::ConnectionSubmission::getSocket ();/&l:Interface::ConnectionSubmission::getSocketNonBlocking ();.
     * @param socket - &id:oatpp::network::virtual_::Socket;.
     */
    void setSocket(const std::shared_ptr<Socket>& socket);

    /**
     * Block and wait for socket.
     * @return - `std::shared_ptr` to &id:oatpp::network::virtual_::Socket;.
     */
    std::shared_ptr<Socket> getSocket();

    /**
     * Check if socket already available.
     * User should repeat call if `(!socket && isPending())`.
     * @return - `std::shared_ptr` to &id:oatpp::network::virtual_::Socket;.
     */
    std::shared_ptr<Socket> getSocketNonBlocking();

    /**
     * Check if submission is still valid.
     * @return - `true` if still valid.
     */
    bool isValid();
    
  };

private:
  std::shared_ptr<Socket> acceptSubmission(const std::shared_ptr<ConnectionSubmission>& submission);
private:
  oatpp::String m_name;
  std::mutex m_mutex;
  std::condition_variable m_condition;
  oatpp::collection::LinkedList<std::shared_ptr<ConnectionSubmission>> m_submissions;
public:
  /**
   * Constructor.
   * @param name - interface name.
   */
  Interface(const oatpp::String& name)
    : m_name(name)
  {}
public:

  /**
   * Create shared Interface.
   * @param name  - interface name.
   * @return - `std::shared_ptr` to Interface.
   */
  static std::shared_ptr<Interface> createShared(const oatpp::String& name) {
    return std::make_shared<Interface>(name);
  }

  /**
   * Connect to interface.
   * @return - &l:Interface::ConnectionSubmission;.
   */
  std::shared_ptr<ConnectionSubmission> connect();

  /**
   * Connect to interface.
   * @return - &l:Interface::ConnectionSubmission; on success. Empty `std::shared_ptr` on failure.
   */
  std::shared_ptr<ConnectionSubmission> connectNonBlocking();

  /**
   * Block and wait for incloming connection.
   * @param waitingHandle - reference to a boolean variable.
   * User may set waitingHandle = false and call &l:Interface::notifyAcceptors (); in order to break waiting loop. and exit accept() method.
   * @return - `std::shared_ptr` to &id:oatpp::network::virtual_::Socket;.
   */
  std::shared_ptr<Socket> accept(const bool& waitingHandle = true);

  /**
   * Check if incoming connection is available. NonBlocking.
   * @return - `std::shared_ptr` to &id:oatpp::network::virtual_::Socket; if available.
   * Empty `std::shared_ptr` if no incoming connection is available at the moment.
   */
  std::shared_ptr<Socket> acceptNonBlocking();

  /**
   * Notify all threads that are waiting on accept().
   * Those threads that have waitingHandle changed to false will be unblocked.
   */
  void notifyAcceptors();

  /**
   * Get interface name.
   * @return - &id:oatpp::String;.
   */
  oatpp::String getName() {
    return m_name;
  }
  
};
  
}}}

#endif /* oatpp_network_virtual__Interface_hpp */
