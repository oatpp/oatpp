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

#if defined(WIN32) || defined(_WIN32)
#include <io.h>
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#if defined(__FreeBSD__)
#include <netinet/in.h>
#endif
#endif

#include "oatpp/network/BufferedUDPConnection.hpp"

#define MAX_UDP_PAYLOAD_SIZE 65507

namespace oatpp { namespace network {

oatpp::data::stream::DefaultInitializedContext BufferedUDPConnection::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_FINITE);

BufferedUDPConnection::BufferedUDPConnection(v_io_handle handle) : BufferedConnection(handle, MAX_UDP_PAYLOAD_SIZE) {

}

BufferedUDPConnection::~BufferedUDPConnection() {
  flush();
}

v_io_size BufferedUDPConnection::populate() {

  // Allocate buffer
  m_in.resize(m_maxOutBuffSize);

  // Prepare the memory to save the senders details
  socklen_t clientAddressSize = sizeof(m_clientAddress);
  memset(&m_clientAddress, 0, clientAddressSize);

  // Read the whole message (as long as the buffer is sufficient).
  // In this case we are safe because there is a hard UDP payload limit.
  // Also we are saving the senders details so we can address it for our response
  ssize_t rc = recvfrom(m_handle, (char *) m_in.data(), m_maxOutBuffSize,
                        MSG_WAITALL, (struct sockaddr *) &m_clientAddress,
                        &clientAddressSize);

  if (rc < 0) {
    OATPP_LOGE("[oatpp::network::BufferedUDPConnection::populate()]", "Error. recv failed: %s (%d)", strerror(errno), errno);
    throw std::runtime_error("[oatpp::network::BufferedUDPConnection::populate()]: Error. recv failed.");
  }

  // Resize the buffer to the actual amount of data.
  // This is important to save RAM and correct readout later on in `read()`.
  m_in.resize(rc);

  return rc;
}

void BufferedUDPConnection::flush() {
  ssize_t rc;

  // Do we care if the call fails? Probably not, its UDP
  // so don't escalate or throw anything if sendto fails.
  if (m_clientAddress.ss_family == AF_INET) {
    struct sockaddr_in cliaddr;
    memcpy(&cliaddr, &m_clientAddress, sizeof(cliaddr));
    rc = sendto(m_handle, (const char *) m_out.data(), m_out.size(), 0,
                (struct sockaddr *) &cliaddr, sizeof(cliaddr));
  } else if(m_clientAddress.ss_family == AF_INET6) {
    struct sockaddr_in6 cliaddr;
    memcpy(&cliaddr, &m_clientAddress, sizeof(cliaddr));
    rc = sendto(m_handle, (const char *) m_out.data(), m_out.size(), 0,
                (struct sockaddr *) &cliaddr, sizeof(cliaddr));
  } else {
    OATPP_LOGE("[oatpp::network::BufferedUDPConnection::flush()]", "Error. Unknown ss_family");
    throw std::runtime_error("[oatpp::network::BufferedUDPConnection::flus()]: Error. Unknown ss_family.");
  }

  // But at least print this error message...
  if (rc < 0) {
    OATPP_LOGE("[oatpp::network::BufferedUDPConnection::flush()]","Error: %s (%d)",strerror(errno), errno);
  }

  // Clear up data and set to 0 before to not leave any sensitive things in memory
  memset(m_out.data(), 0, m_out.size());
  m_out.resize(0);
}

///////////////////////////////////////////////////////////////////////////////////
/// Default IOStream related stuff

void BufferedUDPConnection::setOutputStreamIOMode(data::stream::IOMode ioMode) {
  m_mode = ioMode;
}

data::stream::IOMode BufferedUDPConnection::getOutputStreamIOMode() {
  return m_mode;
}

data::stream::Context &BufferedUDPConnection::getOutputStreamContext() {
  return DEFAULT_CONTEXT;
}

void BufferedUDPConnection::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_mode = ioMode;
}

data::stream::IOMode BufferedUDPConnection::getInputStreamIOMode() {
  return m_mode;
}
data::stream::Context &BufferedUDPConnection::getInputStreamContext() {
  return DEFAULT_CONTEXT;
}

}}
