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

#ifndef oatpp_network_virtual__Interface_hpp
#define oatpp_network_virtual__Interface_hpp

#include "./Socket.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

namespace oatpp { namespace network { namespace virtual_ {
  
class Interface : public oatpp::base::Controllable {
public:
  
  class ConnectionSubmission {
  private:
    std::shared_ptr<Socket> m_socket;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_pending;
  public:
    
    ConnectionSubmission() : m_pending(true) {}
    
    void setSocket(const std::shared_ptr<Socket>& socket);
    std::shared_ptr<Socket> getSocket();
    std::shared_ptr<Socket> getSocketNonBlocking();
    bool isPending();
    
  };

private:
  std::shared_ptr<Socket> acceptSubmission(const std::shared_ptr<ConnectionSubmission>& submission);
private:
  std::mutex m_mutex;
  std::condition_variable m_condition;
  oatpp::collection::LinkedList<std::shared_ptr<ConnectionSubmission>> m_submissions;
public:
  
  static std::shared_ptr<Interface> createShared() {
    return std::make_shared<Interface>();
  }
  
  std::shared_ptr<ConnectionSubmission> connect();
  std::shared_ptr<Socket> accept();
  std::shared_ptr<Socket> acceptNonBlocking();
  
};
  
}}}

#endif /* oatpp_network_virtual__Interface_hpp */
