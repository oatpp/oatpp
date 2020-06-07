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

#ifndef oatpp_network_BufferedConnection_hpp
#define oatpp_network_BufferedConnection_hpp

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace network {

/**
 * Buffered Connection implementation. Extends &id:oatpp::base::Countable; and &id:oatpp::data::stream::IOStream;.
 */
class BufferedConnection : public oatpp::base::Countable, public oatpp::data::stream::IOStream {
 public:

 protected:
  std::vector<v_char8> m_in;
  v_io_size m_in_off = 0;

  std::vector<v_char8> m_out;
  v_io_size m_maxOutBuffSize = 0;

  v_io_handle m_handle;

 public:

  /**
   * Constructor.
   */
  explicit BufferedConnection(v_io_handle handle, v_io_size maxBufferSize);

  virtual ~BufferedConnection() {};

  /**
   * Override of &id:oatpp::data::stream::IOStream::write. Writes data to a buffer instead of the Network-Stream
   * @param buff - buffer containing data to write.
   * @param count - bytes count you want to write.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual amount of bytes written. See &id:oatpp::v_io_size;.
   */
  v_io_size write(const void *buff, v_buff_size count, async::Action &action) final;

  /**
   * Override of &id:oatpp::data::stream::IOStream::write. Reads data from the buffered messages instead of the Network-Stream
   * @param buff - buffer to read data to.
   * @param count - buffer size.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual amount of bytes read. See &id:oatpp::v_io_size;.
   */
  v_io_size read(void *buff, v_buff_size count, async::Action &action) final;

  /**
   * This function shall clear the output buffer and writes it to the Network-Stream.
   */
  virtual void flush() = 0;

  /**
 * This function shall read all available data and put it into the `m_in` buffer.
 * @return bytes read
 */
  virtual v_io_size populate() = 0;

};

}
}

#endif /* oatpp_network_BufferedConnection_hpp */
