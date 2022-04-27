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

#include "./ConnectionProvider.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

#include <fcntl.h>

#if defined(WIN32) || defined(_WIN32)
  #include <io.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>
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


// Workaround for MinGW from: https://www.mail-archive.com/users@ipv6.org/msg02107.html
#if defined(__MINGW32__) && _WIN32_WINNT < 0x0600
  const char * inet_ntop (int af, const void *src, char *dst, socklen_t cnt) {
    if (af == AF_INET) {
      struct sockaddr_in in;

      memset (&in, 0, sizeof(in));
      in.sin_family = AF_INET;
      memcpy (&in.sin_addr, src, sizeof(struct in_addr));
      getnameinfo ((struct sockaddr *)&in, sizeof (struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
      return dst;
    } else if (af == AF_INET6) {
      struct sockaddr_in6 in;
      memset (&in, 0, sizeof(in));
      in.sin6_family = AF_INET6;
      memcpy (&in.sin6_addr, src, sizeof(struct in_addr6));
      getnameinfo ((struct sockaddr *)&in, sizeof (struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
      return dst;
    }

    return NULL;
  }
#endif

namespace oatpp { namespace network { namespace tcp { namespace server {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedConnection

const char* const ConnectionProvider::ExtendedConnection::PROPERTY_PEER_ADDRESS = "peer_address";
const char* const ConnectionProvider::ExtendedConnection::PROPERTY_PEER_ADDRESS_FORMAT = "peer_address_format";
const char* const ConnectionProvider::ExtendedConnection::PROPERTY_PEER_PORT = "peer_port";

ConnectionProvider::ExtendedConnection::ExtendedConnection(v_io_handle handle, data::stream::Context::Properties&& properties)
  : Connection(handle)
  , m_context(data::stream::StreamType::STREAM_INFINITE, std::forward<data::stream::Context::Properties>(properties))
{}

oatpp::data::stream::Context& ConnectionProvider::ExtendedConnection::getOutputStreamContext() {
  return m_context;
}

oatpp::data::stream::Context& ConnectionProvider::ExtendedConnection::getInputStreamContext() {
  return m_context;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionProvider::ConnectionInvalidator

void ConnectionProvider::ConnectionInvalidator::invalidate(const std::shared_ptr<data::stream::IOStream>& connection) {

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

  auto c = std::static_pointer_cast<network::tcp::Connection>(connection);
  v_io_handle handle = c->getHandle();

#if defined(WIN32) || defined(_WIN32)
  shutdown(handle, SD_BOTH);
#else
  shutdown(handle, SHUT_RDWR);
#endif


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionProvider

ConnectionProvider::ConnectionProvider(const network::Address& address, bool useExtendedConnections)
        : m_invalidator(std::make_shared<ConnectionInvalidator>())
        , m_address(address)
        , m_closed(false)
        , m_useExtendedConnections(useExtendedConnections)
{
  setProperty(PROPERTY_HOST, m_address.host);
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(m_address.port));
  m_serverHandle = instantiateServer();
}

ConnectionProvider::~ConnectionProvider() {
  stop();
}

void ConnectionProvider::stop() {
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

oatpp::v_io_handle ConnectionProvider::instantiateServer(){

  SOCKET serverHandle = INVALID_SOCKET;

  struct addrinfo *result = nullptr;
  struct addrinfo hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE;

  switch(m_address.family) {
    case Address::IP_4: hints.ai_family = AF_INET; break;
    case Address::IP_6: hints.ai_family = AF_INET6; break;
    default:
      hints.ai_family = AF_UNSPEC;
  }

  auto portStr = oatpp::utils::conversion::int32ToStr(m_address.port);

  const int iResult = getaddrinfo(m_address.host->c_str(), portStr->c_str(), &hints, &result);
  if (iResult != 0) {
    OATPP_LOGE("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d", iResult);
    throw std::runtime_error("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  struct addrinfo* currResult = result;
  while(currResult != nullptr) {

    serverHandle = socket(currResult->ai_family, currResult->ai_socktype, currResult->ai_protocol);

    if (serverHandle != INVALID_SOCKET) {

      int no = 0;

      if (hints.ai_family == AF_UNSPEC || hints.ai_family == Address::IP_6) {
        if (setsockopt(serverHandle, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&no, sizeof( int ) ) != 0 ) {
          OATPP_LOGW("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]",
                     "Warning. Failed to set %s for accepting socket: %s", "IPV6_V6ONLY",
                     strerror(errno));
        }
      }

      if (bind(serverHandle, currResult->ai_addr, (int) currResult->ai_addrlen) != SOCKET_ERROR &&
          listen(serverHandle, SOMAXCONN) != SOCKET_ERROR)
      {
        break;
      }

      closesocket(serverHandle);

    }

    currResult = currResult->ai_next;

  }

  freeaddrinfo(result);

  if (currResult == nullptr) {
    OATPP_LOGE("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]",
               "Error. Couldn't bind. WSAGetLastError=%ld", WSAGetLastError());
    throw std::runtime_error("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]: "
                             "Error. Couldn't bind ");
  }

  u_long flags = 1;
  if(NO_ERROR != ioctlsocket(serverHandle, FIONBIO, &flags)) {
    throw std::runtime_error("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]: Error. Call to ioctlsocket failed.");
  }

  // Update port after binding (typicaly in case of port = 0)
  struct ::sockaddr_in s_in;
  ::memset(&s_in, 0, sizeof(s_in));
  ::socklen_t s_in_len = sizeof(s_in);
  ::getsockname(serverHandle, (struct sockaddr *)&s_in, &s_in_len);
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(ntohs(s_in.sin_port)));

  return serverHandle;

}

#else

oatpp::v_io_handle ConnectionProvider::instantiateServer(){

  oatpp::v_io_handle serverHandle;
  v_int32 ret;
  int yes = 1;

  struct addrinfo *result = NULL;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE;

  switch(m_address.family) {
    case Address::IP_4: hints.ai_family = AF_INET; break;
    case Address::IP_6: hints.ai_family = AF_INET6; break;
    default:
      hints.ai_family = AF_UNSPEC;
  }

  auto portStr = oatpp::utils::conversion::int32ToStr(m_address.port);

  ret = getaddrinfo(m_address.host->c_str(), portStr->c_str(), &hints, &result);
  if (ret != 0) {
    OATPP_LOGE("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]", "Error. Call to getaddrinfo() failed with result=%d: %s", ret, strerror(errno));
    throw std::runtime_error("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]: Error. Call to getaddrinfo() failed.");
  }

  struct addrinfo* currResult = result;
  while(currResult != nullptr) {

    serverHandle = socket(currResult->ai_family, currResult->ai_socktype, currResult->ai_protocol);

    if (serverHandle >= 0) {

      if (setsockopt(serverHandle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != 0) {
        OATPP_LOGW("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]",
                   "Warning. Failed to set %s for accepting socket: %s", "SO_REUSEADDR", strerror(errno));
      }

      if (bind(serverHandle, currResult->ai_addr, (int) currResult->ai_addrlen) == 0 &&
          listen(serverHandle, 10000) == 0)
      {
        break;
      }

      ::close(serverHandle);

    }

    currResult = currResult->ai_next;

  }

  freeaddrinfo(result);

  if (currResult == nullptr) {
    std::string err = strerror(errno);
    OATPP_LOGE("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]",
               "Error. Couldn't bind. %s", err.c_str());
    throw std::runtime_error("[oatpp::network::tcp::server::ConnectionProvider::instantiateServer()]: "
                             "Error. Couldn't bind " + err);
  }

  fcntl(serverHandle, F_SETFL, O_NONBLOCK);

  // Update port after binding (typicaly in case of port = 0)
  struct ::sockaddr_in s_in;
  ::memset(&s_in, 0, sizeof(s_in));
  ::socklen_t s_in_len = sizeof(s_in);
  ::getsockname(serverHandle, (struct sockaddr *)&s_in, &s_in_len);
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(ntohs(s_in.sin_port)));

  return serverHandle;

}

#endif

bool ConnectionProvider::prepareConnectionHandle(oatpp::v_io_handle handle) {

  if (handle < 0) {
    v_int32 error = errno;
    if(error == EAGAIN || error == EWOULDBLOCK){
      return false;
    } else {
      if(!m_closed) { // m_serverHandle==0 if ConnectionProvider was closed. Not an error.
        OATPP_LOGD("[oatpp::network::tcp::server::ConnectionProvider::prepareConnectionHandle()]", "Error. %d", error);
      }
      return false;
    }
  }

#ifdef SO_NOSIGPIPE
  int yes = 1;
  v_int32 ret = setsockopt(handle, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int));
  if(ret < 0) {
    OATPP_LOGD("[oatpp::network::tcp::server::ConnectionProvider::prepareConnectionHandle()]", "Warning. Failed to set %s for socket", "SO_NOSIGPIPE");
  }
#endif

  return true;

}

provider::ResourceHandle<data::stream::IOStream> ConnectionProvider::getDefaultConnection() {

  oatpp::v_io_handle handle = accept(m_serverHandle, nullptr, nullptr);

  if(!oatpp::isValidIOHandle(handle)) {
    return nullptr;
  }

  if(prepareConnectionHandle(handle)) {
    return provider::ResourceHandle<data::stream::IOStream>(
      std::make_shared<Connection>(handle),
        m_invalidator
    );
  }

  return nullptr;

}

provider::ResourceHandle<data::stream::IOStream> ConnectionProvider::getExtendedConnection() {

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

    OATPP_LOGE("[oatpp::network::tcp::server::ConnectionProvider::getExtendedConnection()]", "Error. Unknown address family.");
    return nullptr;

  }

  if(prepareConnectionHandle(handle)) {
    return provider::ResourceHandle<data::stream::IOStream>(
      std::make_shared<ExtendedConnection>(handle, std::move(properties)),
      m_invalidator
    );
  }

  return nullptr;

}

provider::ResourceHandle<oatpp::data::stream::IOStream> ConnectionProvider::get() {

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

}}}}
