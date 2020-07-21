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

#include "StreamBufferedProxy.hpp"

namespace oatpp { namespace data{ namespace stream {
  
v_io_size OutputStreamBufferedProxy::write(const void *data, v_buff_size count, async::Action& action) {
  if(m_buffer.availableToWrite() > 0) {
    return m_buffer.write(data, count);
  } else {
    auto bytesFlushed = m_buffer.readAndWriteToStream(m_outputStream.get(), m_buffer.getBufferSize(), action);
    if(bytesFlushed > 0) {
      return m_buffer.write(data, count);
    }
    return bytesFlushed;
  }
}

void OutputStreamBufferedProxy::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_outputStream->setOutputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode OutputStreamBufferedProxy::getOutputStreamIOMode() {
  return m_outputStream->getOutputStreamIOMode();
}

Context& OutputStreamBufferedProxy::getOutputStreamContext() {
  return m_outputStream->getOutputStreamContext();
}

v_io_size OutputStreamBufferedProxy::flush() {
  return m_buffer.flushToStream(m_outputStream.get());
}

oatpp::async::CoroutineStarter OutputStreamBufferedProxy::flushAsync() {
  return m_buffer.flushToStreamAsync(m_outputStream);
}

v_io_size OutputStreamBufferedProxy::getBytesBufferedCount() {
  return m_buffer.availableToRead();
}
  
v_io_size InputStreamBufferedProxy::read(void *data, v_buff_size count, async::Action& action) {
  
  if(m_buffer.availableToRead() > 0) {
    return m_buffer.read(data, count);
  } else {
    auto bytesBuffered = m_buffer.readFromStreamAndWrite(m_inputStream.get(), m_buffer.getBufferSize(), action);
    if(bytesBuffered > 0) {
      return m_buffer.read(data, count);
    }
    return bytesBuffered;
  }
  
}

v_io_size InputStreamBufferedProxy::peek(void *data, v_buff_size count, async::Action& action) {

  if(m_buffer.availableToRead() > 0) {
    return m_buffer.peek(data, count);
  } else {
    auto bytesBuffered = m_buffer.readFromStreamAndWrite(m_inputStream.get(), m_buffer.getBufferSize(), action);
    if(bytesBuffered > 0) {
      return m_buffer.peek(data, count);
    }
    return bytesBuffered;
  }

}

v_io_size InputStreamBufferedProxy::commitReadOffset(v_buff_size count) {
  return m_buffer.commitReadOffset(count);
}

bool InputStreamBufferedProxy::hasUnreadData() {
  return m_buffer.availableToRead() > 0;
}

void InputStreamBufferedProxy::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_inputStream->setInputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode InputStreamBufferedProxy::getInputStreamIOMode() {
  return m_inputStream->getInputStreamIOMode();
}

Context& InputStreamBufferedProxy::getInputStreamContext() {
  return m_inputStream->getInputStreamContext();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IOStreamBufferedProxy

IOStreamBufferedProxy::IOStreamBufferedProxy(const std::shared_ptr<IOStream>& stream,
                                             v_buff_size inBufferSize,
                                             v_buff_size outBufferSize)
  : m_stream(stream)
  , m_buffer(new v_char8[inBufferSize + outBufferSize])
  , m_inFifo(m_buffer, inBufferSize)
  , m_outFifo(m_buffer + inBufferSize, outBufferSize)
{}

IOStreamBufferedProxy::~IOStreamBufferedProxy() {
  delete [] m_buffer;
}

v_io_size IOStreamBufferedProxy::write(const void *buff, v_buff_size count, async::Action& action) {
  if(m_outFifo.availableToWrite() > 0) {
    return m_outFifo.write(buff, count);
  } else {
    auto bytesFlushed = m_outFifo.readAndWriteToStream(m_stream.get(), m_outFifo.getBufferSize(), action);
    if(bytesFlushed > 0) {
      return m_outFifo.write(buff, count);
    }
    return bytesFlushed;
  }
}

v_io_size IOStreamBufferedProxy::flush() {
  return m_outFifo.flushToStream(m_stream.get());
}

oatpp::async::CoroutineStarter IOStreamBufferedProxy::flushAsync() {
  return m_outFifo.flushToStreamAsync(m_stream);
}

v_io_size IOStreamBufferedProxy::read(void *buff, v_buff_size count, async::Action& action) {
  if(m_inFifo.availableToRead() > 0) {
    return m_inFifo.read(buff, count);
  } else {
    auto bytesBuffered = m_inFifo.readFromStreamAndWrite(m_stream.get(), m_inFifo.getBufferSize(), action);
    if(bytesBuffered > 0) {
      return m_inFifo.read(buff, count);
    }
    return bytesBuffered;
  }
}

v_io_size IOStreamBufferedProxy::peek(void *buff, v_buff_size count, async::Action& action) {
  if(m_inFifo.availableToRead() > 0) {
    return m_inFifo.peek(buff, count);
  } else {
    auto bytesBuffered = m_inFifo.readFromStreamAndWrite(m_stream.get(), m_inFifo.getBufferSize(), action);
    if(bytesBuffered > 0) {
      return m_inFifo.peek(buff, count);
    }
    return bytesBuffered;
  }
}

v_io_size IOStreamBufferedProxy::commitReadOffset(v_buff_size count) {
  return m_inFifo.commitReadOffset(count);
}

bool IOStreamBufferedProxy::hasUnreadData() {
  return m_inFifo.availableToRead() > 0;
}

void IOStreamBufferedProxy::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_stream->setOutputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode IOStreamBufferedProxy::getOutputStreamIOMode() {
  return m_stream->getOutputStreamIOMode();
}

oatpp::data::stream::Context& IOStreamBufferedProxy::getOutputStreamContext() {
  return m_stream->getOutputStreamContext();
}

void IOStreamBufferedProxy::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_stream->setInputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode IOStreamBufferedProxy::IOStreamBufferedProxy::getInputStreamIOMode() {
  return m_stream->getInputStreamIOMode();
}

oatpp::data::stream::Context& IOStreamBufferedProxy::getInputStreamContext() {
  return m_stream->getInputStreamContext();
}

std::shared_ptr<IOStream> IOStreamBufferedProxy::getBaseStream() {
  return m_stream;
}

buffer::FIFOBuffer& IOStreamBufferedProxy::getOutputFifo()  {
  return m_outFifo;
}

buffer::FIFOBuffer& IOStreamBufferedProxy::getInputFifo() {
  return m_inFifo;
}
  
}}}
