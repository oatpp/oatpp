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

#include "InterfaceTest.hpp"

#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

namespace oatpp { namespace test { namespace network { namespace virtual_ {
  
namespace {
  
  typedef oatpp::network::virtual_::Interface Interface;
  typedef oatpp::network::virtual_::Socket Socket;
  typedef oatpp::collection::LinkedList<std::shared_ptr<oatpp::concurrency::Thread>> ThreadList;
  
  class ClientTask : public oatpp::concurrency::Runnable {
  private:
    std::shared_ptr<Interface> m_interface;
    oatpp::String m_dataSample;
  public:
    
    ClientTask(const std::shared_ptr<Interface>& interface,
               const oatpp::String& dataSample)
      : m_interface(interface)
      , m_dataSample(dataSample)
    {}
    
    void run() override {
      auto submission = m_interface->connect();
      auto socket = submission->getSocket();
      
      auto res = oatpp::data::stream::writeExactSizeData(socket.get(), m_dataSample->getData(), m_dataSample->getSize());
      OATPP_ASSERT(res == m_dataSample->getSize());
      
      v_char8 buffer[100];
      auto stream = oatpp::data::stream::ChunkedBuffer::createShared();
      res = oatpp::data::stream::transfer(socket, stream, 2, buffer, 100);
      
      OATPP_ASSERT(res == 2);
      OATPP_ASSERT(stream->getSize() == res);
      OATPP_ASSERT(stream->toString() == "OK");
      
      //OATPP_LOGD("client", "finished - OK");
      
    }
    
  };
  
  class ServerTask : public oatpp::concurrency::Runnable {
  private:
    std::shared_ptr<Socket> m_socket;
    oatpp::String m_dataSample;
  public:
    
    ServerTask(const std::shared_ptr<Socket>& socket,
               const oatpp::String& dataSample)
      : m_socket(socket)
      , m_dataSample(dataSample)
    {}
    
    void run() override {
      v_char8 buffer[100];
      auto stream = oatpp::data::stream::ChunkedBuffer::createShared();
      auto res = oatpp::data::stream::transfer(m_socket, stream, m_dataSample->getSize(), buffer, 100);
      
      OATPP_ASSERT(res == m_dataSample->getSize());
      OATPP_ASSERT(stream->getSize() == res);
      OATPP_ASSERT(stream->toString() == m_dataSample);
      
      res = oatpp::data::stream::writeExactSizeData(m_socket.get(), "OK", 2);
      
      OATPP_ASSERT(res == 2);
    }
    
  };
  
  class Server : public oatpp::concurrency::Runnable {
  private:
    std::shared_ptr<Interface> m_interface;
    oatpp::String m_dataSample;
    v_int32 m_numTasks;
  public:
    
    Server(const std::shared_ptr<Interface>& interface,
           const oatpp::String& dataSample,
           v_int32 numTasks)
      : m_interface(interface)
      , m_dataSample(dataSample)
      , m_numTasks(numTasks)
    {}
    
    void run() override {
      ThreadList threadList;
      for(v_int32 i = 0; i < m_numTasks; i++) {
        auto socket = m_interface->accept();
        auto task = oatpp::concurrency::Thread::createShared(std::make_shared<ServerTask>(socket, m_dataSample));
        threadList.pushBack(task);
      }
      auto curr = threadList.getFirstNode();
      while (curr != nullptr) {
        curr->getData()->join();
        curr = curr->getNext();
      }
    }
    
  };
  
}
  
bool InterfaceTest::onRun() {
  
  oatpp::String dataSample = "1234567890-=][poiuytrewqasdfghjkl;'/.,mnbvcxzzxcvbnm,./';lkjhgfdsaqwertyuiop][=-0987654321";
  
  auto interface = Interface::createShared("virtualhost");
  v_int32 numTasks = 100;
  
  ThreadList threadList;
  
  auto server = oatpp::concurrency::Thread::createShared(std::make_shared<Server>(interface, dataSample, numTasks));
  
  for(v_int32 i = 0; i < numTasks; i++) {
    auto clientTask = oatpp::concurrency::Thread::createShared(std::make_shared<ClientTask>(interface, dataSample));
    threadList.pushBack(clientTask);
  }
  
  auto curr = threadList.getFirstNode();
  while (curr != nullptr) {
    curr->getData()->join();
    curr = curr->getNext();
  }
  
  server->join();
  
  return true;
}
  
}}}}
