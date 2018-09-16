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
  errno = 0;
  auto result = Library::handle_write(m_handle, buff, count);
  if(result <= 0) {
    auto e = errno;
    if(e == EAGAIN || e == EWOULDBLOCK){
      return oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY; // For async io. In case socket is non_blocking
    } else if(e == EINTR) {
      return oatpp::data::stream::Errors::ERROR_IO_RETRY;
    } else if(e == EPIPE) {
      return oatpp::data::stream::Errors::ERROR_IO_PIPE;
    } else {
      //OATPP_LOGD("Connection", "write errno=%d", e);
    }
  }
  return result;
}

Connection::Library::v_size Connection::read(void *buff, Library::v_size count){
  errno = 0;
  auto result = Library::handle_read(m_handle, buff, count);
  if(result <= 0) {
    auto e = errno;
    if(e == EAGAIN || e == EWOULDBLOCK){
      return oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY; // For async io. In case socket is non_blocking
    } else if(e == EINTR) {
      return oatpp::data::stream::Errors::ERROR_IO_RETRY;
    } else if(e == ECONNRESET) {
      return oatpp::data::stream::Errors::ERROR_IO_PIPE;
    } else {
      //OATPP_LOGD("Connection", "write errno=%d", e);
    }
  }
  return result;
}

void Connection::close(){
  Library::handle_close(m_handle);
}

}}
