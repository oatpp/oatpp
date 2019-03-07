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

#include <atomic>
#include <list>
#include <unordered_map>
#include <cstring>

namespace oatpp { namespace base { namespace  memory {
  
class MemoryPool {
public:
  static oatpp::concurrency::SpinLock::Atom POOLS_ATOM;
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
private:
  std::string m_name;
  v_int32 m_entrySize;
  v_int32 m_chunkSize;
  v_int64 m_id;
  std::list<p_char8> m_chunks;
  EntryHeader* m_rootEntry;
  oatpp::concurrency::SpinLock::Atom m_atom;
  v_int32 m_objectsCount;
public:
  
  MemoryPool(const std::string& name, v_int32 entrySize, v_int32 chunkSize)
    : m_name(name)
    , m_entrySize(entrySize)
    , m_chunkSize(chunkSize)
    , m_id(++poolIdCounter)
    , m_rootEntry(nullptr)
    , m_atom(false)
    , m_objectsCount(0)
  {
    allocChunk();
    oatpp::concurrency::SpinLock lock(POOLS_ATOM);
    POOLS[m_id] = this;
  }
  
  virtual ~MemoryPool() {
    auto it = m_chunks.begin();
    while (it != m_chunks.end()) {
      p_char8 chunk = *it;
      delete [] chunk;
      it++;
    }
    oatpp::concurrency::SpinLock lock(POOLS_ATOM);
    POOLS.erase(m_id);
  }
  
  void* obtain();
  void* obtainLockFree();
  
  void freeByEntryHeader(EntryHeader* entry);
  static void free(void* entry);
  
  std::string getName();
  v_int32 getEntrySize();
  v_int64 getSize();
  v_int32 getObjectsCount();
  
};
  
class ThreadDistributedMemoryPool {
private:
  v_int32 m_shardsCount;
  MemoryPool** m_shards;
public:
  static const v_int32 SHARDS_COUNT_DEFAULT;
public:
  ThreadDistributedMemoryPool(const std::string& name, v_int32 entrySize, v_int32 chunkSize,
                              v_int32 shardsCount = SHARDS_COUNT_DEFAULT);
  virtual ~ThreadDistributedMemoryPool();
  void* obtain();
};
  
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
    
    v_int32 newSize = m_size + m_growSize;
    T** newIndex = new T*[newSize];
    std::memcpy(newIndex, m_index, m_size);
    
    Block* b = new Block(new v_char8 [m_growSize * sizeof(T)], m_blocks);
    m_blocks = b;
    for(v_int32 i = 0; i < m_growSize; i++) {
      newIndex[m_size + i] = (T*) (&b->memory[i * sizeof(T)]);
    }
    
    delete [] m_index;
    m_size = newSize;
    m_index = newIndex;
    
  }
  
private:
  v_int32 m_growSize;
  v_int32 m_size;
  v_int32 m_indexPosition;
  Block* m_blocks;
  T** m_index;
public:
  
  Bench(v_int32 growSize)
    : m_growSize(growSize)
    , m_size(0)
    , m_indexPosition(0)
    , m_blocks(nullptr)
    , m_index(nullptr)
  {
    grow();
  }
  
  ~Bench(){
    auto curr = m_blocks;
    while (curr != nullptr) {
      auto next = curr->next;
      delete curr;
      curr = next;
    }
    delete [] m_index;
  }
  
  template<typename ... Args>
  T* obtain(Args... args) {
    if(m_indexPosition == m_size) {
      grow();
    }
    return new (m_index[m_indexPosition ++]) T(args...);
  }
  
  void free(T* entry) {
    entry->~T();
    m_index[--m_indexPosition] = entry;
  }
  
};
  
}}}

#endif /* oatpp_base_memory_MemoryPool_hpp */
