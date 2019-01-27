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

#include "ConnectionProvider.hpp"

namespace oatpp { namespace network { namespace virtual_ { namespace client {
  
std::shared_ptr<ConnectionProvider::IOStream> ConnectionProvider::getConnection() {
  auto submission = m_interface->connect();
  auto socket = submission->getSocket();
  socket->setNonBlocking(false);
  socket->setMaxAvailableToReadWrtie(m_maxAvailableToRead, m_maxAvailableToWrite);
  return socket;
}
  
oatpp::async::Action ConnectionProvider::getConnectionAsync(oatpp::async::AbstractCoroutine* parentCoroutine, AsyncCallback callback) {
  
  class ConnectCoroutine : public oatpp::async::CoroutineWithResult<ConnectCoroutine, std::shared_ptr<oatpp::data::stream::IOStream>> {
  private:
    std::shared_ptr<virtual_::Interface> m_interface;
    os::io::Library::v_size m_maxAvailableToRead;
    os::io::Library::v_size m_maxAvailableToWrite;
    std::shared_ptr<virtual_::Interface::ConnectionSubmission> m_submission;
  public:
    
    ConnectCoroutine(const std::shared_ptr<virtual_::Interface>& interface,
                     os::io::Library::v_size maxAvailableToRead,
                     os::io::Library::v_size maxAvailableToWrite)
      : m_interface(interface)
      , m_maxAvailableToRead(maxAvailableToRead)
      , m_maxAvailableToWrite(maxAvailableToWrite)
    {}
    
    Action act() override {
      m_submission = m_interface->connectNonBlocking();
      if(m_submission){
        return yieldTo(&ConnectCoroutine::obtainSocket);
      }
      return waitRetry();
    }
    
    Action obtainSocket() {
      auto socket = m_submission->getSocketNonBlocking();
      if(socket) {
        socket->setNonBlocking(true);
        socket->setMaxAvailableToReadWrtie(m_maxAvailableToRead, m_maxAvailableToWrite);
        return _return(socket);
      }
      return waitRetry();
    }
    
  };
  
  return parentCoroutine->startCoroutineForResult<ConnectCoroutine>(callback, m_interface, m_maxAvailableToRead, m_maxAvailableToWrite);
  
}
  
}}}}
