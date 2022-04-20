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

#include "oatpp/network/tcp/Connection.hpp"
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
  #include <unistd.h>
#endif

namespace oatpp { namespace network { namespace tcp { namespace client {

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

ConnectionProvider::ConnectionProvider(const network::Address& address)
  : m_invalidator(std::make_shared<ConnectionInvalidator>())
  , m_address(address)
{
  setProperty(PROPERTY_HOST, address.host);
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(address.port));
}

provider::ResourceHandle<data::stream::IOStream> ConnectionProvider::get() {

  auto portStr = oatpp::utils::conversion::int32ToStr(m_address.port);

  struct addrinfo hints;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  switch(m_address.family) {
    case Address::IP_4: hints.ai_family = AF_INET; break;
    case Address::IP_6: hints.ai_family = AF_INET6; break;
    default:
      hints.ai_family = AF_UNSPEC;
  }

  struct addrinfo* result;
  auto res = getaddrinfo(m_address.host->c_str(), portStr->c_str(), &hints, &result);

  if (res != 0) {
#if defined(WIN32) || defined(_WIN32)
    throw std::runtime_error("[oatpp::network::tcp::client::ConnectionProvider::getConnection()]. "
                             "Error. Call to getaddrinfo() failed with code " + std::to_string(res));
#else
    std::string errorString = "[oatpp::network::tcp::client::ConnectionProvider::getConnection()]. Error. Call to getaddrinfo() failed: ";
	  throw std::runtime_error(errorString.append(gai_strerror(res)));
#endif
  }

  if (result == nullptr) {
    throw std::runtime_error("[oatpp::network::tcp::client::ConnectionProvider::getConnection()]. Error. Call to getaddrinfo() returned no results.");
  }

  struct addrinfo* currResult = result;
  oatpp::v_io_handle clientHandle = INVALID_IO_HANDLE;

  while(currResult != nullptr) {

    clientHandle = socket(currResult->ai_family, currResult->ai_socktype, currResult->ai_protocol);

    if(clientHandle >= 0) {

      if(connect(clientHandle, currResult->ai_addr, (int)currResult->ai_addrlen) == 0) {
        break;
      } else {
#if defined(WIN32) || defined(_WIN32)
		    ::closesocket(clientHandle);
#else
        ::close(clientHandle);
#endif
      }

    }

    currResult = currResult->ai_next;

  }

  freeaddrinfo(result);

  if(currResult == nullptr) {
    throw std::runtime_error("[oatpp::network::tcp::client::ConnectionProvider::getConnection()]: Error. Can't connect.");
  }

#ifdef SO_NOSIGPIPE
  int yes = 1;
  v_int32 ret = setsockopt(clientHandle, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int));
  if(ret < 0) {
    OATPP_LOGD("[oatpp::network::tcp::client::ConnectionProvider::getConnection()]", "Warning. Failed to set %s for socket", "SO_NOSIGPIPE");
  }
#endif

  return provider::ResourceHandle<data::stream::IOStream>(
      std::make_shared<oatpp::network::tcp::Connection>(clientHandle),
      m_invalidator
  );

}

oatpp::async::CoroutineStarterForResult<const provider::ResourceHandle<data::stream::IOStream>&> ConnectionProvider::getAsync() {

  class ConnectCoroutine : public oatpp::async::CoroutineWithResult<ConnectCoroutine, const provider::ResourceHandle<oatpp::data::stream::IOStream>&> {
  private:
    std::shared_ptr<ConnectionInvalidator> m_connectionInvalidator;
    network::Address m_address;
    oatpp::v_io_handle m_clientHandle;
  private:
    struct addrinfo* m_result;
    struct addrinfo* m_currentResult;
    bool m_isHandleOpened;
  public:

    ConnectCoroutine(const std::shared_ptr<ConnectionInvalidator>& connectionInvalidator,
                     const network::Address& address)
      : m_connectionInvalidator(connectionInvalidator)
      , m_address(address)
      , m_result(nullptr)
      , m_currentResult(nullptr)
      , m_isHandleOpened(false)
    {}

    ~ConnectCoroutine() {
      if(m_result != nullptr) {
        freeaddrinfo(m_result);
      }
    }

    Action act() override {

      auto portStr = oatpp::utils::conversion::int32ToStr(m_address.port);

      struct addrinfo hints;

      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = 0;
      hints.ai_protocol = 0;

      switch(m_address.family) {
        case Address::IP_4: hints.ai_family = AF_INET; break;
        case Address::IP_6: hints.ai_family = AF_INET6; break;
        default:
          hints.ai_family = AF_UNSPEC;
      }

      // TODO make call to get addrinfo non-blocking !!!
      auto res = getaddrinfo(m_address.host->c_str(), portStr->c_str(), &hints, &m_result);
      if (res != 0) {
        return error<async::Error>(
          "[oatpp::network::tcp::client::ConnectionProvider::getConnectionAsync()]. Error. Call to getaddrinfo() failed.");
      }

      m_currentResult = m_result;

      if (m_result == nullptr) {
        return error<async::Error>(
          "[oatpp::network::tcp::client::ConnectionProvider::getConnectionAsync()]. Error. Call to getaddrinfo() returned no results.");
      }

      return yieldTo(&ConnectCoroutine::iterateAddrInfoResults);

    }

    Action iterateAddrInfoResults() {

      /*
       * Close previously opened socket here.
       * Don't ever close socket in the method which returns action ioWait or ioRepeat
       */
      if(m_isHandleOpened) {
        m_isHandleOpened = false;
#if defined(WIN32) || defined(_WIN32)
        ::closesocket(m_clientHandle);
#else
        ::close(m_clientHandle);
#endif

      }

      if(m_currentResult != nullptr) {

        m_clientHandle = socket(m_currentResult->ai_family, m_currentResult->ai_socktype, m_currentResult->ai_protocol);

#if defined(WIN32) || defined(_WIN32)
        if (m_clientHandle == INVALID_SOCKET) {
          m_currentResult = m_currentResult->ai_next;
          return repeat();
        }
        u_long flags = 1;
        ioctlsocket(m_clientHandle, FIONBIO, &flags);
#else
        if (m_clientHandle < 0) {
          m_currentResult = m_currentResult->ai_next;
          return repeat();
        }
        fcntl(m_clientHandle, F_SETFL, O_NONBLOCK);
#endif

#ifdef SO_NOSIGPIPE
        int yes = 1;
        v_int32 ret = setsockopt(m_clientHandle, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int));
        if(ret < 0) {
          OATPP_LOGD("[oatpp::network::tcp::client::ConnectionProvider::getConnectionAsync()]", "Warning. Failed to set %s for socket", "SO_NOSIGPIPE");
        }
#endif

        m_isHandleOpened = true;
        return yieldTo(&ConnectCoroutine::doConnect);

      }

      return error<Error>("[oatpp::network::tcp::client::ConnectionProvider::getConnectionAsync()]: Error. Can't connect.");

    }

    Action doConnect() {
      errno = 0;

      auto res = connect(m_clientHandle, m_currentResult->ai_addr, (int)m_currentResult->ai_addrlen);

#if defined(WIN32) || defined(_WIN32)

      auto error = WSAGetLastError();

      if(res == 0 || error == WSAEISCONN) {
        return _return(provider::ResourceHandle<data::stream::IOStream>(
                std::make_shared<oatpp::network::tcp::Connection>(m_clientHandle),
                m_connectionInvalidator
            ));
      }
      if(error == WSAEWOULDBLOCK || error == WSAEINPROGRESS) {
        return ioWait(m_clientHandle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
      } else if(error == WSAEINTR || error == WSAEALREADY) {
        return ioRepeat(m_clientHandle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
      } else if(error == WSAEINVAL) {
         return AbstractCoroutine::error(new async::Error(
                  "[oatpp::network::tcp::client::ConnectionProvider::doConnect()]: Error. The parameter m_clientHandle is a listening socket."));
      }

#else

      if(res == 0 || errno == EISCONN) {
        return _return(provider::ResourceHandle<data::stream::IOStream>(
                std::make_shared<oatpp::network::tcp::Connection>(m_clientHandle),
                m_connectionInvalidator
            ));
      }
      if(errno == EALREADY || errno == EINPROGRESS) {
        return ioWait(m_clientHandle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
      } else if(errno == EINTR) {
        return ioRepeat(m_clientHandle, oatpp::async::Action::IOEventType::IO_EVENT_WRITE);
      }

#endif

      m_currentResult = m_currentResult->ai_next;
      return yieldTo(&ConnectCoroutine::iterateAddrInfoResults);

    }

  };

  return ConnectCoroutine::startForResult(m_invalidator, m_address);

}

}}}}
