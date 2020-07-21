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

#ifndef oatpp_data_stream_StreamBufferedProxy_hpp
#define oatpp_data_stream_StreamBufferedProxy_hpp

#include "Stream.hpp"
#include "oatpp/core/data/buffer/FIFOBuffer.hpp"
#include "oatpp/core/data/share/MemoryLabel.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace data{ namespace stream {
  
class OutputStreamBufferedProxy : public oatpp::base::Countable, public OutputStream {
public:
  OBJECT_POOL(OutputStreamBufferedProxy_Pool, OutputStreamBufferedProxy, 32)
  SHARED_OBJECT_POOL(Shared_OutputStreamBufferedProxy_Pool, OutputStreamBufferedProxy, 32)
private:
  std::shared_ptr<OutputStream> m_outputStream;
  oatpp::data::share::MemoryLabel m_memoryLabel;
  buffer::FIFOBuffer m_buffer;
public:
  OutputStreamBufferedProxy(const std::shared_ptr<OutputStream>& outputStream,
                            const oatpp::data::share::MemoryLabel& memoryLabel)
    : m_outputStream(outputStream)
    , m_memoryLabel(memoryLabel)
    , m_buffer(memoryLabel.getData(), memoryLabel.getSize())
  {}
public:
  
  static std::shared_ptr<OutputStreamBufferedProxy> createShared(const std::shared_ptr<OutputStream>& outputStream, const oatpp::data::share::MemoryLabel& memoryLabel) {
    return Shared_OutputStreamBufferedProxy_Pool::allocateShared(outputStream, memoryLabel);
  }
  
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
   * Get context of the underlying stream.
   * @return
   */
  Context& getOutputStreamContext() override;

  v_io_size flush();
  oatpp::async::CoroutineStarter flushAsync();

  void setBufferPosition(v_io_size readPosition, v_io_size writePosition, bool canRead) {
    m_buffer.setBufferPosition(readPosition, writePosition, canRead);
  }

  v_io_size getBytesBufferedCount();
  
};
  
class InputStreamBufferedProxy : public oatpp::base::Countable, public InputStream {
public:
  OBJECT_POOL(InputStreamBufferedProxy_Pool, InputStreamBufferedProxy, 32)
  SHARED_OBJECT_POOL(Shared_InputStreamBufferedProxy_Pool, InputStreamBufferedProxy, 32)
protected:
  std::shared_ptr<InputStream> m_inputStream;
  oatpp::data::share::MemoryLabel m_memoryLabel;
  buffer::FIFOBuffer m_buffer;
public:
  InputStreamBufferedProxy(const std::shared_ptr<InputStream>& inputStream,
                           const oatpp::data::share::MemoryLabel& memoryLabel,
                           v_io_size bufferReadPosition,
                           v_io_size bufferWritePosition,
                           bool bufferCanRead)
    : m_inputStream(inputStream)
    , m_memoryLabel(memoryLabel)
    , m_buffer(memoryLabel.getData(), memoryLabel.getSize(), bufferReadPosition, bufferWritePosition, bufferCanRead)
  {}
public:
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                                const oatpp::data::share::MemoryLabel& memoryLabel)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream, memoryLabel, 0, 0, false);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                                const oatpp::data::share::MemoryLabel& memoryLabel,
                                                                v_io_size bufferReadPosition,
                                                                v_io_size bufferWritePosition,
                                                                bool bufferCanRead)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream, memoryLabel, bufferReadPosition, bufferWritePosition, bufferCanRead);
  }
  
  v_io_size read(void *data, v_buff_size count, async::Action& action) override;

  v_io_size peek(void *data, v_buff_size count, async::Action& action);

  v_io_size commitReadOffset(v_buff_size count);

  bool hasUnreadData();

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
   * Get context of the underlying stream.
   * @return
   */
  Context& getInputStreamContext() override;

  void setBufferPosition(v_io_size readPosition, v_io_size writePosition, bool canRead) {
    m_buffer.setBufferPosition(readPosition, writePosition, canRead);
  }
  
};

class IOStreamBufferedProxy : public IOStream {
private:
  std::shared_ptr<IOStream> m_stream;
  p_char8 m_buffer;
  buffer::FIFOBuffer m_inFifo;
  buffer::FIFOBuffer m_outFifo;
public:

  /**
   * Constructor.
   * @param stream - base stream.
   * @param inBufferSize - input buffer size.
   * @param outBufferSize - output buffer size.
   */
  IOStreamBufferedProxy(const std::shared_ptr<IOStream>& stream,
                        v_buff_size inBufferSize,
                        v_buff_size outBufferSize);

  /**
   * Virtual destructor.
   */
  ~IOStreamBufferedProxy();

  /**
   * Implementation of &id:oatpp::data::stream::IOStream::write;.
   * @param buff - buffer containing data to write.
   * @param count - bytes count you want to write.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual amount of bytes written. See &id:oatpp::v_io_size;.
   */
  v_io_size write(const void *buff, v_buff_size count, async::Action& action) override;

  /**
   * Flush output buffer to the underlying stream.
   * @return
   */
  v_io_size flush();

  /**
   * Flush output buffer to the underlying stream (Asynchronously).
   * @return
   */
  oatpp::async::CoroutineStarter flushAsync();

  /**
   * Implementation of &id:oatpp::data::stream::IOStream::read;.
   * @param buff - buffer to read data to.
   * @param count - buffer size.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual amount of bytes read. See &id:oatpp::v_io_size;.
   */
  v_io_size read(void *buff, v_buff_size count, async::Action& action) override;

  /**
   * Peek data available in buffer or read from stream.
   * @param buff
   * @param count
   * @param action
   * @return
   */
  v_io_size peek(void *buff, v_buff_size count, async::Action& action);

  /**
   * Claim amount of bytes read. Typically called after `peek()`.
   * @param count
   * @return
   */
  v_io_size commitReadOffset(v_buff_size count);

  /**
   * Check if the input buffer has unread data.
   * @return
   */
  bool hasUnreadData();

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

  /**
   * Get the underlying stream.
   * @return
   */
  std::shared_ptr<IOStream> getBaseStream();

  /**
   * Get output FIFO - &id:oatpp::data::buffer::FIFOBuffer;.
   * @return
   */
  buffer::FIFOBuffer& getOutputFifo();

  /**
   * Get input FIFO - &id:oatpp::data::buffer::FIFOBuffer;.
   * @return
   */
  buffer::FIFOBuffer& getInputFifo();

};
  
}}}

#endif /* oatpp_data_stream_StreamBufferedProxy_hpp */
