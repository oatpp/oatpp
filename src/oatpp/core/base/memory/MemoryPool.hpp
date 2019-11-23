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

#ifndef oatpp_base_memory_MemoryPool_hpp
#define oatpp_base_memory_MemoryPool_hpp

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <list>
#include <unordered_map>
#include <cstring>

namespace oatpp { namespace base { namespace  memory {

/**
 * Memory Pool allocates memory chunks. Each chunk consists of specified number of fixed-size entries.
 * Entries can be obtained and freed by user. When memory pool runs out of free entries, new chunk is allocated.
 */
class MemoryPool {
public:
  static oatpp::concurrency::SpinLock POOLS_SPIN_LOCK;
  static std::unordered_map<v_int64, MemoryPool*> POOLS;
private:
  static std::atomic<v_int64> poolIdCounter;
private:
  
  class EntryHeader {
  public:
    
    EntryHeader(MemoryPool* pPool, v_int64 pPoolId, EntryHeader* pNext)
      : pool(pPool)
      , poolId(pPoolId)
      , next(pNext)
    {}
    
    MemoryPool* pool;
    v_int64 poolId;
    EntryHeader* next;
    
  };
  
private:
  void allocChunk();
  void freeByEntryHeader(EntryHeader* entry);
private:
  std::string m_name;
  v_buff_size m_entrySize;
  v_buff_size m_chunkSize;
  v_int64 m_id;
  std::list<p_char8> m_chunks;
  EntryHeader* m_rootEntry;
  v_int64 m_objectsCount;
  oatpp::concurrency::SpinLock m_lock;
public:

  /**
   * Constructor.
   * @param name - name of the pool.
   * @param entrySize - size of the entry in bytes returned in call to &l:MemoryPool::obtain ();.
   * @param chunkSize - number of entries in one chunk.
   */
  MemoryPool(const std::string& name, v_buff_size entrySize, v_buff_size chunkSize);

  /**
   * Deleted copy-constructor.
   */
  MemoryPool(const MemoryPool&) = delete;

  /**
   * Virtual destructor.
   */
  virtual ~MemoryPool();

  /**
   * Obtain pointer to memory entry.
   * When entry is no more needed, user must call &id:oatpp::base::memory::MemoryPool::free; and pass obtained entry pointer as a parameter.
   * @return - pointer to memory entry.
   */
  void* obtain();

  /**
   * Free obtained earlier memory entry.
   * This method is static, because entry "knows" to what pool it belongs.
   * @param entry - entry obtained by call to &l:MemoryPool::obtain ();
   */
  static void free(void* entry);

  /**
   * Get name of the memory pool.
   * @return - memory pool name as `std::string`.
   */
  std::string getName();

  /**
   * Get size of the memory entry in bytes which can be obtained by call to &l:MemoryPool::obtain ();.
   * @return - size of the enrty in bytes.
   */
  v_buff_size getEntrySize();

  /**
   * Get size of the memory allocated by memory pool.
   * @return - size of the memory allocated by memory pool.
   */
  v_buff_size getSize();

  /**
   * Get number of entries currently in use.
   * @return - number of entries currently in use.
   */
  v_int64 getObjectsCount();
  
};

/**
 * Creates multiple MemoryPools (&l:MemoryPool;) to reduce concurrency blocking in call to &l:ThreadDistributedMemoryPool::obtain ();
 */
class ThreadDistributedMemoryPool {
private:
  v_int64 m_shardsCount;
  MemoryPool** m_shards;
  bool m_deleted;
public:

  /**
   * Default number of MemoryPools (&l:MemoryPool;) "shards" to create.
   */
  static const v_int64 SHARDS_COUNT_DEFAULT;
public:

  /**
   * Constructor.
   * @param name - name of the memory pool.
   * @param entrySize - size of memory pool entry.
   * @param chunkSize - number of entries in chunk.
   * @param shardsCount - number of MemoryPools (&l:MemoryPool;) "shards" to create.
   */
  ThreadDistributedMemoryPool(const std::string& name, v_buff_size entrySize, v_buff_size chunkSize,
                              v_int64 shardsCount = SHARDS_COUNT_DEFAULT);

  /**
   * Deleted copy-constructor.
   */
  ThreadDistributedMemoryPool(const ThreadDistributedMemoryPool&) = delete;

  virtual ~ThreadDistributedMemoryPool();

  /**
   * Obtain pointer to memory entry.
   * When entry is no more needed, user must call &id:oatpp::base::memory::MemoryPool::free; and pass obtained entry pointer as a parameter.
   * @return - pointer to memory entry.
   */
  void* obtain();

};

/**
 * Not thread-safe pool of objects of specified type.
 * @tparam T - object type.
 */
template<typename T>
class Bench {
private:
  
  class Block {
  public:
    Block(p_char8 mem, Block* pNext)
      : memory(mem)
      , next(pNext)
    {}
    p_char8 memory;
    Block* next;
  };
  
private:
  
  void grow(){
    
    v_buff_size newSize = m_size + m_growSize;
    T** newIndex = new T*[newSize];
    std::memcpy(newIndex, m_index, m_size);
    
    Block* b = new Block(new v_char8 [m_growSize * sizeof(T)], m_blocks);
    m_blocks = b;
    for(v_buff_size i = 0; i < m_growSize; i++) {
      newIndex[m_size + i] = (T*) (&b->memory[i * sizeof(T)]);
    }
    
    delete [] m_index;
    m_size = newSize;
    m_index = newIndex;
    
  }
  
private:
  v_buff_size m_growSize;
  v_buff_size m_size;
  v_buff_size m_indexPosition;
  Block* m_blocks;
  T** m_index;
public:

  /**
   * Constructor.
   * @param growSize - number of objects to allocate when no free objects left.
   */
  Bench(v_buff_size growSize)
    : m_growSize(growSize)
    , m_size(0)
    , m_indexPosition(0)
    , m_blocks(nullptr)
    , m_index(nullptr)
  {
    grow();
  }

  /**
   * Non virtual destructor.
   */
  ~Bench(){
    auto curr = m_blocks;
    while (curr != nullptr) {
      auto next = curr->next;
      delete curr;
      curr = next;
    }
    delete [] m_index;
  }

  /**
   * Construct object and get pointer to constructed object.
   * @tparam Args - arguments to be passed to object constructor.
   * @param args - actual arguments to pass to object constructor.
   * @return - pointer to constructed object.
   */
  template<typename ... Args>
  T* obtain(Args... args) {
    if(m_indexPosition == m_size) {
      grow();
    }
    return new (m_index[m_indexPosition ++]) T(args...);
  }

  /**
   * Call object destructor and put it on "bench".
   * @param entry - object to be freed.
   */
  void free(T* entry) {
    entry->~T();
    m_index[--m_indexPosition] = entry;
  }
  
};
  
}}}

#endif /* oatpp_base_memory_MemoryPool_hpp */
