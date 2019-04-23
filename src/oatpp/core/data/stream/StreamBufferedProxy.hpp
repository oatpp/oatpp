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
#include "oatpp/core/data/buffer/IOBuffer.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace data{ namespace stream {
  
class OutputStreamBufferedProxy : public oatpp::base::Countable, public OutputStream {
public:
  OBJECT_POOL(OutputStreamBufferedProxy_Pool, OutputStreamBufferedProxy, 32)
  SHARED_OBJECT_POOL(Shared_OutputStreamBufferedProxy_Pool, OutputStreamBufferedProxy, 32)
public:
  typedef v_int32 v_bufferSize;
private:
  std::shared_ptr<OutputStream> m_outputStream;
  std::shared_ptr<oatpp::data::buffer::IOBuffer> m_bufferPtr;
  std::shared_ptr<buffer::FIFOBuffer> m_buffer;
public:
  OutputStreamBufferedProxy(const std::shared_ptr<OutputStream>& outputStream,
                            const std::shared_ptr<oatpp::data::buffer::IOBuffer>& bufferPtr,
                            p_char8 buffer,
                            v_bufferSize bufferSize)
    : m_outputStream(outputStream)
    , m_bufferPtr(bufferPtr)
    , m_buffer(std::make_shared<buffer::FIFOBuffer>(buffer, bufferSize))
  {}
public:
  
  static std::shared_ptr<OutputStreamBufferedProxy> createShared(const std::shared_ptr<OutputStream>& outputStream,
                                                                 const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
  {
    return Shared_OutputStreamBufferedProxy_Pool::allocateShared(outputStream,
                                                                 buffer,
                                                                 (p_char8) buffer->getData(),
                                                                 buffer->getSize());
  }
  
  static std::shared_ptr<OutputStreamBufferedProxy> createShared(const std::shared_ptr<OutputStream>& outputStream,
                                                           p_char8 buffer,
                                                           v_bufferSize bufferSize)
  {
    return Shared_OutputStreamBufferedProxy_Pool::allocateShared(outputStream,
                                                                 nullptr,
                                                                 buffer,
                                                                 bufferSize);
  }
  
  data::v_io_size write(const void *data, data::v_io_size count) override;

  oatpp::async::Action suggestOutputStreamAction(data::v_io_size ioResult) override;

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

  data::v_io_size flush();
  oatpp::async::CoroutineStarter flushAsync();

  void setBufferPosition(data::v_io_size readPosition, data::v_io_size writePosition, bool canRead) {
    m_buffer->setBufferPosition(readPosition, writePosition, canRead);
  }
  
};
  
class InputStreamBufferedProxy : public oatpp::base::Countable, public InputStream {
public:
  OBJECT_POOL(InputStreamBufferedProxy_Pool, InputStreamBufferedProxy, 32)
  SHARED_OBJECT_POOL(Shared_InputStreamBufferedProxy_Pool, InputStreamBufferedProxy, 32)
public:
  typedef v_int32 v_bufferSize;
protected:
  std::shared_ptr<InputStream> m_inputStream;
  std::shared_ptr<oatpp::data::buffer::IOBuffer> m_bufferPtr;
  buffer::FIFOBuffer m_buffer;
public:
  InputStreamBufferedProxy(const std::shared_ptr<InputStream>& inputStream,
                           const std::shared_ptr<oatpp::data::buffer::IOBuffer>& bufferPtr,
                           p_char8 buffer,
                           v_bufferSize bufferSize,
                           data::v_io_size bufferReadPosition,
                           data::v_io_size bufferWritePosition,
                           bool bufferCanRead)
    : m_inputStream(inputStream)
    , m_bufferPtr(bufferPtr)
    , m_buffer(buffer, bufferSize, bufferReadPosition, bufferWritePosition, bufferCanRead)
  {}
public:
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                                const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                buffer,
                                                                (p_char8) buffer->getData(),
                                                                buffer->getSize(),
                                                                0, 0, false);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                                const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer,
                                                                data::v_io_size bufferReadPosition,
                                                                data::v_io_size bufferWritePosition,
                                                                bool bufferCanRead)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                buffer,
                                                                (p_char8) buffer->getData(),
                                                                buffer->getSize(),
                                                                bufferReadPosition,
                                                                bufferWritePosition,
                                                                bufferCanRead);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                                p_char8 buffer,
                                                                v_bufferSize bufferSize)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                nullptr,
                                                                buffer,
                                                                bufferSize,
                                                                0, 0, false);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                                p_char8 buffer,
                                                                v_bufferSize bufferSize,
                                                                data::v_io_size bufferReadPosition,
                                                                data::v_io_size bufferWritePosition,
                                                                bool bufferCanRead)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                nullptr,
                                                                buffer,
                                                                bufferSize,
                                                                bufferReadPosition,
                                                                bufferWritePosition,
                                                                bufferCanRead);
  }
  
  data::v_io_size read(void *data, data::v_io_size count) override;

  oatpp::async::Action suggestInputStreamAction(data::v_io_size ioResult) override;

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

  void setBufferPosition(data::v_io_size readPosition, data::v_io_size writePosition, bool canRead) {
    m_buffer.setBufferPosition(readPosition, writePosition, canRead);
  }
  
};
  
}}}

#endif /* oatpp_data_stream_StreamBufferedProxy_hpp */
