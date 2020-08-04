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
const v_int32 Server::STATUS_STARTING = 1;
const v_int32 Server::STATUS_RUNNING = 2;
const v_int32 Server::STATUS_STOPPING = 3;
const v_int32 Server::STATUS_DONE = 4;

Server::Server(const std::shared_ptr<ServerConnectionProvider> &connectionProvider,
               const std::shared_ptr<ConnectionHandler> &connectionHandler)
    : m_status(STATUS_CREATED)
    , m_connectionProvider(connectionProvider)
    , m_connectionHandler(connectionHandler)
    , m_threaded(false) {}

void Server::mainLoop(Server *instance) {

  instance->setStatus(STATUS_STARTING, STATUS_RUNNING);

  std::shared_ptr<const std::unordered_map<oatpp::String, oatpp::String>> params;

  while (instance->getStatus() == STATUS_RUNNING) {

    auto connection = instance->m_connectionProvider->getConnection();

    if (connection) {
      if (instance->getStatus() == STATUS_RUNNING) {
        instance->m_connectionHandler->handleConnection(connection, params /* null params */);
      } else {
        OATPP_LOGD("Server", "Already stopped. Closing connection...");
      }
    }

  }

  instance->setStatus(STATUS_DONE);

}

void Server::run(bool startAsNewThread) {
  std::unique_lock<std::mutex> ul(m_mutex);
  switch (getStatus()) {
    case STATUS_STARTING:
      throw std::runtime_error("[oatpp::network::server::run()] Error. Server already starting");
    case STATUS_RUNNING:
      throw std::runtime_error("[oatpp::network::server::run()] Error. Server already started");
  }
  if (startAsNewThread) {
    m_threaded = startAsNewThread;
    setStatus(STATUS_CREATED, STATUS_STARTING);
    m_thread = std::thread(mainLoop, this);
  } else {
    m_threaded = startAsNewThread;
    setStatus(STATUS_CREATED, STATUS_STARTING);
    ul.unlock(); // early unlock
    mainLoop(this);
  }
}

void Server::stop() {
  std::lock_guard<std::mutex> lg(m_mutex);
  switch (getStatus()) {
    case STATUS_CREATED:
      return;
    case STATUS_STARTING:
    case STATUS_RUNNING:
      setStatus(STATUS_STOPPING);
      break;
  }

  if (m_threaded && m_thread.joinable()) {
    m_thread.join();
  }
}

bool Server::setStatus(v_int32 expectedStatus, v_int32 newStatus) {
  v_int32 expected = expectedStatus;
  return m_status.compare_exchange_weak(expected, newStatus);
}

void Server::setStatus(v_int32 status) {
  m_status.store(status);
}

v_int32 Server::getStatus() {
  return m_status.load();
}

Server::~Server() {
  stop();
}

}}}
