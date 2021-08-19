/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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


#include "FIFOStream.hpp"
#include "oatpp/core/utils/Binary.hpp"

namespace oatpp { namespace data { namespace stream {

data::stream::DefaultInitializedContext FIFOInputStream::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_FINITE);

FIFOInputStream::FIFOInputStream(v_buff_size initialSize)
  : m_memoryHandle(std::make_shared<std::string>(initialSize, (char)0))
  , m_fifo(std::make_shared<data::buffer::FIFOBuffer>((void*)m_memoryHandle->data(), m_memoryHandle->size(), 0, 0, false))
  , m_maxCapacity(-1) {

}

void FIFOInputStream::reset() {
  m_fifo->setBufferPosition(0, 0, false);
}

v_io_size FIFOInputStream::read(void *data, v_buff_size count, async::Action& action) {
  (void) action;
  return m_fifo->read(data, count);
}

void FIFOInputStream::setInputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode FIFOInputStream::getInputStreamIOMode() {
  return m_ioMode;
}

Context& FIFOInputStream::getInputStreamContext() {
  return DEFAULT_CONTEXT;
}

std::shared_ptr<std::string> FIFOInputStream::getDataMemoryHandle() {
  return m_memoryHandle;
}

v_io_size FIFOInputStream::write(const void *data, v_buff_size count, async::Action &action) {
  (void) action;
  reserveBytesUpfront(count);
  return m_fifo->write(data, count);
}

void FIFOInputStream::reserveBytesUpfront(v_buff_size count) {

  v_buff_size capacityNeeded = availableToRead() + count;

  if(capacityNeeded > m_fifo->getBufferSize()) {

    v_buff_size newCapacity = utils::Binary::nextP2(capacityNeeded);

    if(newCapacity < 0 || (m_maxCapacity > 0 && newCapacity > m_maxCapacity)) {
      newCapacity = m_maxCapacity;
    }

    if(newCapacity < capacityNeeded) {
      throw std::runtime_error("[oatpp::data::stream::BufferOutputStream::reserveBytesUpfront()]: Error. Unable to allocate requested memory.");
    }

    // ToDo: In-Memory-Resize
    auto newHandle = std::make_shared<std::string>(newCapacity, (char)0);
    v_io_size oldSize = m_fifo->availableToRead();
    m_fifo->read((void*)newHandle->data(), oldSize);
    auto newFifo = std::make_shared<data::buffer::FIFOBuffer>((void*)newHandle->data(), newHandle->size(), 0, oldSize, oldSize > 0);
    m_memoryHandle = newHandle;
    m_fifo = newFifo;
  }

}

v_io_size FIFOInputStream::writeBufferToStream(stream::WriteCallback *writeCallback, v_buff_size count) {
  async::Action action;
  v_io_size size = m_fifo->readAndWriteToStream(writeCallback, count, action);
  if(!action.isNone()) {
    OATPP_LOGE("[oatpp::data::stream::FIFOInputStream::writeBufferToStream()]", "Error. writeSimple is called on a stream in Async mode.");
    throw std::runtime_error("[oatpp::data::stream::FIFOInputStream::writeBufferToStream()]: Error. writeSimple is called on a stream in Async mode.");
  }
  return size;
}

v_io_size FIFOInputStream::availableToWrite() {
  return m_fifo->availableToWrite();
}

v_io_size FIFOInputStream::peek(void *data, v_buff_size count, async::Action &action) {
  (void) action;
  return m_fifo->peek(data, count);
}

v_io_size FIFOInputStream::availableToRead() const {
  return m_fifo->availableToRead();
}

v_io_size FIFOInputStream::commitReadOffset(v_buff_size count) {
  return m_fifo->commitReadOffset(count);
}

}}}