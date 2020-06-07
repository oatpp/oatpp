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

#include "./BufferedConnection.hpp"

#if defined(WIN32) || defined(_WIN32)
#include <io.h>
#include <WinSock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

#include <thread>
#include <chrono>
#include <fcntl.h>

namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferedConnection

BufferedConnection::BufferedConnection(v_io_handle handle, v_io_size maxBufferSize) : m_handle(handle), m_maxOutBuffSize(maxBufferSize) {

};

v_io_size BufferedConnection::write(const void *buff,
                                    v_buff_size count,
                                    async::Action &action) {
  if (m_out.size() == m_maxOutBuffSize) {
    return -ENOMEM;
  }
  if (m_out.size() + count > m_maxOutBuffSize) {
    count = m_maxOutBuffSize - m_out.size();
  }
  size_t writeTo = m_out.size();
  m_out.resize(m_out.size() + count);
  memcpy(m_out.data() + writeTo, buff, count);
  return count;
}

v_io_size BufferedConnection::read(void *buff,
                                   v_buff_size count,
                                   async::Action &action) {
  if (m_in.size() < (m_in_off + count)) {
    count = m_in.size() - m_in_off;
  }

  memcpy(buff, m_in.data() + m_in_off, count);
  m_in_off += count;
  return count;
}

}}
