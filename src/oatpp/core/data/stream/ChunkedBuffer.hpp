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

#ifndef oatpp_data_stream_ChunkedBuffer_hpp
#define oatpp_data_stream_ChunkedBuffer_hpp

#include "Stream.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace data{ namespace stream {
  
class ChunkedBuffer : public oatpp::base::Controllable, public OutputStream {
public:
  static const char* ERROR_ASYNC_FAILED_TO_WRITE_ALL_DATA;
public:
  OBJECT_POOL(ChunkedBuffer_Pool, ChunkedBuffer, 32)
  SHARED_OBJECT_POOL(Shared_ChunkedBuffer_Pool, ChunkedBuffer, 32)
public:
  
  static const char* const CHUNK_POOL_NAME;
  
  static const os::io::Library::v_size CHUNK_ENTRY_SIZE_INDEX_SHIFT;
  static const os::io::Library::v_size CHUNK_ENTRY_SIZE;
  static const os::io::Library::v_size CHUNK_CHUNK_SIZE;

  static oatpp::base::memory::ThreadDistributedMemoryPool& getSegemntPool(){
    static oatpp::base::memory::ThreadDistributedMemoryPool pool(CHUNK_POOL_NAME,
                                                                 (v_int32) CHUNK_ENTRY_SIZE,
                                                                 (v_int32) CHUNK_CHUNK_SIZE);
    return pool;
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
  
  class Chunk : public oatpp::base::Controllable {
  public:
    OBJECT_POOL(ChunkedBuffer_Chunk_Pool, Chunk, 32)
    SHARED_OBJECT_POOL(Shared_ChunkedBuffer_Chunk_Pool, Chunk, 32)
  public:
    
    Chunk(void* pData, os::io::Library::v_size pSize)
      : data(pData)
      , size(pSize)
    {}
    
    static std::shared_ptr<Chunk> createShared(void* data, os::io::Library::v_size size){
      return Shared_ChunkedBuffer_Chunk_Pool::allocateShared(data, size);
    }
    
    const void* data;
    const os::io::Library::v_size size;
    
  };
  
public:
  typedef oatpp::collection::LinkedList<std::shared_ptr<Chunk>> Chunks;
private:
  
  os::io::Library::v_size m_size;
  os::io::Library::v_size m_chunkPos;
  ChunkEntry* m_firstEntry;
  ChunkEntry* m_lastEntry;
  
private:
  
  ChunkEntry* obtainNewEntry();
  void freeEntry(ChunkEntry* entry);
  
  os::io::Library::v_size writeToEntry(ChunkEntry* entry,
                                       const void *data,
                                       os::io::Library::v_size count,
                                       os::io::Library::v_size& outChunkPos);
  
  os::io::Library::v_size writeToEntryFrom(ChunkEntry* entry,
                                           os::io::Library::v_size inChunkPos,
                                           const void *data,
                                           os::io::Library::v_size count,
                                           os::io::Library::v_size& outChunkPos);
  
  ChunkEntry* getChunkForPosition(ChunkEntry* fromChunk,
                                      os::io::Library::v_size pos,
                                      os::io::Library::v_size& outChunkPos);
  
public:
  
  ChunkedBuffer()
    : m_size(0)
    , m_chunkPos(0)
    , m_firstEntry(nullptr)
    , m_lastEntry(nullptr)
  {}
  
  ~ChunkedBuffer() {
    clear();
  }
  
public:

  static std::shared_ptr<ChunkedBuffer> createShared(){
    return Shared_ChunkedBuffer_Pool::allocateShared();
  }

  os::io::Library::v_size write(const void *data, os::io::Library::v_size count) override;

  os::io::Library::v_size readSubstring(void *buffer,
                                        os::io::Library::v_size pos,
                                        os::io::Library::v_size count);

  /**
   * return substring of the data written to stream; NOT NULL
   */
  oatpp::String getSubstring(os::io::Library::v_size pos, os::io::Library::v_size count);

  /**
   * return data written to stream as oatpp::String; NOT NULL
   */
  oatpp::String toString() {
    return getSubstring(0, m_size);
  }

  bool flushToStream(const std::shared_ptr<OutputStream>& stream);
  oatpp::async::Action flushToStreamAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                           const oatpp::async::Action& actionOnFinish,
                                           const std::shared_ptr<OutputStream>& stream);
  
  std::shared_ptr<Chunks> getChunks();

  os::io::Library::v_size getSize();
  void clear();

};
  
}}}

#endif /* oatpp_data_stream_ChunkedBuffer_hpp */
