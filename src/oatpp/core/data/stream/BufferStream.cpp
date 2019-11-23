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

namespace oatpp { namespace data{ namespace stream {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferOutputStream

BufferOutputStream::BufferOutputStream(v_buff_size initialCapacity, v_buff_size growBytes)
  : m_data(new v_char8[initialCapacity])
  , m_capacity(initialCapacity)
  , m_position(0)
  , m_growBytes(growBytes)
  , m_ioMode(IOMode::NON_BLOCKING)
{}

BufferOutputStream::~BufferOutputStream() {
  delete [] m_data;
}

data::v_io_size BufferOutputStream::write(const void *data, v_buff_size count) {

  reserveBytesUpfront(count);

  std::memcpy(m_data + m_position, data, count);
  m_position += count;

  return count;

}

void BufferOutputStream::setOutputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode BufferOutputStream::getOutputStreamIOMode() {
  return m_ioMode;
}

void BufferOutputStream::reserveBytesUpfront(v_buff_size count) {

  if(m_position + count > m_capacity) {

    if(m_growBytes <= 0) {
      throw std::runtime_error("[oatpp::data::stream::BufferOutputStream::reserveBytesUpfront()]: Error. Buffer was not allowed to grow.");
    }

    v_buff_size extraNeeded = m_position + count - m_capacity;
    v_buff_size extraChunks = extraNeeded / m_growBytes;

    if(extraChunks * m_growBytes < extraNeeded) {
      extraChunks ++;
    }

    v_buff_size newCapacity = m_capacity + extraChunks * m_growBytes;
    p_char8 newData = new v_char8[newCapacity];

    std::memcpy(newData, m_data, m_position);
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

oatpp::String BufferOutputStream::toString() {
  return oatpp::String((const char*)m_data, m_position, true);
}

oatpp::String BufferOutputStream::getSubstring(v_buff_size pos, v_buff_size count) {
  if(pos + count <= m_position) {
    return oatpp::String((const char *) (m_data + pos), count, true);
  } else {
    return oatpp::String((const char *) (m_data + pos), m_position - pos, true);
  }
}

oatpp::data::v_io_size BufferOutputStream::flushToStream(OutputStream* stream) {
  return oatpp::data::stream::writeExactSizeData(stream, m_data, m_position);
}

oatpp::async::CoroutineStarter BufferOutputStream::flushToStreamAsync(const std::shared_ptr<BufferOutputStream>& _this, const std::shared_ptr<OutputStream>& stream) {

  class WriteDataCoroutine : public oatpp::async::Coroutine<WriteDataCoroutine> {
  private:
    std::shared_ptr<BufferOutputStream> m_this;
    std::shared_ptr<oatpp::data::stream::OutputStream> m_stream;
    AsyncInlineWriteData m_inlineData;
  public:

    WriteDataCoroutine(const std::shared_ptr<BufferOutputStream>& _this,
                       const std::shared_ptr<oatpp::data::stream::OutputStream>& stream)
      : m_this(_this)
      , m_stream(stream)
    {}

    Action act() {
      if(m_inlineData.currBufferPtr == nullptr) {
        m_inlineData.currBufferPtr = m_this->m_data;
        m_inlineData.bytesLeft = m_this->m_position;
      }
      return writeExactSizeDataAsyncInline(m_stream.get(), m_inlineData, finish());
    }

  };

  return WriteDataCoroutine::start(_this, stream);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferInputStream

BufferInputStream::BufferInputStream(const std::shared_ptr<base::StrBuffer>& memoryHandle, p_char8 data, v_buff_size size)
  : m_memoryHandle(memoryHandle)
  , m_data(data)
  , m_size(size)
  , m_position(0)
  , m_ioMode(IOMode::NON_BLOCKING)
{}

BufferInputStream::BufferInputStream(const oatpp::String& data)
  : BufferInputStream(data.getPtr(), data->getData(), data->getSize())
{}

void BufferInputStream::reset(const std::shared_ptr<base::StrBuffer>& memoryHandle, p_char8 data, v_buff_size size) {
  m_memoryHandle = memoryHandle;
  m_data = data;
  m_size = size;
  m_position = 0;
}

void BufferInputStream::reset() {
  m_memoryHandle = nullptr;
  m_data = nullptr;
  m_size = 0;
  m_position = 0;
}

data::v_io_size BufferInputStream::read(void *data, v_buff_size count) {
  v_buff_size desiredAmount = count;
  if(desiredAmount > m_size - m_position) {
    desiredAmount = m_size - m_position;
  }
  std::memcpy(data, &m_data[m_position], desiredAmount);
  m_position += desiredAmount;
  return desiredAmount;
}

oatpp::async::Action BufferInputStream::suggestInputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  OATPP_LOGE("[oatpp::data::stream::BufferInputStream::suggestInputStreamAction()]", "Error. ioResult=%d", ioResult);

  const char* message =
    "Error. BufferInputStream::suggestOutputStreamAction() method is called with (ioResult <= 0).\n"
    "Conceptual error.";

  throw std::runtime_error(message);

}

void BufferInputStream::setInputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode BufferInputStream::getInputStreamIOMode() {
  return m_ioMode;
}

std::shared_ptr<base::StrBuffer> BufferInputStream::getDataMemoryHandle() {
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

}}}