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
    
    ConnectionSubmission()
      : m_pending(true)
    {}
    
    void setSocket(const std::shared_ptr<Socket>& socket) {
      m_socket = socket;
      m_condition.notify_one();
    }
    
    std::shared_ptr<Socket> getSocket() {
      std::unique_lock<std::mutex> lock(m_mutex);
      while (!m_socket && m_pending) {
        m_condition.wait(lock);
      }
      return m_socket;
    }
    
    std::shared_ptr<Socket> getSocketNonBlocking() {
      return m_socket;
    }
    
    bool isPending() {
      return m_pending;
    }
    
  };

private:
  
  std::shared_ptr<Socket> acceptSubmission(const std::shared_ptr<ConnectionSubmission>& submission) {
    
    auto pipeIn = Pipe::createShared();
    auto pipeOut = Pipe::createShared();
    
    auto serverSocket = Socket::createShared(pipeIn, pipeOut);
    auto clientSocket = Socket::createShared(pipeOut, pipeIn);
    
    submission->setSocket(clientSocket);
    
    return serverSocket;
    
  }
  
private:
  std::mutex m_mutex;
  std::condition_variable m_condition;
  oatpp::collection::LinkedList<std::shared_ptr<ConnectionSubmission>> m_submissions;
public:
  
  std::shared_ptr<ConnectionSubmission> connect() {
    auto submission = std::make_shared<ConnectionSubmission>();
    
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_submissions.pushBack(submission);
    }
    
    m_condition.notify_one();
    return submission;
  }
  
  std::shared_ptr<Socket> accept() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_submissions.getFirstNode() == nullptr) {
      m_condition.wait(lock);
    }
    return acceptSubmission(m_submissions.popFront());
  }
  
  std::shared_ptr<Socket> acceptNonBlocking() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_submissions.getFirstNode() == nullptr) {
      return acceptSubmission(m_submissions.popFront());
    }
    return nullptr;
  }
  
};
  
}}}

#endif /* oatpp_network_virtual__Interface_hpp */
