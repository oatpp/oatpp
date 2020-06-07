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

#ifndef oatpp_network_BufferedUDPConnection_hpp
#define oatpp_network_BufferedUDPConnection_hpp

#include <sys/socket.h>

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/network/BufferedConnection.hpp"

namespace oatpp { namespace network {

/**
 * Buffered UDP Connection implementation. Extends &id:oatpp::network::BufferedConnection;.
 */
class BufferedUDPConnection : public oatpp::network::BufferedConnection {
 private:
  struct sockaddr_storage m_clientAddress;
  static oatpp::data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
  data::stream::IOMode m_mode;

 public:

  /**
   * Constructor.
   */
  explicit BufferedUDPConnection(v_io_handle handle);

  /**
   * Flushing the data just before this connection is dropped (i.E. all data written).
   */
  ~BufferedUDPConnection() override;

  /**
   * Clears the output buffer and writes it to the Network-Stream
   */
  virtual void flush() override;

  /**
 * Reads all data from the message and puts it into `m_in`.
 * @return read bytes
 */
  v_io_size populate() override;

  /**
   * Set OutputStream I/O mode.
   * @param ioMode
   */
  void setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;

  /**
   * Set OutputStream I/O mode.
   * @return
   */
  oatpp::data::stream::IOMode getOutputStreamIOMode() override;

  /**
   * Get output stream context.
   * @return - &id:oatpp::data::stream::Context;.
   */
  oatpp::data::stream::Context& getOutputStreamContext() override;

  /**
   * Set InputStream I/O mode.
   * @param ioMode
   */
  void setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;

  /**
   * Get InputStream I/O mode.
   * @return
   */
  oatpp::data::stream::IOMode getInputStreamIOMode() override;

  /**
   * Get input stream context. <br>
   * @return - &id:oatpp::data::stream::Context;.
   */
  oatpp::data::stream::Context& getInputStreamContext() override;

};

}}

#endif /* oatpp_network_BufferedUDPConnection_hpp */
