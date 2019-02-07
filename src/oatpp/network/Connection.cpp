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

#include <unistd.h>
#include <sys/socket.h>
#include <thread>
#include <chrono>

namespace oatpp { namespace network {

Connection::Connection(data::v_io_handle handle)
  : m_handle(handle)
{
}

Connection::~Connection(){
  close();
}

data::v_io_size Connection::write(const void *buff, data::v_io_size count){

  errno = 0;

  v_int32 flags = 0;
#ifdef MSG_NOSIGNAL
  flags |= MSG_NOSIGNAL;
#endif
  auto result = ::send(m_handle, buff, count, flags);

  if(result <= 0) {
    auto e = errno;
    if(e == EAGAIN || e == EWOULDBLOCK){
      return data::IOError::WAIT_RETRY; // For async io. In case socket is non_blocking
    } else if(e == EINTR) {
      return data::IOError::RETRY;
    } else if(e == EPIPE) {
      return data::IOError::BROKEN_PIPE;
    } else {
      //OATPP_LOGD("Connection", "write errno=%d", e);
    }
  }
  return result;
}

data::v_io_size Connection::read(void *buff, data::v_io_size count){
  errno = 0;
  auto result = ::read(m_handle, buff, count);
  if(result <= 0) {
    auto e = errno;
    if(e == EAGAIN || e == EWOULDBLOCK){
      return data::IOError::WAIT_RETRY; // For async io. In case socket is non_blocking
    } else if(e == EINTR) {
      return data::IOError::RETRY;
    } else if(e == ECONNRESET) {
      return data::IOError::BROKEN_PIPE;
    } else {
      //OATPP_LOGD("Connection", "write errno=%d", e);
    }
  }
  return result;
}

void Connection::close(){
  ::close(m_handle);
}

}}
