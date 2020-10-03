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

#ifndef oatpp_data_stream_ChunkedBuffer_hpp
#define oatpp_data_stream_ChunkedBuffer_hpp

#include "Stream.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace data{ namespace stream {

/**
 * Buffer wich can grow by chunks and implements &id:oatpp::data::stream::ConsistentOutputStream; interface.
 */
class ChunkedBuffer : public oatpp::base::Countable, public ConsistentOutputStream, public std::enable_shared_from_this<ChunkedBuffer> {
public:
  static data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
public:
  static const char* ERROR_ASYNC_FAILED_TO_WRITE_ALL_DATA;
public:
  OBJECT_POOL(ChunkedBuffer_Pool, ChunkedBuffer, 32)
  SHARED_OBJECT_POOL(Shared_ChunkedBuffer_Pool, ChunkedBuffer, 32)
public:
  
  static const char* const CHUNK_POOL_NAME;
  
  static const v_buff_size CHUNK_ENTRY_SIZE_INDEX_SHIFT;
  static const v_buff_size CHUNK_ENTRY_SIZE;
  static const v_buff_size CHUNK_CHUNK_SIZE;

  static oatpp::base::memory::ThreadDistributedMemoryPool& getSegemntPool() {
    static auto pool = new oatpp::base::memory::ThreadDistributedMemoryPool(CHUNK_POOL_NAME, CHUNK_ENTRY_SIZE, CHUNK_CHUNK_SIZE);
    return *pool;
  }
  
private:
  
  class ChunkEntry {
  public:
    OBJECT_POOL(ChunkedBuffer_ChunkEntry_Pool, ChunkEntry, 32)
  public:
  
    ChunkEntry(void* pChunk, ChunkEntry* pNext)
      : chunk(pChunk)
      , next(pNext)
    {}
    
    ~ChunkEntry(){
    }
    
    void* chunk;
    ChunkEntry* next;
    
  };
  
public:
  
  class Chunk : public oatpp::base::Countable {
  public:
    OBJECT_POOL(ChunkedBuffer_Chunk_Pool, Chunk, 32)
    SHARED_OBJECT_POOL(Shared_ChunkedBuffer_Chunk_Pool, Chunk, 32)
  public:
    
    Chunk(void* pData, v_buff_size pSize)
      : data(pData)
      , size(pSize)
    {}
    
    static std::shared_ptr<Chunk> createShared(void* data, v_buff_size size){
      return Shared_ChunkedBuffer_Chunk_Pool::allocateShared(data, size);
    }
    
    const void* data;
    const v_buff_size size;
    
  };
  
public:
  typedef oatpp::collection::LinkedList<std::shared_ptr<Chunk>> Chunks;
private:

  v_buff_size m_size;
  v_buff_size m_chunkPos;
  ChunkEntry* m_firstEntry;
  ChunkEntry* m_lastEntry;
  IOMode m_ioMode;
  
private:
  
  ChunkEntry* obtainNewEntry();
  void freeEntry(ChunkEntry* entry);
  
  v_io_size writeToEntry(ChunkEntry* entry,
                               const void *data,
                               v_buff_size count,
                               v_buff_size& outChunkPos);
  
  v_io_size writeToEntryFrom(ChunkEntry* entry,
                                   v_buff_size inChunkPos,
                                   const void *data,
                                   v_buff_size count,
                                   v_buff_size& outChunkPos);
  
  ChunkEntry* getChunkForPosition(ChunkEntry* fromChunk,
                                  v_buff_size pos,
                                  v_buff_size& outChunkPos);
  
public:

  /**
   * Constructor.
   */
  ChunkedBuffer();

  /**
   * Virtual Destructor.
   */
  ~ChunkedBuffer();

public:

  /**
   * Deleted copy constructor.
   */
  ChunkedBuffer(const ChunkedBuffer&) = delete;

  ChunkedBuffer& operator=(const ChunkedBuffer&) = delete;
  
public:

  /**
   * Create shared ChunkedBuffer.
   * @return `std::shared_ptr` to ChunkedBuffer.
   */
  static std::shared_ptr<ChunkedBuffer> createShared(){
    return Shared_ChunkedBuffer_Pool::allocateShared();
  }

  /**
   * Write data to ChunkedBuffer. Implementation of &id:oatpp::data::stream::OutputStream::write; method.
   * @param data - data to write.
   * @param count - size of data in bytes.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written.
   */
  v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

  /**
   * Set stream I/O mode.
   * @param ioMode
   */
  void setOutputStreamIOMode(IOMode ioMode) override;

  /**
   * Set stream I/O mode.
   * @return
   */
  IOMode getOutputStreamIOMode() override;

  /**
   * Get stream context.
   * @return - &id:oatpp::data::stream::Context;.
   */
  Context& getOutputStreamContext() override;

  /**
   * Read part of ChunkedBuffer to buffer.
   * @param buffer - buffer to write data to.
   * @param pos - starting position in ChunkedBuffer to read data from.
   * @param count - number of bytes to read.
   * @return - actual number of bytes read from ChunkedBuffer and written to buffer.
   */
  v_io_size readSubstring(void *buffer, v_buff_size pos, v_buff_size count);

  /**
   * Create &id:oatpp::String; from part of ChunkedBuffer.
   * @param pos - starting position in ChunkedBuffer.
   * @param count - size of bytes to write to substring.
   * @return - &id:oatpp::String;
   */
  oatpp::String getSubstring(v_buff_size pos, v_buff_size count);

  /**
   * Create &id:oatpp::String; from all data in ChunkedBuffer.
   * @return - &id:oatpp::String;
   */
  oatpp::String toString() {
    return getSubstring(0, m_size);
  }

  /**
   * Write all data from ChunkedBuffer to &id:oatpp::data::stream::OutputStream;.
   * ChunkedBuffer will not be cleared during this call!
   * @param stream - &id:oatpp::data::stream::OutputStream; stream to write all data to.
   * @return - `true` if no errors occured. **will be refactored to return actual amount of bytes flushed**.
   */
  bool flushToStream(OutputStream* stream);

  /**
   * Write all data from ChunkedBuffer to &id:oatpp::data::stream::OutputStream; in asynchronous manner.
   * @param stream - &id:oatpp::data::stream::OutputStream; stream to write all data to.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter flushToStreamAsync(const std::shared_ptr<OutputStream>& stream);
  
  std::shared_ptr<Chunks> getChunks();

  /**
   * Get number of bytes written to ChunkedBuffer.
   * @return - number of bytes written to ChunkedBuffer.
   */
  v_buff_size getSize();

  /**
   * Clear data in ChunkedBuffer.
   */
  void clear();

};
  
}}}

#endif /* oatpp_data_stream_ChunkedBuffer_hpp */
