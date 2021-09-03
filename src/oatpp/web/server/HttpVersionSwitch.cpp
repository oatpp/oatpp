/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include "HttpVersionSwitch.hpp"

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"

namespace oatpp { namespace data { namespace stream {

class PeekableIOStream : public data::stream::IOStream, public data::stream::BufferedInputStream {
 private:
  std::shared_ptr<data::stream::IOStream> m_io;
  std::shared_ptr<data::stream::InputStreamBufferedProxy> m_iproxy;

 public:
  PeekableIOStream(const std::shared_ptr<data::stream::IOStream> &io)
  : m_io(io)
  , m_iproxy(data::stream::InputStreamBufferedProxy::createShared(m_io, std::make_shared<std::string>(data::buffer::IOBuffer::BUFFER_SIZE, 0))) {}

  v_io_size write(const void *data, v_buff_size count, async::Action &action) override {
    return m_io->write(data, count, action);
  }
  void setOutputStreamIOMode(data::stream::IOMode ioMode) override {
    m_io->setOutputStreamIOMode(ioMode);
  }
  data::stream::IOMode getOutputStreamIOMode() override {
    return m_io->getOutputStreamIOMode();
  }
  data::stream::Context &getOutputStreamContext() override {
    return m_io->getOutputStreamContext();
  }
  v_io_size read(void *buffer, v_buff_size count, async::Action &action) override {
    return m_iproxy->read(buffer, count, action);
  }
  void setInputStreamIOMode(data::stream::IOMode ioMode) override {
    m_iproxy->setInputStreamIOMode(ioMode);
  }
  data::stream::IOMode getInputStreamIOMode() override {
    return m_iproxy->getInputStreamIOMode();
  }
  data::stream::Context &getInputStreamContext() override {
    return m_iproxy->getInputStreamContext();
  }
  v_io_size peek(void *data, v_buff_size count, async::Action &action) override {
    return m_iproxy->peek(data, count, action);
  }
  v_io_size availableToRead() const override {
    return m_iproxy->availableToRead();
  }
  v_io_size commitReadOffset(v_buff_size count) override {
    return m_iproxy->commitReadOffset(count);
  }
};

}}}

namespace oatpp { namespace web { namespace server {

void HttpVersionSwitch::handleConnection(const std::shared_ptr<IOStream> &connection,
                                         const std::shared_ptr<const ParameterMap> &params) {

  // Only do the checking if both handlers are given. If only one is given, just pass it to the given handler.
  if (m_http1 != nullptr && m_http2 != nullptr) {
    // Disclaimer: In general we should avoid using the "VersionSwitch". It requires us to have to peek into the stream.
    // For this, we create an "InputStreamBufferedProxy". This proxy itself isn't a problem. However, it introduces
    // some overhead and unnecessarily copies data back and forth. However, it could be totally different and actually
    // improves the performance since less ::read calls are needed on the kernel (see InputStreamBufferedProxy implementation).
    auto pio = std::make_shared<data::stream::PeekableIOStream>(connection);
    v_uint8 buf[24];
    v_buff_size available = 0;
    async::Action action;

    // 0x505249202a20485454502f322e300d0a0d0a534d0d0a0d0a
    // PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n
    // |    12    |
    // Step 1: Peek the first 12 bytes. Each valid HTTP request will at least have 12 bytes
    // We only want to peek the data, not read. Both underlying connection-handler do not know we consume the data here.
    while (available < 12) {
      auto ret = pio->peek(buf, 12 - available, action);
      if (ret < 0) {
        if (ret == IOError::RETRY_READ || ret == IOError::RETRY_WRITE) {
          continue;
        } else {
          throw std::runtime_error("[oatpp::web::server::HttpVersionSwitch::handleConnection()] Error: Unable to read first 12 bytes from connection.");
        }
      }
      available += ret;
    }
    // Now compare the 12 bytes with the HTTP2 preflight message.
    p_uint32 pre = (p_uint32)web::protocol::http2::HTTP2_PRI_MESSAGE;
    p_uint32 inbuf = (p_uint32)buf;
    if ((*pre++ == *inbuf++) && (*pre++ == *inbuf++) && (*pre++ == *inbuf++)) {
      // We have a match, we can safely assume, that this is a valid HTTP2 handshake
      // So read the entire preflight message from the stream and validate the rest
      pio->peek(buf, 24, action);
      if ((*pre++ == *inbuf++) && (*pre++ == *inbuf++) && (*pre == *inbuf)) {
        // This is a valid HTTP2 request. We can now delegate the request to http2.
        m_http2->handleConnection(pio, params);
        return;
      }
      throw std::runtime_error("[oatpp::web::server::HttpVersionSwitch::handleConnection()] Error: Received invalid HTTP/2 preflight message.");
    }
    // Its no HTTP2 preflight message, delegate to http1 handler
    m_http1->handleConnection(pio, params);
    return;
  } else if (m_http1 != nullptr) {
    m_http1->handleConnection(connection, params);
    return;
  } else if (m_http2 != nullptr) {
    m_http2->handleConnection(connection, params);
    return;
  }
  throw std::runtime_error("[oatpp::web::server::HttpVersionSwitch::handleConnection()] Error: No handler to delegate request to.");
}

void HttpVersionSwitch::stop() {
  if (m_http1) {
    m_http1->stop();
  }
  if (m_http2) {
    m_http2->stop();
  }
}

}}}
