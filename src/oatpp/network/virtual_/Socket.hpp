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

#ifndef oatpp_network_virtual__Socket_hpp
#define oatpp_network_virtual__Socket_hpp

#include "./Pipe.hpp"

namespace oatpp { namespace network { namespace virtual_ {

/**
 * Virtual socket implementation. Can be used as a bidirectional data transfer between different threads of the same process. <br>
 * Under the hood it uses a pair of &id:oatpp::network::virtual_::Pipe;. One to write data to, and one to read data from.
 * Extends &id:oatpp::base::Countable; and &id:oatpp::data::stream::IOStream;.
 */
class Socket : public oatpp::data::stream::IOStream, public oatpp::base::Countable {
private:
  std::shared_ptr<Pipe> m_pipeIn;
  std::shared_ptr<Pipe> m_pipeOut;
public:
  /**
   * Constructor.
   * @param pipeIn - pipe to read data from.
   * @param pipeOut - pipe to write data to.
   */
  Socket(const std::shared_ptr<Pipe>& pipeIn, const std::shared_ptr<Pipe>& pipeOut);
public:

  /**
   * Create shared socket.
   * @param pipeIn - pipe to read data from.
   * @param pipeOut - pipe to write data to.
   * @return - `std::shared_ptr` to Socket.
   */
  static std::shared_ptr<Socket> createShared(const std::shared_ptr<Pipe>& pipeIn, const std::shared_ptr<Pipe>& pipeOut);

  /**
   * Virtual destructor. Close corresponding pipes.
   */
  ~Socket() override;

  /**
   * Limit the available amount of bytes to read from socket and limit the available amount of bytes to write to socket. <br>
   * This method is used for testing purposes only.<br>
   * @param maxToRead - maximum available amount of bytes to read.
   * @param maxToWrite - maximum available amount of bytes to write.
   */
  void setMaxAvailableToReadWrtie(v_io_size maxToRead, v_io_size maxToWrite);

  /**
   * Read data from socket.
   * @param data - buffer to read data to.
   * @param count - buffer size.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual amount of data read from socket.
   */
  v_io_size read(void *data, v_buff_size count, async::Action& action) override;

  /**
   * Write data to socket.
   * @param data - data to write to socket.
   * @param count - data size.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual amount of data written to socket.
   */
  v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

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
   * Get output stream context.
   * @return
   */
  oatpp::data::stream::Context& getOutputStreamContext() override;

  /**
   * Get input stream context.
   * @return
   */
  oatpp::data::stream::Context& getInputStreamContext() override;

  /**
   * Close socket pipes.
   */
  void close();
  
};
  
}}}

#endif /* oatpp_network_virtual__Socket_hpp */
