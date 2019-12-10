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
#endif

namespace oatpp { namespace network { namespace server {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedConnection

const char* const SimpleTCPConnectionProvider::ExtendedConnection::PROPERTY_PEER_ADDRESS = "peer_address";
const char* const SimpleTCPConnectionProvider::ExtendedConnection::PROPERTY_PEER_ADDRESS_FORMAT = "peer_address_format";
const char* const SimpleTCPConnectionProvider::ExtendedConnection::PROPERTY_PEER_PORT = "peer_port";

SimpleTCPConnectionProvider::ExtendedConnection::ExtendedConnection(data::v_io_handle handle)
  : Connection(handle)
{}

oatpp::data::stream::Context* SimpleTCPConnectionProvider::ExtendedConnection::getOutputStreamContext() {
  return &m_context;
}

oatpp::data::stream::Context* SimpleTCPConnectionProvider::ExtendedConnection::getInputStreamContext() {
  return &m_context;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SimpleTCPConnectionProvider

SimpleTCPConnectionProvider::SimpleTCPConnectionProvider(v_word16 port, bool useExtendedConnections)
  : m_port(port)
  , m_closed(false)
  , m_useExtendedConnections(useExtendedConnections)
{
  m_serverHandle = instantiateServer();
  setProperty(PROPERTY_HOST, "localhost");
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(port));
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

oatpp::data::v_io_handle SimpleTCPConnectionProvider::instantiateServer(){

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

  return ListenSocket;

}

#else

oatpp::data::v_io_handle SimpleTCPConnectionProvider::instantiateServer(){

  oatpp::data::v_io_handle serverHandle;
  v_int32 ret;
  int yes = 1;

  struct sockaddr_in6 addr;

  addr.sin6_family = AF_INET6;
  addr.sin6_port = htons(m_port);
  addr.sin6_addr = in6addr_any;

  serverHandle = socket(AF_INET6, SOCK_STREAM, 0);

  if(serverHandle < 0){
    return -1;
  }

  ret = setsockopt(serverHandle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if(ret < 0) {
    OATPP_LOGD("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]", "Warning. Failed to set %s for accepting socket", "SO_REUSEADDR");
  }

  ret = bind(serverHandle, (struct sockaddr *)&addr, sizeof(addr));

  if(ret != 0) {
    ::close(serverHandle);
    throw std::runtime_error("[oatpp::network::server::SimpleTCPConnectionProvider::instantiateServer()]: Error. Can't bind to address.");
  }

  ret = listen(serverHandle, 10000);
  if(ret < 0) {
    ::close(serverHandle);
    return -1 ;
  }

  fcntl(serverHandle, F_SETFL, 0);//O_NONBLOCK);

  return serverHandle;

}

#endif

bool SimpleTCPConnectionProvider::prepareConnectionHandle(oatpp::data::v_io_handle handle) {

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

  oatpp::data::v_io_handle handle = accept(m_serverHandle, nullptr, nullptr);

  if(prepareConnectionHandle(handle)) {
    return Connection::createShared(handle);
  }

  return nullptr;

}

std::shared_ptr<oatpp::data::stream::IOStream> SimpleTCPConnectionProvider::getExtendedConnection() {

  socklen_t len;
  int port;
  int ipStrSize = INET6_ADDRSTRLEN + 1;
  auto ipstr = std::unique_ptr<char[]>(new char[ipStrSize]);
  const char* addrFormat;
  struct sockaddr_storage addr;

  len = sizeof(addr);

  oatpp::data::v_io_handle handle = accept(m_serverHandle, (struct sockaddr*)&addr, &len);

  if (addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr.get(), ipStrSize);
    addrFormat = "ipv4";
  } else {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr.get(), ipStrSize);
    addrFormat = "ipv6";
  }

  if(prepareConnectionHandle(handle)) {

    auto connection = ExtendedConnection::createShared(handle);
    auto& properties = connection->getInputStreamContext()->getMutableProperties();

    properties.put(ExtendedConnection::PROPERTY_PEER_ADDRESS, oatpp::String((const char*) ipstr.get()));
    properties.put(ExtendedConnection::PROPERTY_PEER_ADDRESS_FORMAT, addrFormat);
    properties.put(ExtendedConnection::PROPERTY_PEER_PORT, oatpp::utils::conversion::int32ToStr(port));

    return connection;

  }

  return nullptr;

}

std::shared_ptr<oatpp::data::stream::IOStream> SimpleTCPConnectionProvider::getConnection(){
  if(m_useExtendedConnections) {
    return getExtendedConnection();
  }
  return getDefaultConnection();
}

}}}
