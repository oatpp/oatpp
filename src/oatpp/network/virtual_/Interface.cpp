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
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace network { namespace virtual_ {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ListenerLock

Interface::ListenerLock::ListenerLock(Interface* _interface)
  : m_interface(_interface)
{}

Interface::ListenerLock::~ListenerLock() {
  if(m_interface != nullptr) {
    m_interface->unbindListener(this);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface

std::recursive_mutex Interface::m_registryMutex;
std::unordered_map<oatpp::String, std::weak_ptr<Interface>> Interface::m_registry;

void Interface::registerInterface(const std::shared_ptr<Interface>& _interface) {

  std::lock_guard<std::recursive_mutex> lock(m_registryMutex);

  auto it = m_registry.find(_interface->getName());
  if(it != m_registry.end()) {
    throw std::runtime_error
    ("[oatpp::network::virtual_::Interface::registerInterface()]: Error. Interface with such name already exists - '" + *_interface->getName() + "'.");
  }

  m_registry.insert({_interface->getName(), _interface});

}

void Interface::unregisterInterface(const oatpp::String& name) {

  std::lock_guard<std::recursive_mutex> lock(m_registryMutex);

  auto it = m_registry.find(name);
  if(it == m_registry.end()) {
    throw std::runtime_error
      ("[oatpp::network::virtual_::Interface::unregisterInterface()]: Error. Interface NOT FOUND - '" + *name + "'.");
  }

  m_registry.erase(it);

}

Interface::Interface(const oatpp::String& name)
  : m_name(name)
  , m_listenerLock(nullptr)
{}

Interface::~Interface() {

  unregisterInterface(getName());

  {
    std::lock_guard<std::mutex> lock(m_listenerMutex);
    if (m_listenerLock != nullptr) {
      OATPP_LOGe("[oatpp::network::virtual_::Interface::~Interface()]",
                 "Error! Interface destructor called, but listener is still bonded!!!")
      m_listenerLock.load()->m_interface = nullptr;
    }
  }

  dropAllConnection();

}

std::shared_ptr<Interface> Interface::obtainShared(const oatpp::String& name) {

  std::lock_guard<std::recursive_mutex> lock(m_registryMutex);

  auto it = m_registry.find(name);
  if(it != m_registry.end()) {
    return it->second.lock();
  }

  std::shared_ptr<Interface> _interface(new Interface(name));
  registerInterface(_interface);
  return _interface;

}

void Interface::ConnectionSubmission::invalidate() {
  m_valid = false;
  m_condition.notify_all();
}

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

std::shared_ptr<Interface::ListenerLock> Interface::bind() {
  std::lock_guard<std::mutex> lock(m_listenerMutex);
  if(m_listenerLock == nullptr) {
    m_listenerLock = new ListenerLock(this);
    return std::shared_ptr<ListenerLock>(m_listenerLock.load());
  }
  throw std::runtime_error(
    "[oatpp::network::virtual_::Interface::bind()]: Can't bind to interface '" + *m_name + "'. Listener lock is already acquired");
}

void Interface::unbindListener(ListenerLock* listenerLock) {
  std::lock_guard<std::mutex> lock(m_listenerMutex);
  if(m_listenerLock != nullptr && m_listenerLock == listenerLock) {
    m_listenerLock = nullptr;
    dropAllConnection();
  } else {
    OATPP_LOGe("[oatpp::network::virtual_::Interface::unbindListener()]", "Error! Unbinding wrong listener!!!")
  }
}
  
std::shared_ptr<Interface::ConnectionSubmission> Interface::connect() {
  if(m_listenerLock != nullptr) {
    auto submission = std::make_shared<ConnectionSubmission>(true);
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_submissions.push_back(submission);
    }
    m_condition.notify_one();
    return submission;
  }
  return std::make_shared<ConnectionSubmission>(false);
}
  
std::shared_ptr<Interface::ConnectionSubmission> Interface::connectNonBlocking() {
  if(m_listenerLock != nullptr) {
    std::shared_ptr<ConnectionSubmission> submission;
    {
      std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
      if (lock.owns_lock()) {
        submission = std::make_shared<ConnectionSubmission>(true);
        m_submissions.push_back(submission);
      }
    }
    if (submission) {
      m_condition.notify_one();
    }
    return submission;
  }
  return std::make_shared<ConnectionSubmission>(false);
}

std::shared_ptr<Socket> Interface::accept(const bool& waitingHandle,
                                          const std::chrono::duration<v_int64, std::micro>& timeout) {

  auto startTime = std::chrono::system_clock::now();
  std::unique_lock<std::mutex> lock(m_mutex);
  while (waitingHandle && m_submissions.empty() && std::chrono::system_clock::now() - startTime < timeout) {
    m_condition.wait_for(lock, std::chrono::milliseconds (100));
  }
  if(!waitingHandle || m_submissions.empty()) {
    return nullptr;
  }
  const auto submission = m_submissions.front();
  m_submissions.pop_front();
  return acceptSubmission(submission);
}

std::shared_ptr<Socket> Interface::acceptNonBlocking() {
  std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
  if(lock.owns_lock() && !m_submissions.empty()) {
    const auto submission = m_submissions.front();
    m_submissions.pop_front();
    return acceptSubmission(submission);
  }
  return nullptr;
}

void Interface::dropAllConnection() {

  std::unique_lock<std::mutex> lock(m_mutex);

  for (const auto& submission : m_submissions) {
    submission->invalidate();
  }
  m_submissions.clear();

}

void Interface::notifyAcceptors() {
  m_condition.notify_all();
}
  
}}}
