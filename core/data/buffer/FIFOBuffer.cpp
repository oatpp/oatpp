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

#include "FIFOBuffer.hpp"

namespace oatpp { namespace data{ namespace buffer {
  
os::io::Library::v_size FIFOBuffer::availableToRead() {
  oatpp::concurrency::SpinLock lock(m_atom);
  if(!m_canRead) {
    return 0;
  }
  if(m_readPosition < m_writePosition) {
    return m_writePosition - m_readPosition;
  }
  return (IOBuffer::BUFFER_SIZE - m_readPosition + m_writePosition);
}

os::io::Library::v_size FIFOBuffer::availableToWrite() {
  oatpp::concurrency::SpinLock lock(m_atom);
  if(m_canRead && m_writePosition == m_readPosition) {
    return 0;
  }
  if(m_writePosition < m_readPosition) {
    return m_readPosition - m_writePosition;
  }
  return (IOBuffer::BUFFER_SIZE - m_writePosition + m_readPosition);
}

os::io::Library::v_size FIFOBuffer::read(void *data, os::io::Library::v_size count) {
  
  oatpp::concurrency::SpinLock lock(m_atom);
  
  if(!m_canRead) {
    return 0;
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
    std::memcpy(data, &((p_char8)m_buffer.getData())[m_readPosition], size);
    m_readPosition += size;
    if(m_readPosition == m_writePosition) {
      m_canRead = false;
    }
    return size;
  }
  
  auto size = IOBuffer::BUFFER_SIZE - m_readPosition;
  
  if(size > count){
    std::memcpy(data, &((p_char8)m_buffer.getData())[m_readPosition], count);
    m_readPosition += count;
    return count;
  }
  
  std::memcpy(data, &((p_char8)m_buffer.getData())[m_readPosition], size);
  auto size2 = m_writePosition;
  if(size2 > count - size) {
    size2 = count - size;
  }
  
  std::memcpy(&((p_char8) data)[size], m_buffer.getData(), size2);
  m_readPosition = size2;
  if(m_readPosition == m_writePosition) {
    m_canRead = false;
  }
  
  return (size + size2);
  
}

os::io::Library::v_size FIFOBuffer::write(const void *data, os::io::Library::v_size count) {
  
  oatpp::concurrency::SpinLock lock(m_atom);
  
  if(m_canRead && m_writePosition == m_readPosition) {
    return 0;
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
    std::memcpy(&((p_char8)m_buffer.getData())[m_writePosition], data, size);
    m_writePosition += size;
    return size;
  }
  
  auto size = IOBuffer::BUFFER_SIZE - m_writePosition;
  
  if(size > count){
    std::memcpy(&((p_char8)m_buffer.getData())[m_writePosition], data, count);
    m_writePosition += count;
    return count;
  }
  
  std::memcpy(&((p_char8)m_buffer.getData())[m_writePosition], data, size);
  auto size2 = m_readPosition;
  if(size2 > count - size) {
    size2 = count - size;
  }
  
  std::memcpy(m_buffer.getData(), &((p_char8) data)[size], size2);
  m_writePosition = size2;
  
  return (size + size2);
  
}
  
}}}
