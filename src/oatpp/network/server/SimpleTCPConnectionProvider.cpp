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

#include "./SimpleTCPConnectionProvider.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

#include <fcntl.h>

#if defined(WIN32) || defined(_WIN32)
  #include <io.h>
  #include <WinSock2.h>
  #include <WS2tcpip.h>
#else
  #include <netdb.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <netinet/tcp.h>
  #include <unistd.h>
  #if defined(__FreeBSD__)
    #include <netinet/in.h>
  #endif
#endif

namespace oatpp { namespace network { namespace server {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedConnection

const char* const SimpleTCPConnectionProvider::ExtendedConnection::PROPERTY_PEER_ADDRESS = "peer_address";
const char* const SimpleTCPConnectionProvider::ExtendedConnection::PROPERTY_PEER_ADDRESS_FORMAT = "peer_address_format";
const char* const SimpleTCPConnectionProvider::ExtendedConnection::PROPERTY_PEER_PORT = "peer_port";

SimpleTCPConnectionProvider::ExtendedConnection::ExtendedConnection(v_io_handle handle, data::stream::Context::Properties&& properties)
  : Connection(handle)
  , m_context(data::stream::StreamType::STREAM_INFINITE, std::forward<data::stream::Context::Properties>(properties))
{}

oatpp::data::stream::Context& SimpleTCPConnectionProvider::ExtendedConnection::getOutputStreamContext() {
  return m_context;
}

oatpp::data::stream::Context& SimpleTCPConnectionProvider::ExtendedConnection::getInputStreamContext() {
  return m_context;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SimpleTCPConnectionProvider

SimpleTCPConnectionProvider::SimpleTCPConnectionProvider(v_uint16 port, bool useExtendedConnections)
  :SimpleTCPConnectionProvider("localhost", port, useExtendedConnections)
{
}

SimpleTCPConnectionProvider::SimpleTCPConnectionProvider(const oatpp::String& host, v_uint16 port, bool useExtendedConnections)
        : m_port(port)
        , m_closed(false)
        , m_useExtendedConnections(useExtendedConnections)
{
  setProperty(PROPERTY_HOST, host);
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(port));
  m_serverHandle = instantiateServer();
}

SimpleTCPConnectionProvider::~SimpleTCPConnectionProvider() {
  close();
}

void SimpleTCPConnectionProvider::close() {
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

oatpp::v_io_handle SimpleTCPConnectionProvider::instantiateServer(){

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
    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d", iResult);
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]", "Error. Call to socket() failed with result=%ld", WSAGetLastError());
    freeaddrinfo(result);
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Call to socket() failed.");
  }

  // Setup the TCP listening socket
  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]", "Error. Call to bind() failed with result=%ld", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Call to bind() failed.");
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]", "Error. Call to listen() failed with result=%ld", WSAGetLastError());
    closesocket(ListenSocket);
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Call to listen() failed.");
  }

  u_long flags = 1;
  if(NO_ERROR != ioctlsocket(ListenSocket, FIONBIO, &flags)) {
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Call to ioctlsocket failed.");
  }

  return ListenSocket;

}

#else

oatpp::v_io_handle SimpleTCPConnectionProvider::instantiateServer(){

  oatpp::v_io_handle serverHandle;
  v_int32 ret;
  int yes = 1;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE;

  auto portStr = oatpp::utils::conversion::int32ToStr(m_port);
  auto hostStr = getProperty(PROPERTY_HOST);

  ret = getaddrinfo((const char *)hostStr.getData(), (const char *) portStr->getData(), &hints, &result);
  if (ret != 0) {
    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d: %s", ret, strerror(errno));
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  while(result != nullptr) {

    serverHandle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (serverHandle >= 0) {

      if (setsockopt(serverHandle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != 0) {
        OATPP_LOGW("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]",
                   "Warning. Failed to set %s for accepting socket: %s", "SO_REUSEADDR", strerror(errno));
      }

      if (bind(serverHandle, result->ai_addr, (int) result->ai_addrlen) == 0 &&
          listen(serverHandle, 10000) == 0)
      {
        break;
      }

      ::close(serverHandle);

    }

    result = result->ai_next;

  }

  if (result == nullptr) {
    std::string err = strerror(errno);
    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]",
               "Error. Couldn't bind. %s", err.c_str());
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: "
                             "Error. Couldn't bind " + err);
  }

  fcntl(serverHandle, F_SETFL, O_NONBLOCK);

  return serverHandle;

}

#endif

bool SimpleTCPConnectionProvider::prepareConnectionHandle(oatpp::v_io_handle handle) {

  if (handle < 0) {
    v_int32 error = errno;
    if(error == EAGAIN || error == EWOULDBLOCK){
      return false;
    } else {
      if(!m_closed) { // m_serverHandle==0 if ConnectionProvider was closed. Not an error.
        OATPP_LOGD("[oatpp::network::server::SimpleTCPConnectionProvider::prepareConnectionHandle()]", "Error. %d", error);
      }
      return false;
    }
  }

#ifdef SO_NOSIGPIPE
  int yes = 1;
  v_int32 ret = setsockopt(handle, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int));
  if(ret < 0) {
    OATPP_LOGD("[oatpp::network::server::SimpleTCPConnectionProvider::prepareConnectionHandle()]", "Warning. Failed to set %s for socket", "SO_NOSIGPIPE");
  }
#endif

  return true;

}

std::shared_ptr<oatpp::data::stream::IOStream> SimpleTCPConnectionProvider::getDefaultConnection() {

  oatpp::v_io_handle handle = accept(m_serverHandle, nullptr, nullptr);

  if(!oatpp::isValidIOHandle(handle)) {
    return nullptr;
  }

  if(prepareConnectionHandle(handle)) {
    return std::make_shared<Connection>(handle);
  }

  return nullptr;

}

std::shared_ptr<oatpp::data::stream::IOStream> SimpleTCPConnectionProvider::getExtendedConnection() {

  struct sockaddr_storage clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);

  data::stream::Context::Properties properties;

  oatpp::v_io_handle handle = accept(m_serverHandle, (struct sockaddr*) &clientAddress, &clientAddressSize);

  if(!oatpp::isValidIOHandle(handle)) {
    return nullptr;
  }

  if (clientAddress.ss_family == AF_INET) {

    char strIp[INET_ADDRSTRLEN];
    struct sockaddr_in* sockAddress = (struct sockaddr_in*) &clientAddress;
    inet_ntop(AF_INET, &sockAddress->sin_addr, strIp, INET_ADDRSTRLEN);

    properties.put_LockFree(ExtendedConnection::PROPERTY_PEER_ADDRESS, oatpp::String((const char*) strIp));
    properties.put_LockFree(ExtendedConnection::PROPERTY_PEER_ADDRESS_FORMAT, "ipv4");
    properties.put_LockFree(ExtendedConnection::PROPERTY_PEER_PORT, oatpp::utils::conversion::int32ToStr(sockAddress->sin_port));

  } else if (clientAddress.ss_family == AF_INET6) {

    char strIp[INET6_ADDRSTRLEN];
    struct sockaddr_in6* sockAddress = (struct sockaddr_in6*) &clientAddress;
    inet_ntop(AF_INET6, &sockAddress->sin6_addr, strIp, INET6_ADDRSTRLEN);

    properties.put_LockFree(ExtendedConnection::PROPERTY_PEER_ADDRESS, oatpp::String((const char*) strIp));
    properties.put_LockFree(ExtendedConnection::PROPERTY_PEER_ADDRESS_FORMAT, "ipv6");
    properties.put_LockFree(ExtendedConnection::PROPERTY_PEER_PORT, oatpp::utils::conversion::int32ToStr(sockAddress->sin6_port));

  } else {

#if defined(WIN32) || defined(_WIN32)
    ::closesocket(handle);
#else
    ::close(handle);
#endif

    OATPP_LOGE("[oatpp::network::server::SimpleTCPConnectionProvider::getExtendedConnection()]", "Error. Unknown address family.");
    return nullptr;

  }

  if(prepareConnectionHandle(handle)) {
    return std::make_shared<ExtendedConnection>(handle, std::move(properties));
  }

  return nullptr;

}

std::shared_ptr<oatpp::data::stream::IOStream> SimpleTCPConnectionProvider::getConnection() {

  fd_set set;
  struct timeval timeout;
  FD_ZERO(&set);
  FD_SET(m_serverHandle, &set);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while(!m_closed) {

    auto res = select(int(m_serverHandle + 1), &set, nullptr, nullptr, &timeout);

    if (res >= 0) {
      break;
    }

  }

  if(m_useExtendedConnections) {
    return getExtendedConnection();
  }

  return getDefaultConnection();

}

void SimpleTCPConnectionProvider::invalidateConnection(const std::shared_ptr<IOStream>& connection) {

  /************************************************
   * WARNING!!!
   *
   * shutdown(handle, SHUT_RDWR)    <--- DO!
   * close(handle);                 <--- DO NOT!
   *
   * DO NOT CLOSE file handle here -
   * USE shutdown instead.
   * Using close prevent FDs popping out of epoll,
   * and they'll be stuck there forever.
   ************************************************/

  auto c = std::static_pointer_cast<network::Connection>(connection);
  v_io_handle handle = c->getHandle();

#if defined(WIN32) || defined(_WIN32)
  shutdown(handle, SD_BOTH);
#else
  shutdown(handle, SHUT_RDWR);
#endif

}

}}}
