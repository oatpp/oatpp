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

#include <mutex>

namespace oatpp { namespace base { namespace  memory {

MemoryPool::MemoryPool(const std::string& name, v_buff_size entrySize, v_buff_size chunkSize)
  : m_name(name)
  , m_entrySize(entrySize)
  , m_chunkSize(chunkSize)
  , m_id(++poolIdCounter)
  , m_rootEntry(nullptr)
  , m_objectsCount(0)
{
  allocChunk();
  std::lock_guard<oatpp::concurrency::SpinLock> lock(POOLS_SPIN_LOCK);
  POOLS[m_id] = this;
}

MemoryPool::~MemoryPool() {
  auto it = m_chunks.begin();
  while (it != m_chunks.end()) {
    p_char8 chunk = *it;
    delete [] chunk;
    it++;
  }
  std::lock_guard<oatpp::concurrency::SpinLock> lock(POOLS_SPIN_LOCK);
  POOLS.erase(m_id);
}

void MemoryPool::allocChunk() {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
  // DO NOTHING
#else
  v_buff_size entryBlockSize = sizeof(EntryHeader) + m_entrySize;
  v_buff_size chunkMemSize = entryBlockSize * m_chunkSize;
  p_char8 mem = new v_char8[chunkMemSize];
  m_chunks.push_back(mem);
  for(v_buff_size i = 0; i < m_chunkSize; i++){
    EntryHeader* entry = new (mem + i * entryBlockSize) EntryHeader(this, m_id, m_rootEntry);
    m_rootEntry = entry;
  }
#endif
}
  
void* MemoryPool::obtain() {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
  return new v_char8[m_entrySize];
#else
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
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

void MemoryPool::freeByEntryHeader(EntryHeader* entry) {
  if(entry->poolId == m_id) {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
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

v_buff_size MemoryPool::getEntrySize(){
  return m_entrySize;
}

v_buff_size MemoryPool::getSize(){
  return m_chunks.size() * m_chunkSize;
}

v_int64 MemoryPool::getObjectsCount(){
  return m_objectsCount;
}

oatpp::concurrency::SpinLock MemoryPool::POOLS_SPIN_LOCK;
std::unordered_map<v_int64, MemoryPool*> MemoryPool::POOLS;
std::atomic<v_int64> MemoryPool::poolIdCounter(0);

const v_int64 ThreadDistributedMemoryPool::SHARDS_COUNT_DEFAULT = OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT;

#if defined(OATPP_DISABLE_POOL_ALLOCATIONS) || defined(OATPP_COMPAT_BUILD_NO_THREAD_LOCAL)
ThreadDistributedMemoryPool::ThreadDistributedMemoryPool(const std::string& name, v_buff_size entrySize, v_buff_size chunkSize, v_int64 shardsCount)
  : m_shardsCount(1)
  , m_shards(new MemoryPool*[1])
  , m_deleted(false)
{
  for(v_int64 i = 0; i < m_shardsCount; i++){
    m_shards[i] = new MemoryPool(name + "_" + oatpp::utils::conversion::int64ToStdStr(i), entrySize, chunkSize);
  }
}
#else
ThreadDistributedMemoryPool::ThreadDistributedMemoryPool(const std::string& name, v_buff_size entrySize, v_buff_size chunkSize, v_int64 shardsCount)
  : m_shardsCount(shardsCount)
  , m_shards(new MemoryPool*[m_shardsCount])
  , m_deleted(false)
{
  for(v_int64 i = 0; i < m_shardsCount; i++){
    m_shards[i] = new MemoryPool(name + "_" + oatpp::utils::conversion::int64ToStdStr(i), entrySize, chunkSize);
  }
}
#endif

ThreadDistributedMemoryPool::~ThreadDistributedMemoryPool(){
  m_deleted = true;
  for(v_int64 i = 0; i < m_shardsCount; i++){
    delete m_shards[i];
  }
  delete [] m_shards;
}

#if defined(OATPP_DISABLE_POOL_ALLOCATIONS) || defined(OATPP_COMPAT_BUILD_NO_THREAD_LOCAL)
void* ThreadDistributedMemoryPool::obtain() {
  if(m_deleted) {
    throw std::runtime_error("[oatpp::base::memory::ThreadDistributedMemoryPool::obtain()]. Error. Pool deleted.");
  }
  return m_shards[0]->obtain();
}
#else
void* ThreadDistributedMemoryPool::obtain() {
  if(m_deleted) {
    throw std::runtime_error("[oatpp::base::memory::ThreadDistributedMemoryPool::obtain()]. Error. Pool deleted.");
  }
  static std::atomic<v_uint16> base(0);
  static thread_local v_int16 index = (++base) % m_shardsCount;
  return m_shards[index]->obtain();
}
#endif
  
}}}
