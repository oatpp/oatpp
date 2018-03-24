/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "./Stream.hpp"
#include "../buffer/IOBuffer.hpp"
#include "../../async/Coroutine.hpp"

namespace oatpp { namespace data{ namespace stream {
  
class OutputStreamBufferedProxy : public oatpp::base::Controllable, public OutputStream {
public:
  OBJECT_POOL(OutputStreamBufferedProxy_Pool, OutputStreamBufferedProxy, 32)
  SHARED_OBJECT_POOL(Shared_OutputStreamBufferedProxy_Pool, OutputStreamBufferedProxy, 32)
public:
  typedef v_int32 v_bufferSize;
private:
  std::shared_ptr<OutputStream> m_outputStream;
  std::shared_ptr<oatpp::data::buffer::IOBuffer> m_bufferPtr;
  p_char8 m_buffer;
  v_bufferSize m_bufferSize;
  v_bufferSize m_pos;
  v_bufferSize m_posEnd;
public:
  OutputStreamBufferedProxy(const std::shared_ptr<OutputStream>& outputStream,
                            const std::shared_ptr<oatpp::data::buffer::IOBuffer>& bufferPtr,
                            p_char8 buffer,
                            v_bufferSize bufferSize)
    : m_outputStream(outputStream)
    , m_bufferPtr(bufferPtr)
    , m_buffer(buffer)
    , m_bufferSize(bufferSize)
    , m_pos(0)
    , m_posEnd(0)
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
  
  os::io::Library::v_size write(const void *data, os::io::Library::v_size count) override;
  os::io::Library::v_size flush();
  oatpp::async::Action2 flushAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   const oatpp::async::Action2& actionOnFinish);
  
  void setBufferPosition(v_bufferSize pos, v_bufferSize posEnd){
    m_pos = pos;
    m_posEnd = posEnd;
  }
  
};
  
class InputStreamBufferedProxy : public oatpp::base::Controllable, public InputStream {
public:
  OBJECT_POOL(InputStreamBufferedProxy_Pool, InputStreamBufferedProxy, 32)
  SHARED_OBJECT_POOL(Shared_InputStreamBufferedProxy_Pool, InputStreamBufferedProxy, 32)
public:
  typedef v_int32 v_bufferSize;
protected:
  std::shared_ptr<InputStream> m_inputStream;
  std::shared_ptr<oatpp::data::buffer::IOBuffer> m_bufferPtr;
  p_char8 m_buffer;
  v_bufferSize m_bufferSize;
  v_bufferSize m_pos;
  v_bufferSize m_posEnd;
public:
  InputStreamBufferedProxy(const std::shared_ptr<InputStream>& inputStream,
                           const std::shared_ptr<oatpp::data::buffer::IOBuffer>& bufferPtr,
                           p_char8 buffer,
                           v_bufferSize bufferSize,
                           v_bufferSize positionStart,
                           v_bufferSize positionEnd)
    : m_inputStream(inputStream)
    , m_bufferPtr(bufferPtr)
    , m_buffer(buffer)
    , m_bufferSize(bufferSize)
    , m_pos(positionStart)
    , m_posEnd(positionEnd)
  {}
public:
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                          const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                buffer,
                                                                (p_char8) buffer->getData(),
                                                                buffer->getSize(),
                                                                0, 0);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                          const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer,
                                                          v_bufferSize positionStart,
                                                          v_bufferSize positionEnd)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                buffer,
                                                                (p_char8) buffer->getData(),
                                                                buffer->getSize(),
                                                                positionStart,
                                                                positionEnd);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                          p_char8 buffer,
                                                          v_bufferSize bufferSize)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                nullptr,
                                                                buffer,
                                                                bufferSize,
                                                                0, 0);
  }
  
  static std::shared_ptr<InputStreamBufferedProxy> createShared(const std::shared_ptr<InputStream>& inputStream,
                                                          p_char8 buffer,
                                                          v_bufferSize bufferSize,
                                                          v_bufferSize positionStart,
                                                          v_bufferSize positionEnd)
  {
    return Shared_InputStreamBufferedProxy_Pool::allocateShared(inputStream,
                                                                nullptr,
                                                                buffer,
                                                                bufferSize,
                                                                positionStart,
                                                                positionEnd);
  }
  
  os::io::Library::v_size read(void *data, os::io::Library::v_size count) override;
  
  void setBufferPosition(v_bufferSize pos, v_bufferSize posEnd){
    m_pos = pos;
    m_posEnd = posEnd;
  }
  
};
  
}}}

#endif /* oatpp_data_stream_StreamBufferedProxy_hpp */
