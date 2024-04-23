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

void InputStreamBufferedProxy::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_inputStream->setInputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode InputStreamBufferedProxy::getInputStreamIOMode() {
  return m_inputStream->getInputStreamIOMode();
}

Context& InputStreamBufferedProxy::getInputStreamContext() {
  return m_inputStream->getInputStreamContext();
}

v_io_size InputStreamBufferedProxy::availableToRead() const {
  return m_buffer.availableToRead();
}

}}}
