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

#include "Interface.hpp"

namespace oatpp { namespace network { namespace virtual_ {
  
void Interface::ConnectionSubmission::setSocket(const std::shared_ptr<Socket>& socket) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_socket = socket;
  }
  m_condition.notify_one();
}

std::shared_ptr<Socket> Interface::ConnectionSubmission::getSocket() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (!m_socket && m_valid) {
    m_condition.wait(lock);
  }
  return m_socket;
}

std::shared_ptr<Socket> Interface::ConnectionSubmission::getSocketNonBlocking() {
  if(m_valid) {
    std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
    if(lock.owns_lock()) {
      return m_socket;
    }
  }
  return nullptr;
}

bool Interface::ConnectionSubmission::isValid() {
  return m_valid;
}
  
std::shared_ptr<Socket> Interface::acceptSubmission(const std::shared_ptr<ConnectionSubmission>& submission) {
  
  auto pipeIn = Pipe::createShared();
  auto pipeOut = Pipe::createShared();

  auto serverSocket = Socket::createShared(pipeIn, pipeOut);
  auto clientSocket = Socket::createShared(pipeOut, pipeIn);
  
  submission->setSocket(clientSocket);
  
  return serverSocket;
  
}
  
std::shared_ptr<Interface::ConnectionSubmission> Interface::connect() {
  auto submission = std::make_shared<ConnectionSubmission>();
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_submissions.pushBack(submission);
  }
  m_condition.notify_one();
  return submission;
}
  
std::shared_ptr<Interface::ConnectionSubmission> Interface::connectNonBlocking() {
  std::shared_ptr<ConnectionSubmission> submission;
  {
    std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
    if(lock.owns_lock()) {
      submission = std::make_shared<ConnectionSubmission>();
      m_submissions.pushBack(submission);
    }
  }
  if(submission) {
    m_condition.notify_one();
  }
  return submission;
}

std::shared_ptr<Socket> Interface::accept(const bool& waitingHandle) {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (waitingHandle && m_submissions.getFirstNode() == nullptr) {
    m_condition.wait(lock);
  }
  if(!waitingHandle) {
    return nullptr;
  }
  return acceptSubmission(m_submissions.popFront());
}

std::shared_ptr<Socket> Interface::acceptNonBlocking() {
  std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
  if(lock.owns_lock() && m_submissions.getFirstNode() != nullptr) {
    return acceptSubmission(m_submissions.popFront());
  }
  return nullptr;
}

void Interface::notifyAcceptors() {
  m_condition.notify_all();
}
  
}}}
