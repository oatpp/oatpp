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
  ~Socket();

  /**
   * Limit the available amount of bytes to read from socket and limit the available amount of bytes to write to socket. <br>
   * This method is used for testing purposes only.<br>
   * @param maxToRead - maximum available amount of bytes to read.
   * @param maxToWrite - maximum available amount of bytes to write.
   */
  void setMaxAvailableToReadWrtie(data::v_io_size maxToRead, data::v_io_size maxToWrite);

  /**
   * Read data from socket.
   * @param data - buffer to read data to.
   * @param count - buffer size.
   * @return - actual amount of data read from socket.
   */
  data::v_io_size read(void *data, data::v_io_size count) override;

  /**
   * Write data to socket.
   * @param data - data to write to socket.
   * @param count - data size.
   * @return - actual amount of data written to socket.
   */
  data::v_io_size write(const void *data, data::v_io_size count) override;

  /**
   * Implementation of OutputStream must suggest async actions for I/O results.
   * Suggested Action is used for scheduling coroutines in async::Executor.
   * @param ioResult - result of the call to &l:OutputStream::write ();.
   * @return - &id:oatpp::async::Action;.
   */
  oatpp::async::Action suggestOutputStreamAction(data::v_io_size ioResult) override;

  /**
   * Implementation of InputStream must suggest async actions for I/O results.
   * Suggested Action is used for scheduling coroutines in async::Executor.
   * @param ioResult - result of the call to &l:InputStream::read ();.
   * @return - &id:oatpp::async::Action;.
   */
  oatpp::async::Action suggestInputStreamAction(data::v_io_size ioResult) override;

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
   * Close socket pipes.
   */
  void close();
  
};
  
}}}

#endif /* oatpp_network_virtual__Socket_hpp */
