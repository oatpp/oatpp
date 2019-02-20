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

#include "MemoryPool.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

namespace oatpp { namespace base { namespace  memory {

void MemoryPool::allocChunk() {
  v_int32 entryBlockSize = sizeof(EntryHeader) + m_entrySize;
  v_int32 chunkMemSize = entryBlockSize * m_chunkSize;
  p_char8 mem = new v_char8[chunkMemSize];
  m_chunks.push_back(mem);
  for(v_int32 i = 0; i < m_chunkSize; i++){
    EntryHeader* entry = new (mem + i * entryBlockSize) EntryHeader(this, m_id, m_rootEntry);
    m_rootEntry = entry;
  }
}
  
void* MemoryPool::obtain() {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
  return new v_char8[m_entrySize];
#else
  oatpp::concurrency::SpinLock lock(m_atom);
  if(m_rootEntry != nullptr) {
    auto entry = m_rootEntry;
    m_rootEntry = m_rootEntry->next;
    ++ m_objectsCount;
    return ((p_char8) entry) + sizeof(EntryHeader);
  } else {
    allocChunk();
    if(m_rootEntry == nullptr) {
      throw std::runtime_error("[oatpp::base::memory::MemoryPool:obtain()]: Unable to allocate entry");
    }
    auto entry = m_rootEntry;
    m_rootEntry = m_rootEntry->next;
    ++ m_objectsCount;
    return ((p_char8) entry) + sizeof(EntryHeader);
  }
#endif
}
  
void* MemoryPool::obtainLockFree() {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
  return new v_char8[m_entrySize];
#else
  if(m_rootEntry != nullptr) {
    auto entry = m_rootEntry;
    m_rootEntry = m_rootEntry->next;
    ++ m_objectsCount;
    return ((p_char8) entry) + sizeof(EntryHeader);
  } else {
    allocChunk();
    if(m_rootEntry == nullptr) {
      throw std::runtime_error("[oatpp::base::memory::MemoryPool:obtainLockFree()]: Unable to allocate entry");
    }
    auto entry = m_rootEntry;
    m_rootEntry = m_rootEntry->next;
    ++ m_objectsCount;
    return ((p_char8) entry) + sizeof(EntryHeader);
  }
#endif
}

void MemoryPool::freeByEntryHeader(EntryHeader* entry) {
  if(entry->poolId == m_id) {
    oatpp::concurrency::SpinLock lock(m_atom);
    entry->next = m_rootEntry;
    m_rootEntry = entry;
    -- m_objectsCount;
  } else {
    OATPP_LOGD("[oatpp::base::memory::MemoryPool::freeByEntryHeader()]",
      "Error. Invalid EntryHeader. Expected poolId=%d, entry poolId=%d", m_id, entry->poolId);
    throw std::runtime_error("[oatpp::base::memory::MemoryPool::freeByEntryHeader()]: Invalid EntryHeader");
  }
}

void MemoryPool::free(void* entry) {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
  delete [] ((p_char8) entry);
#else
  EntryHeader* header = (EntryHeader*)(((p_char8) entry) - sizeof (EntryHeader));
  header->pool->freeByEntryHeader(header);
#endif
}

std::string MemoryPool::getName(){
  return m_name;
}

v_int32 MemoryPool::getEntrySize(){
  return m_entrySize;
}

v_int64 MemoryPool::getSize(){
  return m_chunks.size() * m_chunkSize;
}

v_int32 MemoryPool::getObjectsCount(){
  return m_objectsCount;
}
  
  
  
oatpp::concurrency::SpinLock::Atom MemoryPool::POOLS_ATOM(false);
std::unordered_map<v_int64, MemoryPool*> MemoryPool::POOLS;
std::atomic<v_int64> MemoryPool::poolIdCounter(0);
  
ThreadDistributedMemoryPool::ThreadDistributedMemoryPool(const std::string& name, v_int32 entrySize, v_int32 chunkSize, v_int32 shardsCount)
  : m_shardsCount(shardsCount)
  , m_shards(new MemoryPool*[m_shardsCount])
{
  for(v_int32 i = 0; i < m_shardsCount; i++){
    m_shards[i] = new MemoryPool(name + "_" + oatpp::utils::conversion::int32ToStdStr(i), entrySize, chunkSize);
  }
}

ThreadDistributedMemoryPool::~ThreadDistributedMemoryPool(){
  for(v_int32 i = 0; i < m_shardsCount; i++){
    delete m_shards[i];
  }
  delete [] m_shards;
}

void* ThreadDistributedMemoryPool::obtain() {
  static std::atomic<v_word16> base(0);
  static thread_local v_int16 index = (++base) % m_shardsCount;
  return m_shards[index]->obtain();
}
  
}}}
