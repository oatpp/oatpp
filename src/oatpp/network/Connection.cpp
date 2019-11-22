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

#include "./Connection.hpp"

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

Connection::Connection(data::v_io_handle handle)
  : m_handle(handle)
{
#if defined(WIN32) || defined(_WIN32)
    // in Windows, there is no reliable method to get if a socket is blocking or not.
    // Eevery socket is created blocking in Windows so we assume this state and pray.
    m_mode = data::stream::BLOCKING;
#endif
}

Connection::~Connection(){
  close();
}

data::v_io_size Connection::write(const void *buff, v_buff_size count){

#if defined(WIN32) || defined(_WIN32)

  auto result = ::send(m_handle, (const char*) buff, (int)count, 0);

  if(result == SOCKET_ERROR) {

    auto e = WSAGetLastError();

    if(e == WSAEWOULDBLOCK){
      return data::IOError::WAIT_RETRY; // For async io. In case socket is non_blocking
    } else if(e == WSAEINTR) {
      return data::IOError::RETRY;
    } else if(e == WSAECONNRESET) {
      return data::IOError::BROKEN_PIPE;
    } else {
      //OATPP_LOGD("Connection", "write errno=%d", e);
    }
  }
  return result;

#else

  errno = 0;
  v_int32 flags = 0;

#ifdef MSG_NOSIGNAL
  flags |= MSG_NOSIGNAL;
#endif

  auto result = ::send(m_handle, buff, (size_t)count, flags);

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

#endif

}

data::v_io_size Connection::read(void *buff, v_buff_size count){

#if defined(WIN32) || defined(_WIN32)

  auto result = ::recv(m_handle, (char*)buff, (int)count, 0);

  if(result == SOCKET_ERROR) {

    auto e = WSAGetLastError();

    if(e == WSAEWOULDBLOCK){
      return data::IOError::WAIT_RETRY; // For async io. In case socket is non_blocking
    } else if(e == WSAEINTR) {
      return data::IOError::RETRY;
    } else if(e == WSAECONNRESET) {
      return data::IOError::BROKEN_PIPE;
    } else {
      //OATPP_LOGD("Connection", "write errno=%d", e);
    }
  }
  return result;


#else

  errno = 0;

  auto result = ::read(m_handle, buff, (size_t)count);

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

#endif

}

#if defined(WIN32) || defined(_WIN32)
void Connection::setStreamIOMode(oatpp::data::stream::IOMode ioMode) {

    u_long flags;

    switch(ioMode) {
        case data::stream::BLOCKING:
            flags = 0;
            if(NO_ERROR != ioctlsocket(m_handle, FIONBIO, &flags)) {
                throw std::runtime_error("[oatpp::network::Connection::setStreamIOMode()]: Error. Can't set stream I/O mode to IOMode::BLOCKING.");
            }
            m_mode = data::stream::BLOCKING;
            break;
        case data::stream::NON_BLOCKING:
            flags = 1;
            if(NO_ERROR != ioctlsocket(m_handle, FIONBIO, &flags)) {
                throw std::runtime_error("[oatpp::network::Connection::setStreamIOMode()]: Error. Can't set stream I/O mode to IOMode::NON_BLOCKING.");
            }
            m_mode = data::stream::NON_BLOCKING;
            break;
    }
}
#else
void Connection::setStreamIOMode(oatpp::data::stream::IOMode ioMode) {

  auto flags = fcntl(m_handle, F_GETFL);
  if (flags < 0) {
    throw std::runtime_error("[oatpp::network::Connection::setStreamIOMode()]: Error. Can't get socket flags.");
  }

  switch(ioMode) {

    case oatpp::data::stream::IOMode::BLOCKING:
      flags = flags & (~O_NONBLOCK);
      if (fcntl(m_handle, F_SETFL, flags) < 0) {
        throw std::runtime_error("[oatpp::network::Connection::setStreamIOMode()]: Error. Can't set stream I/O mode to IOMode::BLOCKING.");
      }
      break;

    case oatpp::data::stream::IOMode::NON_BLOCKING:
      flags = (flags | O_NONBLOCK);
      if (fcntl(m_handle, F_SETFL, flags) < 0) {
        throw std::runtime_error("[oatpp::network::Connection::setStreamIOMode()]: Error. Can't set stream I/O mode to IOMode::NON_BLOCKING.");
      }
      break;

  }
}
#endif

#if defined(WIN32) || defined(_WIN32)
oatpp::data::stream::IOMode Connection::getStreamIOMode() {
    return m_mode;
}
#else
oatpp::data::stream::IOMode Connection::getStreamIOMode() {

  auto flags = fcntl(m_handle, F_GETFL);
  if (flags < 0) {
    throw std::runtime_error("[oatpp::network::Connection::getStreamIOMode()]: Error. Can't get socket flags.");
  }

  if((flags & O_NONBLOCK) > 0) {
    return oatpp::data::stream::IOMode::NON_BLOCKING;
  }

  return oatpp::data::stream::IOMode::BLOCKING;

}
#endif

oatpp::async::Action Connection::suggestOutputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createIORepeatAction(m_handle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
  }

  switch (ioResult) {
    case oatpp::data::IOError::WAIT_RETRY:
      return oatpp::async::Action::createIOWaitAction(m_handle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
    case oatpp::data::IOError::RETRY:
      return oatpp::async::Action::createIORepeatAction(m_handle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
  }

  throw std::runtime_error("[oatpp::network::virtual_::Pipe::Reader::suggestInputStreamAction()]: Error. Unable to suggest async action for I/O result.");

}

oatpp::async::Action Connection::suggestInputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createIORepeatAction(m_handle, oatpp::async::Action::IOEventType::IO_EVENT_READ);
  }

  switch (ioResult) {
    case oatpp::data::IOError::WAIT_RETRY:
      return oatpp::async::Action::createIOWaitAction(m_handle, oatpp::async::Action::IOEventType::IO_EVENT_READ);
    case oatpp::data::IOError::RETRY:
      return oatpp::async::Action::createIORepeatAction(m_handle, oatpp::async::Action::IOEventType::IO_EVENT_READ);
  }

  throw std::runtime_error("[oatpp::network::virtual_::Pipe::Reader::suggestInputStreamAction()]: Error. Unable to suggest async action for I/O result.");


}

void Connection::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  setStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode Connection::getOutputStreamIOMode() {
  return getStreamIOMode();
}

void Connection::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  setStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode Connection::getInputStreamIOMode() {
  return getStreamIOMode();
}

void Connection::close(){
#if defined(WIN32) || defined(_WIN32)
	::closesocket(m_handle);
#else
	::close(m_handle);
#endif
}

}}
