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

#include "BufferStream.hpp"

#include "oatpp/utils/Binary.hpp"

namespace oatpp { namespace data{ namespace stream {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferOutputStream

data::stream::DefaultInitializedContext BufferOutputStream::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_INFINITE);

BufferOutputStream::BufferOutputStream(v_buff_size initialCapacity, const std::shared_ptr<void>& captureData)
  : m_data(new v_char8[static_cast<unsigned long>(initialCapacity)])
  , m_capacity(initialCapacity)
  , m_position(0)
  , m_maxCapacity(-1)
  , m_ioMode(IOMode::ASYNCHRONOUS)
  , m_capturedData(captureData)
{}

BufferOutputStream::~BufferOutputStream() {
  m_capturedData.reset(); // reset capture data before deleting data.
  delete [] m_data;
}

v_io_size BufferOutputStream::write(const void *data, v_buff_size count, async::Action& action) {

  (void) action;

  reserveBytesUpfront(count);

  std::memcpy(m_data + m_position, data, static_cast<size_t>(count));
  m_position += count;

  return count;

}

void BufferOutputStream::setOutputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode BufferOutputStream::getOutputStreamIOMode() {
  return m_ioMode;
}

Context& BufferOutputStream::getOutputStreamContext() {
  return DEFAULT_CONTEXT;
}

void BufferOutputStream::reserveBytesUpfront(v_buff_size count) {

  v_buff_size capacityNeeded = m_position + count;

  if(capacityNeeded > m_capacity) {

    v_buff_size newCapacity = utils::Binary::nextP2(capacityNeeded);

    if(newCapacity < 0 || (m_maxCapacity > 0 && newCapacity > m_maxCapacity)) {
      newCapacity = m_maxCapacity;
    }

    if(newCapacity < capacityNeeded) {
      throw std::runtime_error("[oatpp::data::stream::BufferOutputStream::reserveBytesUpfront()]: Error. Unable to allocate requested memory.");
    }

    auto newData = new v_char8[static_cast<unsigned long>(newCapacity)];

    std::memcpy(newData, m_data, static_cast<size_t>(m_position));
    delete [] m_data;
    m_data = newData;
    m_capacity = newCapacity;

  }

}

p_char8 BufferOutputStream::getData() {
  return m_data;
}


v_buff_size BufferOutputStream::getCapacity() {
  return m_capacity;
}


v_buff_size BufferOutputStream::getCurrentPosition() {
  return m_position;
}


void BufferOutputStream::setCurrentPosition(v_buff_size position) {
  m_position = position;
}

void BufferOutputStream::reset(v_buff_size initialCapacity) {
  delete [] m_data;
  m_data = new v_char8[static_cast<unsigned long>(initialCapacity)];
  m_capacity = initialCapacity;
  m_position = 0;
}

oatpp::String BufferOutputStream::toString() {
  return oatpp::String(reinterpret_cast<const char*>(m_data), m_position);
}

std::string BufferOutputStream::toStdString() const {
  return std::string(reinterpret_cast<const char*>(m_data), static_cast<unsigned long>(m_position));
}

oatpp::String BufferOutputStream::getSubstring(v_buff_size pos, v_buff_size count) {
  if(pos + count <= m_position) {
    return oatpp::String(reinterpret_cast<const char*>(m_data + pos), count);
  } else {
    return oatpp::String(reinterpret_cast<const char*>(m_data + pos), m_position - pos);
  }
}

oatpp::v_io_size BufferOutputStream::flushToStream(OutputStream* stream) {
  return stream->writeExactSizeDataSimple(m_data, m_position);
}

oatpp::async::CoroutineStarter BufferOutputStream::flushToStreamAsync(const std::shared_ptr<BufferOutputStream>& _this, const std::shared_ptr<OutputStream>& stream) {

  class WriteDataCoroutine : public oatpp::async::Coroutine<WriteDataCoroutine> {
  private:
    std::shared_ptr<BufferOutputStream> m_this;
    std::shared_ptr<oatpp::data::stream::OutputStream> m_stream;
    data::buffer::InlineWriteData m_inlineData;
  public:

    WriteDataCoroutine(const std::shared_ptr<BufferOutputStream>& _this,
                       const std::shared_ptr<oatpp::data::stream::OutputStream>& stream)
      : m_this(_this)
      , m_stream(stream)
    {}

    Action act() override {
      if(m_inlineData.currBufferPtr == nullptr) {
        m_inlineData.currBufferPtr = m_this->m_data;
        m_inlineData.bytesLeft = m_this->m_position;
      }
      return m_stream.get()->writeExactSizeDataAsyncInline(m_inlineData, finish());
    }

  };

  return WriteDataCoroutine::start(_this, stream);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferInputStream

data::stream::DefaultInitializedContext BufferInputStream::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_FINITE);

BufferInputStream::BufferInputStream(const std::shared_ptr<std::string>& memoryHandle,
                                     const void* data,
                                     v_buff_size size,
                                     const std::shared_ptr<void>& captureData)
  : m_memoryHandle(memoryHandle)
  , m_data(reinterpret_cast<p_char8>(const_cast<void*>(data)))
  , m_size(size)
  , m_position(0)
  , m_ioMode(IOMode::ASYNCHRONOUS)
  , m_capturedData(captureData)
{}

BufferInputStream::BufferInputStream(const oatpp::String& data, const std::shared_ptr<void>& captureData)
  : BufferInputStream(data.getPtr(), reinterpret_cast<p_char8>(data->data()), static_cast<v_buff_size>(data->size()), captureData)
{}

void BufferInputStream::reset(const std::shared_ptr<std::string>& memoryHandle,
                              p_char8 data,
                              v_buff_size size,
                              const std::shared_ptr<void>& captureData)
{
  m_memoryHandle = memoryHandle;
  m_data = data;
  m_size = size;
  m_position = 0;
  m_capturedData = captureData;
}

void BufferInputStream::reset() {
  m_memoryHandle = nullptr;
  m_data = nullptr;
  m_size = 0;
  m_position = 0;
  m_capturedData.reset();
}

v_io_size BufferInputStream::read(void *data, v_buff_size count, async::Action& action) {

  (void) action;

  v_buff_size desiredAmount = count;
  if(desiredAmount > m_size - m_position) {
    desiredAmount = m_size - m_position;
  }
  std::memcpy(data, &m_data[m_position], static_cast<size_t>(desiredAmount));
  m_position += desiredAmount;
  return desiredAmount;
}

void BufferInputStream::setInputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode BufferInputStream::getInputStreamIOMode() {
  return m_ioMode;
}

Context& BufferInputStream::getInputStreamContext() {
  return DEFAULT_CONTEXT;
}

std::shared_ptr<std::string> BufferInputStream::getDataMemoryHandle() {
  return m_memoryHandle;
}

p_char8 BufferInputStream::getData() {
  return m_data;
}

v_buff_size BufferInputStream::getDataSize() {
  return m_size;
}

v_buff_size BufferInputStream::getCurrentPosition() {
  return m_position;
}

void BufferInputStream::setCurrentPosition(v_buff_size position) {
  m_position = position;
}

v_io_size BufferInputStream::peek(void *data, v_buff_size count, async::Action &action) {
  (void) action;

  v_buff_size desiredAmount = count;
  if(desiredAmount > m_size - m_position) {
    desiredAmount = m_size - m_position;
  }
  std::memcpy(data, &m_data[m_position], static_cast<size_t>(desiredAmount));
  return desiredAmount;
}

v_io_size BufferInputStream::availableToRead() const {
  return m_size - m_position;
}

v_io_size BufferInputStream::commitReadOffset(v_buff_size count) {
  if(count > m_size - m_position) {
    count = m_size - m_position;
  }
  m_position += count;
  return count;
}

}}}
