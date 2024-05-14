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

#include "oatpp/data/stream/BufferStream.hpp"

#include <thread>
#include <list>

namespace oatpp { namespace test { namespace network { namespace virtual_ {
  
namespace {
  
  typedef oatpp::network::virtual_::Interface Interface;
  typedef oatpp::network::virtual_::Socket Socket;
  typedef std::list<std::thread> ThreadList;
  
  class ClientTask : public oatpp::base::Countable {
  private:
    std::shared_ptr<Interface> m_interface;
    oatpp::String m_dataSample;
  public:
    
    ClientTask(const std::shared_ptr<Interface>& _interface,
               const oatpp::String& dataSample)
      : m_interface(_interface)
      , m_dataSample(dataSample)
    {}
    
    void run() {
      auto submission = m_interface->connect();
      auto socket = submission->getSocket();
      
      auto res = socket->writeExactSizeDataSimple(m_dataSample->data(), static_cast<v_buff_size>(m_dataSample->size()));
      OATPP_ASSERT(res == static_cast<v_io_size>(m_dataSample->size()))
      
      v_char8 buffer[100];
      oatpp::data::stream::BufferOutputStream stream;
      res = oatpp::data::stream::transfer(socket.get(), &stream, 2, buffer, 100);
      
      OATPP_ASSERT(res == 2)
      OATPP_ASSERT(stream.getCurrentPosition() == res)
      OATPP_ASSERT(stream.toString() == "OK")
      
      //OATPP_LOGv("client", "finished - OK")
      
    }
    
  };
  
  class ServerTask : public oatpp::base::Countable {
  private:
    std::shared_ptr<Socket> m_socket;
    oatpp::String m_dataSample;
  public:
    
    ServerTask(const std::shared_ptr<Socket>& socket,
               const oatpp::String& dataSample)
      : m_socket(socket)
      , m_dataSample(dataSample)
    {}
    
    void run() {
      v_char8 buffer[100];
      oatpp::data::stream::BufferOutputStream stream;
      auto res = oatpp::data::stream::transfer(m_socket.get(), &stream, static_cast<v_io_size>(m_dataSample->size()), buffer, 100);
      
      OATPP_ASSERT(res == static_cast<v_io_size>(m_dataSample->size()))
      OATPP_ASSERT(stream.getCurrentPosition() == res)
      OATPP_ASSERT(stream.toString() == m_dataSample)
      
      res = m_socket->writeExactSizeDataSimple("OK", 2);
      
      OATPP_ASSERT(res == 2)
    }
    
  };
  
  class Server : public oatpp::base::Countable {
  private:
    std::shared_ptr<Interface> m_interface;
    oatpp::String m_dataSample;
    v_int32 m_numTasks;
  public:
    
    Server(const std::shared_ptr<Interface>& _interface,
           const oatpp::String& dataSample,
           v_int32 numTasks)
      : m_interface(_interface)
      , m_dataSample(dataSample)
      , m_numTasks(numTasks)
    {}
    
    void run() {
      ThreadList threadList;
      for(v_int32 i = 0; i < m_numTasks; i++) {
        auto socket = m_interface->accept();
        threadList.push_back(std::thread(&ServerTask::run, ServerTask(socket, m_dataSample)));
      }
      for(auto& thread : threadList) {
        thread.join();
      }
    }
    
  };
  
}
  
void InterfaceTest::onRun() {
  
  oatpp::String dataSample = "1234567890-=][poiuytrewqasdfghjkl;'/.,mnbvcxzzxcvbnm,./';lkjhgfdsaqwertyuiop][=-0987654321";
  
  auto _interface = Interface::obtainShared("virtualhost");
  auto bindLock = _interface->bind();

  v_int32 numTasks = 100;
  
  ThreadList threadList;
  
  std::thread server(&Server::run, Server(_interface, dataSample, numTasks));
  
  for(v_int32 i = 0; i < numTasks; i++) {
    threadList.push_back(std::thread(&ClientTask::run, ClientTask(_interface, dataSample)));
  }

  for(auto& thread : threadList) {
    thread.join();
  }
  
  server.join();

}
  
}}}}
