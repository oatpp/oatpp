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

#ifndef oatpp_data_stream_FIFOStream_hpp
#define oatpp_data_stream_FIFOStream_hpp

#include "Stream.hpp"
#include "oatpp/core/data/buffer/FIFOBuffer.hpp"

namespace oatpp { namespace data { namespace stream {


/**
 * FIFOInputStream
 */
class FIFOInputStream : public BufferedInputStream, public WriteCallback {
 public:
  static data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
 private:
  std::shared_ptr<std::string> m_memoryHandle;
  std::shared_ptr<data::buffer::FIFOBuffer> m_fifo;
  v_buff_size m_maxCapacity;
  IOMode m_ioMode;
 public:

  /**
   * Constructor.
   * @param data - buffer.
   */
  FIFOInputStream(v_buff_size initialSize = 4096);

  static std::shared_ptr<FIFOInputStream> createShared(v_buff_size initialSize = 4096) {
    return std::make_shared<FIFOInputStream>(initialSize);
  }

  /**
   * Discards all data in the buffer and resets it to an empty state
   */
  void reset();

  /**
   * Read data from stream. <br>
   * It is a legal case if return result < count. Caller should handle this!
   * *Calls to this method are always NON-BLOCKING*
   * @param data - buffer to read data to.
   * @param count - size of the buffer.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes read. 0 - designates end of the buffer.
   */
  v_io_size read(void *data, v_buff_size count, async::Action& action) override;

  /**
   * Set stream I/O mode.
   * @throws
   */
  void setInputStreamIOMode(IOMode ioMode) override;

  /**
   * Get stream I/O mode.
   * @return
   */
  IOMode getInputStreamIOMode() override;

  /**
   * Get stream context.
   * @return
   */
  Context& getInputStreamContext() override;

  /**
   * Get data memory handle.
   * @return - data memory handle.
   */
  std::shared_ptr<std::string> getDataMemoryHandle();

  /**
   * Write operation callback.
   * @param data - pointer to data.
   * @param count - size of the data in bytes.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written. 0 - to indicate end-of-file.
   */
  v_io_size write(const void *data, v_buff_size count, async::Action &action) override;

  /**
   * Peek up to count of bytes int he buffer
   * @param data
   * @param count
   * @return [1..count], IOErrors.
   */
  v_io_size peek(void *data, v_buff_size count, async::Action& action) override;

  /**
   * Amount of bytes currently available to read from buffer.
   * @return &id:oatpp::v_io_size;.
   */
  v_io_size availableToRead() const override;

  /**
   * Commit read offset
   * @param count
   * @return [1..count], IOErrors.
   */
  v_io_size commitReadOffset(v_buff_size count) override;

  /**
   * Reserve bytes for future writes. Check &id:oatpp::data::stream::FIFOStream::availableToWrite for the capacity.
   */
  void reserveBytesUpfront(v_buff_size count);

  /**
   * call read and then write bytes read to output stream
   * @param stream
   * @param count
   * @param action
   * @return [1..count], IOErrors.
   */
  v_io_size readAndWriteToStream(data::stream::OutputStream* stream, v_buff_size count, async::Action& action);

  /**
   * call stream.read() and then write bytes read to buffer
   * @param stream
   * @param count
   * @param action
   * @return
   */
  v_io_size readFromStreamAndWrite(data::stream::InputStream* stream, v_buff_size count, async::Action& action);

  /**
   * flush all availableToRead bytes to stream
   * @param stream
   * @return
   */
  v_io_size flushToStream(data::stream::OutputStream* stream);

  /**
   * flush all availableToRead bytes to stream in asynchronous manner
   * @param stream - &id:data::stream::OutputStream;.
   * @return - &id:async::CoroutineStarter;.
   */
  async::CoroutineStarter flushToStreamAsync(const std::shared_ptr<data::stream::OutputStream>& stream);

  /**
   * Amount of buffer space currently available for data writes.
   * @return &id:oatpp::v_io_size;.
   */
  v_io_size availableToWrite();
};

}}}

#endif // oatpp_data_stream_FIFOStream_hpp
