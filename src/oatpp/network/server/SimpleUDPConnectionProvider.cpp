/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <oatpp@bamkrs.de>
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

#include "./SimpleUDPConnectionProvider.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

#include <fcntl.h>

#if defined(WIN32) || defined(_WIN32)
#include <io.h>
  #include <WinSock2.h>
  #include <WS2udpip.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <unistd.h>
#if defined(__FreeBSD__)
#include <netinet/in.h>
#endif
#endif

#define MAX_UDP_PAYLOAD_SIZE 65507

namespace oatpp { namespace network { namespace server {

SimpleUDPConnectionProvider::SimpleUDPConnectionProvider(v_uint16 port)
    : m_port(port)
    , m_closed(false)
{
  m_serverHandle = instantiateServer();
  setProperty(PROPERTY_HOST, "0.0.0.0");
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(port));
}

SimpleUDPConnectionProvider::~SimpleUDPConnectionProvider() {
  close();
}

void SimpleUDPConnectionProvider::close() {
  if(!m_closed) {
    m_closed = true;
#if defined(WIN32) || defined(_WIN32)
    ::closesocket(m_serverHandle);
#else
    ::close(m_serverHandle);
#endif
  }
}

#if defined(WIN32) || defined(_WIN32)

oatpp::v_io_handle SimpleUDPConnectionProvider::instantiateServer(){

  int iResult;

  SOCKET ListenSocket = INVALID_SOCKET;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  auto portStr = oatpp::utils::conversion::int32ToStr(m_port);

  iResult = getaddrinfo(NULL, (const char*) portStr->getData(), &hints, &result);
  if ( iResult != 0 ) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d", iResult);
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Call to socket() failed with result=%ld", WSAGetLastError());
    freeaddrinfo(result);
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Call to socket() failed.");
  }

  // Setup the TCP listening socket
  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Call to bind() failed with result=%ld", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Call to bind() failed.");
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Call to listen() failed with result=%ld", WSAGetLastError());
    closesocket(ListenSocket);
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Call to listen() failed.");
  }

  u_long flags = 1;
  if(NO_ERROR != ioctlsocket(ListenSocket, FIONBIO, &flags)) {
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Call to ioctlsocket failed.");
  }

  return ListenSocket;

}

#else

oatpp::v_io_handle SimpleUDPConnectionProvider::instantiateServer(){

  oatpp::v_io_handle serverHandle;
  v_int32 ret;
  int yes = 1;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = AI_PASSIVE;
  auto portStr = oatpp::utils::conversion::int32ToStr(m_port);

  ret = getaddrinfo(NULL, (const char *) portStr->getData(), &hints, &result);
  if (ret != 0) {
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d: %s", ret, strerror(errno));
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  serverHandle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (serverHandle < 0) {
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Couldn't open a socket: socket(%d, %d, %d) %s",
               result->ai_family, result->ai_socktype, result->ai_protocol, strerror(errno));
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Couldn't open a socket");
  }

  ret = setsockopt(serverHandle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if(ret < 0) {
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Warning. Failed to set %s for accepting socket: %s", "SO_REUSEADDR", strerror(errno));
  }


  ret = bind(serverHandle, result->ai_addr, (int) result->ai_addrlen);
  if(ret != 0) {
    ::close(serverHandle);
    OATPP_LOGE("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]", "Error. Failed to bind port %d: %s", m_port, strerror(errno));
    throw std::runtime_error("[oatpp::network::server::SimpleUDPConnectionProvider::instantiateServer()]: Error. Can't bind to address: %s");
  }

  fcntl(serverHandle, F_SETFL, O_NONBLOCK);

  return serverHandle;

}

#endif

std::shared_ptr<oatpp::data::stream::IOStream> SimpleUDPConnectionProvider::getUDPConnection() {
  auto connection = std::make_shared<BufferedUDPConnection>(m_serverHandle);
  // The BufferedUDPConnection does not populate or flush on its own to be compatible to client and server connections
  // So need to call populate it with the actual udp-message before passing it
  connection->populate();
  return connection;
}

std::shared_ptr<oatpp::data::stream::IOStream> SimpleUDPConnectionProvider::getConnection() {

  fd_set set;
  struct timeval timeout;
  FD_ZERO(&set);
  FD_SET(m_serverHandle, &set);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while(!m_closed) {

    auto res = select(m_serverHandle + 1, &set, nullptr, nullptr, &timeout);

    if (res == 0) {
      return nullptr;
    }

    if (res > 0) {
      break;
    }

  }

  return getUDPConnection();
}

void SimpleUDPConnectionProvider::invalidateConnection(const std::shared_ptr<IOStream>& connection) {

  /**
   * Do nothing. We have no real "connection" just the socket. We don't want to close that here!
   */

}

oatpp::v_io_handle SimpleUDPConnectionProvider::getHandle() {
  return m_serverHandle;
}

}}}