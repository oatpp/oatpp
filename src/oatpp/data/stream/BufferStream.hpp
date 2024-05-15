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

#ifndef oatpp_data_stream_BufferStream_hpp
#define oatpp_data_stream_BufferStream_hpp

#include "Stream.hpp"

namespace oatpp { namespace data{ namespace stream {

/**
 * BufferOutputStream
 */
class BufferOutputStream : public ConsistentOutputStream {
public:
  static data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
private:
  p_char8 m_data;
  v_buff_size m_capacity;
  v_buff_size m_position;
  v_buff_size m_maxCapacity;
  IOMode m_ioMode;
private:
  std::shared_ptr<void> m_capturedData;
public:

  /**
   * Constructor.
   * @param growBytes
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  BufferOutputStream(v_buff_size initialCapacity = 2048, const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Virtual destructor.
   */
  ~BufferOutputStream() override;

  /**
   * Write `count` of bytes to stream.
   * @param data - data to write.
   * @param count - number of bytes to write.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written. &id:oatpp::v_io_size;.
   */
  v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

  /**
   * Set stream I/O mode.
   * @throws
   */
  void setOutputStreamIOMode(IOMode ioMode) override;

  /**
   * Get stream I/O mode.
   * @return
   */
  IOMode getOutputStreamIOMode() override;

  /**
   * Get stream context.
   * @return
   */
  Context& getOutputStreamContext() override;

  /**
   * Reserve bytes for future writes.
   */
  void reserveBytesUpfront(v_buff_size count);

  /**
   * Get pointer to data.
   * @return - pointer to data.
   */
  p_char8 getData();

  /**
   * Get current capacity.
   * Capacity may change.
   * @return
   */
  v_buff_size getCapacity();

  /**
   * Get current data write position.
   * @return - current data write position.
   */
  v_buff_size getCurrentPosition();

  /**
   * Set current data write position.
   * @param position - data write position.
   */
  void setCurrentPosition(v_buff_size position);

  /**
   * Reset stream buffer and its capacity. Also reset write position.
   * @param initialCapacity
   */
  void reset(v_buff_size initialCapacity = 2048);

  /**
   * Copy data to &id:oatpp::String;.
   * @return
   */
  oatpp::String toString();

  /**
   * Copy data to `std::string`.
   * @return
   */
  std::string toStdString() const;

  /**
   * Create &id:oatpp::String; from part of buffer.
   * @param pos - starting position in buffer.
   * @param count - size of bytes to write to substring.
   * @return - &id:oatpp::String;
   */
  oatpp::String getSubstring(v_buff_size pos, v_buff_size count);

  /**
   * Write all bytes from buffer to stream.
   * @param stream - stream to flush all data to.
   * @return - actual amount of bytes flushed.
   */
  oatpp::v_io_size flushToStream(OutputStream* stream);

  /**
   * Write all bytes from buffer to stream in async manner.
   * @param _this - pointer to `this` buffer.
   * @param stream - stream to flush all data to.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  static oatpp::async::CoroutineStarter flushToStreamAsync(const std::shared_ptr<BufferOutputStream>& _this, const std::shared_ptr<OutputStream>& stream);

};

/**
 * BufferInputStream
 */
class BufferInputStream : public BufferedInputStream {
public:
  static data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
private:
  std::shared_ptr<std::string> m_memoryHandle;
  p_char8 m_data;
  v_buff_size m_size;
  v_buff_size m_position;
  IOMode m_ioMode;
private:
  std::shared_ptr<void> m_capturedData;
public:

  /**
   * Constructor.
   * @param memoryHandle - buffer memory handle. May be nullptr.
   * @param data - pointer to buffer data.
   * @param size - size of the buffer.
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  BufferInputStream(const std::shared_ptr<std::string>& memoryHandle,
                    const void* data,
                    v_buff_size size,
                    const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Constructor.
   * @param data - buffer.
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  BufferInputStream(const oatpp::String& data, const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Reset stream data and set position to `0`.
   * @param memoryHandle - buffer memory handle. May be nullptr.
   * @param data - pointer to buffer data.
   * @param size - size of the buffer.
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  void reset(const std::shared_ptr<std::string>& memoryHandle,
             p_char8 data,
             v_buff_size size,
             const std::shared_ptr<void>& captureData = nullptr);


  /**
   * Same as `reset(nullptr, nullptr, 0);.`
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
   * Get pointer to data.
   * @return - pointer to data.
   */
  p_char8 getData();

  /**
   * Get data size.
   * @return - data size.
   */
  v_buff_size getDataSize();

  /**
   * Get current data read position.
   * @return - current data read position.
   */
  v_buff_size getCurrentPosition();

  /**
   * Set current data read position.
   * @param position - data read position.
   */
  void setCurrentPosition(v_buff_size position);

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

};

}}}

#endif // oatpp_data_stream_BufferStream_hpp
