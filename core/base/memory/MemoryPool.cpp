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

#include "MemoryPool.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

namespace oatpp { namespace base { namespace  memory {

oatpp::concurrency::SpinLock::Atom MemoryPool::POOLS_ATOM(false);
std::unordered_map<v_int64, MemoryPool*> MemoryPool::POOLS;
std::atomic<v_int64> MemoryPool::poolIdCounter(0);
  
ThreadDistributedMemoryPool::ThreadDistributedMemoryPool(const std::string& name, v_int32 entrySize, v_int32 chunkSize, v_word32 shardsCount)
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
  static std::hash<std::thread::id> hashFunction;
  static thread_local v_word32 hash = hashFunction(std::this_thread::get_id()) % m_shardsCount;
  return m_shards[hash]->obtain();
}
  
}}}
