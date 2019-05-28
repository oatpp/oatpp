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

#include "FIFOBuffer.hpp"
#include <mutex>

namespace oatpp { namespace data{ namespace buffer {

FIFOBuffer::FIFOBuffer(void* buffer, v_io_size bufferSize,
                       data::v_io_size readPosition, data::v_io_size writePosition,
                       bool canRead)
  : m_buffer((p_char8)buffer)
  , m_bufferSize(bufferSize)
  , m_readPosition(readPosition)
  , m_writePosition(writePosition)
  , m_canRead(canRead)
{}

void FIFOBuffer::setBufferPosition(data::v_io_size readPosition, data::v_io_size writePosition, bool canRead) {
  m_readPosition = readPosition;
  m_writePosition = writePosition;
  m_canRead = canRead;
}

data::v_io_size FIFOBuffer::availableToRead() const {
  if(!m_canRead) {
    return 0;
  }
  if(m_readPosition < m_writePosition) {
    return m_writePosition - m_readPosition;
  }
  return (m_bufferSize - m_readPosition + m_writePosition);
}

data::v_io_size FIFOBuffer::availableToWrite() const {
  if(m_canRead && m_writePosition == m_readPosition) {
    return 0;
  }
  if(m_writePosition < m_readPosition) {
    return m_readPosition - m_writePosition;
  }
  return (m_bufferSize - m_writePosition + m_readPosition);
}

data::v_io_size FIFOBuffer::getBufferSize() const {
  return m_bufferSize;
}

data::v_io_size FIFOBuffer::read(void *data, data::v_io_size count) {
  
  if(!m_canRead) {
    return data::IOError::WAIT_RETRY;
  }
  
  if(count == 0) {
    return 0;
  } else if(count < 0) {
    throw std::runtime_error("[oatpp::data::buffer::FIFOBuffer::read(...)]: count < 0");
  }
  
  if(m_readPosition < m_writePosition) {
    auto size = m_writePosition - m_readPosition;
    if(size > count) {
      size = count;
    }
    std::memcpy(data, &m_buffer[m_readPosition], size);
    m_readPosition += size;
    if(m_readPosition == m_writePosition) {
      m_canRead = false;
    }
    return size;
  }
  
  auto size = m_bufferSize - m_readPosition;
  
  if(size > count){
    std::memcpy(data, &m_buffer[m_readPosition], count);
    m_readPosition += count;
    return count;
  }
  
  std::memcpy(data, &m_buffer[m_readPosition], size);
  auto size2 = m_writePosition;
  if(size2 > count - size) {
    size2 = count - size;
  }
  
  std::memcpy(&((p_char8) data)[size], m_buffer, size2);
  m_readPosition = size2;
  if(m_readPosition == m_writePosition) {
    m_canRead = false;
  }
  
  return (size + size2);
  
}

data::v_io_size FIFOBuffer::write(const void *data, data::v_io_size count) {
  
  if(m_canRead && m_writePosition == m_readPosition) {
    return data::IOError::WAIT_RETRY;
  }
  
  if(count == 0) {
    return 0;
  } else if(count < 0) {
    throw std::runtime_error("[oatpp::data::buffer::FIFOBuffer::write(...)]: count < 0");
  } else {
    m_canRead = true;
  }
  
  if(m_writePosition < m_readPosition) {
    auto size = m_readPosition - m_writePosition;
    if(size > count) {
      size = count;
    }
    std::memcpy(&m_buffer[m_writePosition], data, size);
    m_writePosition += size;
    return size;
  }
  
  auto size = m_bufferSize - m_writePosition;
  
  if(size > count){
    std::memcpy(&m_buffer[m_writePosition], data, count);
    m_writePosition += count;
    return count;
  }
  
  std::memcpy(&m_buffer[m_writePosition], data, size);
  auto size2 = m_readPosition;
  if(size2 > count - size) {
    size2 = count - size;
  }
  
  std::memcpy(m_buffer, &((p_char8) data)[size], size2);
  m_writePosition = size2;
  
  return (size + size2);
  
}

data::v_io_size FIFOBuffer::readAndWriteToStream(data::stream::OutputStream& stream, data::v_io_size count) {

  if(!m_canRead) {
    return data::IOError::WAIT_RETRY;
  }

  if(count == 0) {
    return 0;
  } else if(count < 0) {
    throw std::runtime_error("[oatpp::data::buffer::FIFOBuffer::readAndWriteToStream(...)]: count < 0");
  }

  if(m_readPosition < m_writePosition) {
    auto size = m_writePosition - m_readPosition;
    if(size > count) {
      size = count;
    }
    auto bytesWritten = stream.write(&m_buffer[m_readPosition], size);
    if(bytesWritten > 0) {
      m_readPosition += bytesWritten;
      if (m_readPosition == m_writePosition) {
        m_canRead = false;
      }
    }
    return bytesWritten;
  }

  auto size = m_bufferSize - m_readPosition;

  /* DO NOT call stream.write() twice if size > count !!! */
  if(size > count){
    size = count;
  } else if(size == 0) {

    auto bytesWritten = stream.write(m_buffer, m_writePosition);
    if(bytesWritten > 0) {
      m_readPosition = bytesWritten;
      if (m_readPosition == m_writePosition) {
        m_canRead = false;
      }
    }
    return bytesWritten;

  }

  auto bytesWritten = stream.write(&m_buffer[m_readPosition], size);
  if(bytesWritten > 0) {
    m_readPosition += bytesWritten;
  }
  return bytesWritten;

}

data::v_io_size FIFOBuffer::readFromStreamAndWrite(data::stream::InputStream& stream, data::v_io_size count) {

  if(m_canRead && m_writePosition == m_readPosition) {
    return data::IOError::WAIT_RETRY;
  }

  if(count == 0) {
    return 0;
  } else if(count < 0) {
    throw std::runtime_error("[oatpp::data::buffer::FIFOBuffer::readFromStreamAndWrite(...)]: count < 0");
  }

  if(m_writePosition < m_readPosition) {
    auto size = m_readPosition - m_writePosition;
    if(size > count) {
      size = count;
    }
    auto bytesRead = stream.read(&m_buffer[m_writePosition], size);
    if(bytesRead > 0) {
      m_writePosition += bytesRead;
      m_canRead = true;
    }
    return bytesRead;
  }

  auto size = m_bufferSize - m_writePosition;

  /* DO NOT call stream.read() twice if size > count !!! */
  if(size > count){
    size = count;
  } else if(size == 0) {

    auto bytesRead = stream.read(m_buffer, m_readPosition);
    if(bytesRead > 0) {
      m_writePosition = bytesRead;
      m_canRead = true;
    }

    return bytesRead;

  }

  auto bytesRead = stream.read(&m_buffer[m_writePosition], size);
  if(bytesRead > 0) {
    m_writePosition += bytesRead;
    m_canRead = true;
  }

  return bytesRead;

}

data::v_io_size FIFOBuffer::flushToStream(data::stream::OutputStream& stream) {

  if(!m_canRead) {
    return 0;
  }

  data::v_io_size result = 0;

  if(m_readPosition < m_writePosition) {
    result = data::stream::writeExactSizeData(&stream, &m_buffer[m_readPosition], m_writePosition - m_readPosition);
  } else {
    auto result = data::stream::writeExactSizeData(&stream, &m_buffer[m_readPosition], m_bufferSize - m_readPosition);
    result += data::stream::writeExactSizeData(&stream, m_buffer, m_writePosition);
  }

  setBufferPosition(0, 0, false);

  return result;

}

async::CoroutineStarter FIFOBuffer::flushToStreamAsync(const std::shared_ptr<data::stream::OutputStream>& stream)
{

  class FlushCoroutine : public oatpp::async::Coroutine<FlushCoroutine> {
  private:
    std::shared_ptr<FIFOBuffer> m_fifo;
    std::shared_ptr<data::stream::OutputStream> m_stream;
  private:

    const void* m_data1;
    data::v_io_size m_size1;

    const void* m_data2;
    data::v_io_size m_size2;
  public:

    FlushCoroutine(const std::shared_ptr<FIFOBuffer>& fifo, const std::shared_ptr<data::stream::OutputStream>& stream)
      : m_fifo(fifo)
      , m_stream(stream)
    {}

    Action act() override {

      if(!m_fifo->m_canRead) {
        return finish();
      }

      if(m_fifo->m_readPosition < m_fifo->m_writePosition) {

        m_data1 = &m_fifo->m_buffer[m_fifo->m_readPosition];
        m_size1 = m_fifo->m_writePosition - m_fifo->m_readPosition;

        return yieldTo(&FlushCoroutine::fullFlush);

      } else {

        m_data1 = &m_fifo->m_buffer[m_fifo->m_readPosition];
        m_size1 = m_fifo->m_bufferSize - m_fifo->m_readPosition;

        m_data2 = m_fifo->m_buffer;
        m_size2 = m_fifo->m_writePosition;

        return yieldTo(&FlushCoroutine::partialFlush1);

      }
    }

    Action fullFlush() {
      return data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_data1, m_size1, yieldTo(&FlushCoroutine::beforeFinish));
    }

    Action partialFlush1() {
      return data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_data1, m_size1, yieldTo(&FlushCoroutine::partialFlush2));
    }

    Action partialFlush2() {
      return data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_data2, m_size2, yieldTo(&FlushCoroutine::beforeFinish));
    }

    Action beforeFinish() {
      m_fifo->setBufferPosition(0, 0, false);
      return finish();
    }

  };

  return FlushCoroutine::start(shared_from_this(), stream);

}

//////////////////////////////////////////////////////////////////////////////////////////
// SynchronizedFIFOBuffer

SynchronizedFIFOBuffer::SynchronizedFIFOBuffer(void* buffer, v_io_size bufferSize,
                                               data::v_io_size readPosition, data::v_io_size writePosition,
                                               bool canRead)
  : m_fifo(buffer, bufferSize, readPosition, writePosition, canRead)
{}

void SynchronizedFIFOBuffer::setBufferPosition(data::v_io_size readPosition, data::v_io_size writePosition, bool canRead) {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  m_fifo.setBufferPosition(readPosition, writePosition, canRead);
}

data::v_io_size SynchronizedFIFOBuffer::availableToRead() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  return m_fifo.availableToRead();
}

data::v_io_size SynchronizedFIFOBuffer::availableToWrite() {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  return m_fifo.availableToWrite();
}

data::v_io_size SynchronizedFIFOBuffer::read(void *data, data::v_io_size count) {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  return m_fifo.read(data, count);
}

data::v_io_size SynchronizedFIFOBuffer::write(const void *data, data::v_io_size count) {
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  return m_fifo.write(data, count);
}

}}}
