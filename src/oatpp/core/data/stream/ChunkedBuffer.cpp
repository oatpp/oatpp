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
  
const char* ChunkedBuffer::ERROR_ASYNC_FAILED_TO_WRITE_ALL_DATA = "ERROR_ASYNC_FAILED_TO_WRITE_ALL_DATA";
  
const char* const ChunkedBuffer::CHUNK_POOL_NAME = "ChunkedBuffer_Chunk_Pool";

const data::v_io_size ChunkedBuffer::CHUNK_ENTRY_SIZE_INDEX_SHIFT = 11;
const data::v_io_size ChunkedBuffer::CHUNK_ENTRY_SIZE =
                                              (1 << ChunkedBuffer::CHUNK_ENTRY_SIZE_INDEX_SHIFT);
const data::v_io_size ChunkedBuffer::CHUNK_CHUNK_SIZE = 32;

ChunkedBuffer::ChunkedBuffer()
  : m_size(0)
  , m_chunkPos(0)
  , m_firstEntry(nullptr)
  , m_lastEntry(nullptr)
  , m_ioMode(IOMode::NON_BLOCKING)
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
  
data::v_io_size ChunkedBuffer::writeToEntry(ChunkEntry* entry,
                                                      const void *data,
                                                      data::v_io_size count,
                                                      data::v_io_size& outChunkPos) {
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
  
data::v_io_size ChunkedBuffer::writeToEntryFrom(ChunkEntry* entry,
                                                          data::v_io_size inChunkPos,
                                                          const void *data,
                                                          data::v_io_size count,
                                                          data::v_io_size& outChunkPos) {
  data::v_io_size spaceLeft = CHUNK_ENTRY_SIZE - inChunkPos;
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
                                                                      data::v_io_size pos,
                                                                      data::v_io_size& outChunkPos) {
  
  data::v_io_size segIndex = pos >> CHUNK_ENTRY_SIZE_INDEX_SHIFT;
  outChunkPos = pos - (segIndex << CHUNK_ENTRY_SIZE_INDEX_SHIFT);
  
  auto curr = fromChunk;
  
  for(data::v_io_size i = 0; i < segIndex; i++){
    curr = curr->next;
  }
  
  return curr;
  
}
  
data::v_io_size ChunkedBuffer::write(const void *data, data::v_io_size count){
  
  if(count <= 0){
    return 0;
  }
  
  if(m_lastEntry == nullptr){
    obtainNewEntry();
  }
  
  ChunkEntry* entry = m_lastEntry;
  data::v_io_size pos = 0;
  
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
  
data::v_io_size ChunkedBuffer::readSubstring(void *buffer,
                                             data::v_io_size pos,
                                             data::v_io_size count)
{
  
  if(pos < 0 || pos >= m_size){
    return 0;
  }
  
  data::v_io_size countToRead;
  if(pos + count > m_size){
    countToRead = m_size - pos;
  } else {
    countToRead = count;
  }
  
  data::v_io_size firstChunkPos;
  auto firstChunk = getChunkForPosition(m_firstEntry, pos, firstChunkPos);
  
  data::v_io_size lastChunkPos;
  auto lastChunk = getChunkForPosition(firstChunk, firstChunkPos + countToRead, lastChunkPos);
  
  data::v_io_size bufferPos = 0;
  
  if(firstChunk != lastChunk){
    
    data::v_io_size countToCopy = CHUNK_ENTRY_SIZE - firstChunkPos;
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
    data::v_io_size countToCopy = lastChunkPos - firstChunkPos;
    std::memcpy(buffer, &((p_char8)firstChunk->chunk)[firstChunkPos], (size_t)countToCopy);
  }
  
  return countToRead;
  
}
  
oatpp::String ChunkedBuffer::getSubstring(data::v_io_size pos,
                                                        data::v_io_size count){
  auto str = oatpp::String((v_int32) count);
  readSubstring(str->getData(), pos, count);
  return str;
}

bool ChunkedBuffer::flushToStream(OutputStream* stream){
  data::v_io_size pos = m_size;
  auto curr = m_firstEntry;
  while (pos > 0) {
    if(pos > CHUNK_ENTRY_SIZE) {
      auto res = data::stream::writeExactSizeData(stream, curr->chunk, CHUNK_ENTRY_SIZE);
      if(res != CHUNK_ENTRY_SIZE) {
        return false;
      }
      pos -= res;
    } else {
      auto res = data::stream::writeExactSizeData(stream, curr->chunk, pos);
      if(res != pos) {
        return false;
      }
      pos -= res;
    }
    curr = curr->next;
  }
  return true;
}

oatpp::async::CoroutineStarter ChunkedBuffer::flushToStreamAsync(const std::shared_ptr<OutputStream>& stream) {
 
  class FlushCoroutine : public oatpp::async::Coroutine<FlushCoroutine> {
  private:
    std::shared_ptr<ChunkedBuffer> m_chunkedBuffer;
    std::shared_ptr<OutputStream> m_stream;
    ChunkEntry* m_currEntry;
    data::v_io_size m_bytesLeft;
    Action m_nextAction;
    data::stream::AsyncInlineWriteData m_currData;
  public:
    
    FlushCoroutine(const std::shared_ptr<ChunkedBuffer>& chunkedBuffer,
                   const std::shared_ptr<OutputStream>& stream)
      : m_chunkedBuffer(chunkedBuffer)
      , m_stream(stream)
      , m_currEntry(chunkedBuffer->m_firstEntry)
      , m_bytesLeft(chunkedBuffer->m_size)
      , m_nextAction(Action::createActionByType(Action::TYPE_FINISH))
    {}
    
    Action act() override {
      
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
      return oatpp::data::stream::writeExactSizeDataAsyncInline(this, m_stream.get(), m_currData, Action::clone(m_nextAction));
    }
    
  };
  
  return FlushCoroutine::start(shared_from_this(), stream);
  
}
  
std::shared_ptr<ChunkedBuffer::Chunks> ChunkedBuffer::getChunks() {
  auto chunks = Chunks::createShared();
  auto curr = m_firstEntry;
  v_int32 count = 0;
  while (curr != nullptr) {
    if(curr->next != nullptr){
      chunks->pushBack(Chunk::createShared(curr->chunk, CHUNK_ENTRY_SIZE));
    } else {
      chunks->pushBack(Chunk::createShared(curr->chunk, m_size - CHUNK_ENTRY_SIZE * count));
    }
    ++count;
    curr = curr->next;
  }
  return chunks;
}

data::v_io_size ChunkedBuffer::getSize(){
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
