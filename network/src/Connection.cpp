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

#include "./Connection.hpp"

#include <sys/socket.h>
#include <thread>
#include <chrono>

namespace oatpp { namespace network {

Connection::Connection(Library::v_handle handle)
  : m_handle(handle)
{
}

Connection::~Connection(){
  close();
}

Connection::Library::v_size Connection::write(const void *buff, Library::v_size count){
  return Library::handle_write(m_handle, buff, count);
}

Connection::Library::v_size Connection::read(void *buff, Library::v_size count){
  return Library::handle_read(m_handle, buff, count);
}

v_int32 Connection::shutdown(){
  return ::shutdown(m_handle, SHUT_RDWR);
}

v_int32 Connection::shutdownRead(){
  return ::shutdown(m_handle, SHUT_RD);
}

v_int32 Connection::shutdownWrite(){
  return ::shutdown(m_handle, SHUT_WR);
}

void Connection::prepareGracefulDisconnect(){
  
  if(::shutdown(m_handle, SHUT_WR) == 0){
    
    v_int32 times = 0;
    while(::shutdown(m_handle, SHUT_WR) == 0){
      times++;
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if(times > 0){
      OATPP_LOGD("Server", "Connection tries to shutdown = %d", times);
    }
    
  }
}

void Connection::close(){
  //prepareGracefulDisconnect(); // TODO remove this
  Library::handle_close(m_handle);
}


}}
