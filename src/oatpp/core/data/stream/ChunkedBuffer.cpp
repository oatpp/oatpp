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

#include "ChunkedBuffer.hpp"

namespace oatpp { namespace data{ namespace stream {

data::stream::DefaultInitializedContext ChunkedBuffer::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_INFINITE);

const char* ChunkedBuffer::ERROR_ASYNC_FAILED_TO_WRITE_ALL_DATA = "ERROR_ASYNC_FAILED_TO_WRITE_ALL_DATA";

const char* const ChunkedBuffer::CHUNK_POOL_NAME = "ChunkedBuffer_Chunk_Pool";

const v_buff_size ChunkedBuffer::CHUNK_ENTRY_SIZE_INDEX_SHIFT = 11;
const v_buff_size ChunkedBuffer::CHUNK_ENTRY_SIZE =
                                              (1 << ChunkedBuffer::CHUNK_ENTRY_SIZE_INDEX_SHIFT);
const v_buff_size ChunkedBuffer::CHUNK_CHUNK_SIZE = 32;

ChunkedBuffer::ChunkedBuffer()
  : m_size(0)
  , m_chunkPos(0)
  , m_firstEntry(nullptr)
  , m_lastEntry(nullptr)
  , m_ioMode(IOMode::ASYNCHRONOUS)
{}

ChunkedBuffer::~ChunkedBuffer() {
  clear();
}

ChunkedBuffer::ChunkEntry* ChunkedBuffer::obtainNewEntry(){
  auto result = new ChunkEntry(getSegemntPool().obtain(), nullptr);
  if(m_firstEntry == nullptr) {
    m_firstEntry = result;
  } else {
    m_lastEntry->next = result;
  }
  m_lastEntry = result;
  return result;
}

void ChunkedBuffer::freeEntry(ChunkEntry* entry){
  oatpp::base::memory::MemoryPool::free(entry->chunk);
  delete entry;
}

v_io_size ChunkedBuffer::writeToEntry(ChunkEntry* entry,
                                            const void *data,
                                            v_buff_size count,
                                            v_buff_size& outChunkPos)
{
  if(count >= CHUNK_ENTRY_SIZE){
    std::memcpy(entry->chunk, data, CHUNK_ENTRY_SIZE);
    outChunkPos = 0;
    return CHUNK_ENTRY_SIZE;
  } else {
    std::memcpy(entry->chunk, data, (size_t)count);
    outChunkPos = count;
    return count;
  }
}

v_io_size ChunkedBuffer::writeToEntryFrom(ChunkEntry* entry,
                                                v_buff_size inChunkPos,
                                                const void *data,
                                                v_buff_size count,
                                                v_buff_size& outChunkPos)
{
  v_io_size spaceLeft = CHUNK_ENTRY_SIZE - inChunkPos;
  if(count >= spaceLeft){
    std::memcpy(&((p_char8) entry->chunk)[inChunkPos], data, (size_t)spaceLeft);
    outChunkPos = 0;
    return spaceLeft;
  } else {
    std::memcpy(&((p_char8) entry->chunk)[inChunkPos], data, (size_t)count);
    outChunkPos = inChunkPos + count;
    return count;
  }
}

ChunkedBuffer::ChunkEntry* ChunkedBuffer::getChunkForPosition(ChunkEntry* fromChunk,
                                                              v_buff_size pos,
                                                              v_buff_size& outChunkPos)
{

  v_buff_size segIndex = pos >> CHUNK_ENTRY_SIZE_INDEX_SHIFT;
  outChunkPos = pos - (segIndex << CHUNK_ENTRY_SIZE_INDEX_SHIFT);

  auto curr = fromChunk;

  for(v_buff_size i = 0; i < segIndex; i++){
    curr = curr->next;
  }

  return curr;

}

v_io_size ChunkedBuffer::write(const void *data, v_buff_size count, async::Action& action){

  (void) action;

  if(count <= 0){
    return 0;
  }

  if(m_lastEntry == nullptr){
    obtainNewEntry();
  }

  ChunkEntry* entry = m_lastEntry;
  v_buff_size pos = 0;

  pos += writeToEntryFrom(entry, m_chunkPos, data, count, m_chunkPos);

  if(m_chunkPos == 0){
    entry = obtainNewEntry();
  }

  while (pos < count) {

    pos += writeToEntry(entry, &((p_char8) data)[pos], count - pos, m_chunkPos);

    if(m_chunkPos == 0){
      entry = obtainNewEntry();
    }
  }

  m_size += pos; // pos == count
  return count;

}

void ChunkedBuffer::setOutputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode ChunkedBuffer::getOutputStreamIOMode() {
  return m_ioMode;
}

Context& ChunkedBuffer::getOutputStreamContext() {
  return DEFAULT_CONTEXT;
}

v_io_size ChunkedBuffer::readSubstring(void *buffer,
                                       v_buff_size pos,
                                       v_buff_size count)
{

  if(pos < 0 || pos >= m_size){
    return 0;
  }

  v_buff_size countToRead;
  if(pos + count > m_size){
    countToRead = m_size - pos;
  } else {
    countToRead = count;
  }

  v_buff_size firstChunkPos;
  auto firstChunk = getChunkForPosition(m_firstEntry, pos, firstChunkPos);

  v_buff_size lastChunkPos;
  auto lastChunk = getChunkForPosition(firstChunk, firstChunkPos + countToRead, lastChunkPos);

  v_io_size bufferPos = 0;

  if(firstChunk != lastChunk){

    v_buff_size countToCopy = CHUNK_ENTRY_SIZE - firstChunkPos;
    std::memcpy(buffer, &((p_char8)firstChunk->chunk)[firstChunkPos], (size_t)countToCopy);
    bufferPos += countToCopy;

    auto curr = firstChunk->next;

    while (curr != lastChunk) {
      std::memcpy(&((p_char8)buffer)[bufferPos], curr->chunk, CHUNK_ENTRY_SIZE);
      bufferPos += CHUNK_ENTRY_SIZE;
      curr = curr->next;
    }

    std::memcpy(&((p_char8)buffer)[bufferPos], lastChunk->chunk, (size_t)lastChunkPos);

  } else {
    v_buff_size countToCopy = lastChunkPos - firstChunkPos;
    std::memcpy(buffer, &((p_char8)firstChunk->chunk)[firstChunkPos], (size_t)countToCopy);
  }

  return countToRead;

}

oatpp::String ChunkedBuffer::getSubstring(v_buff_size pos, v_buff_size count){
  auto str = oatpp::String((v_int32) count);
  readSubstring((p_char8)str->data(), pos, count);
  return str;
}

v_io_size ChunkedBuffer::flushBufferToStream(stream::WriteCallback *writeCallback) {
  return writeBufferToStream(writeCallback, m_size, 0);
}

v_io_size ChunkedBuffer::writeBufferToStream(stream::WriteCallback *writeCallback, v_buff_size count, v_buff_size readOffset) {
  if(readOffset < 0 || readOffset >= m_size){
    return 0;
  }

  v_buff_size countToRead;
  if(readOffset + count > m_size){
    countToRead = m_size - readOffset;
  } else {
    countToRead = count;
  }

  v_buff_size firstChunkPos;
  auto firstChunk = getChunkForPosition(m_firstEntry, readOffset, firstChunkPos);

  v_buff_size lastChunkPos;
  auto lastChunk = getChunkForPosition(firstChunk, firstChunkPos + countToRead, lastChunkPos);

  v_io_size bufferPos = 0;

  if(firstChunk != lastChunk){

    v_buff_size countToCopy = CHUNK_ENTRY_SIZE - firstChunkPos;
    writeCallback->writeSimple(&((p_char8)firstChunk->chunk)[firstChunkPos], countToCopy);
    bufferPos += countToCopy;

    auto curr = firstChunk->next;

    while (curr != lastChunk) {

      writeCallback->writeSimple(curr->chunk, CHUNK_ENTRY_SIZE);
      bufferPos += CHUNK_ENTRY_SIZE;
      curr = curr->next;
    }

    writeCallback->writeSimple(lastChunk->chunk, lastChunkPos);

  } else {
    v_buff_size countToCopy = lastChunkPos - firstChunkPos;
    writeCallback->writeSimple(&((p_char8)firstChunk->chunk)[firstChunkPos], countToCopy);
  }

  return countToRead;
}

async::CoroutineStarter ChunkedBuffer::flushBufferToStreamAsync(const std::shared_ptr<data::stream::WriteCallback> &stream) {
  return writeBufferToStreamAsync(stream, m_size, 0);
}

async::CoroutineStarter ChunkedBuffer::writeBufferToStreamAsync(const std::shared_ptr<data::stream::WriteCallback> &stream,
                                                                v_buff_size count, v_buff_size readOffset) {
  class FlushCoroutine : public oatpp::async::Coroutine<FlushCoroutine> {
   private:
    std::shared_ptr<ChunkedBuffer> m_chunkedBuffer;
    std::shared_ptr<WriteCallback> m_stream;
    ChunkEntry* m_currEntry;
    v_io_size m_bytesLeft;
    Action m_nextAction;
    data::buffer::InlineWriteData m_currData;
    bool m_needInit;
    v_buff_size m_count;
    v_buff_size m_readOffset;
   public:

    FlushCoroutine(const std::shared_ptr<ChunkedBuffer>& chunkedBuffer,
                   const std::shared_ptr<WriteCallback>& stream,
                   v_buff_size count, v_buff_size readOffset)
        : m_chunkedBuffer(chunkedBuffer)
        , m_stream(stream)
        , m_currEntry(nullptr)
        , m_bytesLeft(0)
        , m_nextAction(Action::createActionByType(Action::TYPE_FINISH))
        , m_needInit(true)
        , m_count(count)
    {}

    Action act() override {

      if (m_needInit) {
        m_needInit = false;
        m_currEntry = m_chunkedBuffer->m_firstEntry;
        m_bytesLeft = std::min(m_count, m_chunkedBuffer->m_size);
      }

      while (m_readOffset > 0 && m_bytesLeft > 0) {
        if (m_readOffset > CHUNK_ENTRY_SIZE) {
          m_readOffset -= CHUNK_ENTRY_SIZE;
          m_bytesLeft -= CHUNK_ENTRY_SIZE;
          m_currEntry = m_currEntry->next;
          if(m_currEntry == nullptr) {
            return finish();
          }
        } else {
          if (m_bytesLeft > 0) {
            m_currData.set(((p_uint8)m_currEntry->chunk) + m_readOffset, CHUNK_ENTRY_SIZE - m_readOffset);
            m_readOffset = 0;
            m_currEntry = m_currEntry->next;
            m_bytesLeft -= m_currData.bytesLeft;
            m_nextAction = yieldTo(&FlushCoroutine::act);
            return yieldTo(&FlushCoroutine::writeCurrData);
          }
        }
      }

      if(m_currEntry == nullptr) {
        return finish();
      }

      if(m_bytesLeft > CHUNK_ENTRY_SIZE) {
        m_currData.set(m_currEntry->chunk, CHUNK_ENTRY_SIZE);
        m_nextAction = yieldTo(&FlushCoroutine::act);
        m_currEntry = m_currEntry->next;
        m_bytesLeft -= m_currData.bytesLeft;
        return yieldTo(&FlushCoroutine::writeCurrData);
      } else {
        m_currData.set(m_currEntry->chunk, m_bytesLeft);
        m_nextAction = yieldTo(&FlushCoroutine::act);
        m_currEntry = m_currEntry->next;
        m_bytesLeft -= m_currData.bytesLeft;
        return yieldTo(&FlushCoroutine::writeCurrData);
      }

    }

    Action writeCurrData() {
      return m_stream->writeExactSizeDataAsyncInline(m_currData, Action::clone(m_nextAction));
    }

  };

  return FlushCoroutine::start(shared_from_this(), stream, count, readOffset);
}

v_io_size ChunkedBuffer::writeBufferToStream(stream::WriteCallback *writeCallback, v_buff_size count) {
  return BufferedOutputStream::writeBufferToStream(writeCallback, count);
}

async::CoroutineStarter ChunkedBuffer::writeBufferToStreamAsync(const std::shared_ptr<data::stream::WriteCallback> &writeCallback,
                                                                v_buff_size count) {
  return BufferedOutputStream::writeBufferToStreamAsync(writeCallback, count);
}

std::shared_ptr<ChunkedBuffer::Chunks> ChunkedBuffer::getChunks() {
  auto chunks = std::make_shared<Chunks>();
  auto curr = m_firstEntry;
  v_int32 count = 0;
  while (curr != nullptr) {
    chunks->push_back(Chunk::createShared(curr->chunk, curr->next
                                            ? CHUNK_ENTRY_SIZE
                                            : m_size - CHUNK_ENTRY_SIZE * count));
    ++count;
    curr = curr->next;
  }
  return chunks;
}

v_buff_size ChunkedBuffer::getSize(){
  return m_size;
}

void ChunkedBuffer::clear(){

  ChunkEntry* curr = m_firstEntry;
  while (curr != nullptr) {
    ChunkEntry* next = curr->next;
    freeEntry(curr);
    curr = next;
  }

  m_size = 0;
  m_chunkPos = 0;
  m_firstEntry = nullptr;
  m_lastEntry = nullptr;

}

}}}
