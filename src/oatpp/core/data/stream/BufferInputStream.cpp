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

#include "BufferInputStream.hpp"

namespace oatpp { namespace data{ namespace stream {

BufferInputStream::BufferInputStream(const std::shared_ptr<base::StrBuffer>& memoryHandle, p_char8 data, v_io_size size)
  : m_memoryHandle(memoryHandle)
  , m_data(data)
  , m_size(size)
  , m_position(0)
  , m_ioMode(IOMode::NON_BLOCKING)
{}

BufferInputStream::BufferInputStream(const oatpp::String& data)
  : BufferInputStream(data.getPtr(), data->getData(), data->getSize())
{}

void BufferInputStream::reset(const std::shared_ptr<base::StrBuffer>& memoryHandle, p_char8 data, v_io_size size) {
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

data::v_io_size BufferInputStream::read(void *data, data::v_io_size count) {
  data::v_io_size desiredAmount = count;
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

v_io_size BufferInputStream::getDataSize() {
  return m_size;
}

v_io_size BufferInputStream::getCurrentPosition() {
  return m_position;
}

void BufferInputStream::setCurrentPosition(v_io_size position) {
  m_position = position;
}

}}}