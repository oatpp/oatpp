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

#ifndef oatpp_base_memory_Allocator_hpp
#define oatpp_base_memory_Allocator_hpp

#include "./MemoryPool.hpp"

namespace oatpp { namespace base { namespace memory {

class AllocatorPoolInfo {
public:
  AllocatorPoolInfo(const char* pPoolName, v_int32 pPoolChunkSize)
    : poolName(pPoolName)
    , poolChunkSize(pPoolChunkSize)
  {}
  const char* const poolName;
  const v_int32 poolChunkSize;
};
  
template<class T>
class PoolSharedObjectAllocator {
public:
  typedef T value_type;
public:
  const AllocatorPoolInfo& m_poolInfo;
public:
  static oatpp::base::memory::ThreadDistributedMemoryPool& getPool(const AllocatorPoolInfo& info){
    static oatpp::base::memory::ThreadDistributedMemoryPool pool(info.poolName, sizeof(T), info.poolChunkSize);
    return pool;
  }
public:
  PoolSharedObjectAllocator(const AllocatorPoolInfo& info)
    : m_poolInfo(info)
  {};
  
  template<typename U>
  PoolSharedObjectAllocator(const PoolSharedObjectAllocator<U>& other)
    : m_poolInfo(other.m_poolInfo)
  {};
  
  T* allocate(std::size_t n) {
    return static_cast<T*>(getPool(m_poolInfo).obtain());
  }
  
  void deallocate(T* ptr, size_t n) {
    oatpp::base::memory::MemoryPool::free(ptr);
  }
  
};

template <typename T, typename U>
inline bool operator == (const PoolSharedObjectAllocator<T>&, const PoolSharedObjectAllocator<U>&) {
  return true;
}

template <typename T, typename U>
inline bool operator != (const PoolSharedObjectAllocator<T>& a, const PoolSharedObjectAllocator<U>& b) {
  return !(a == b);
}
  
template<class T>
class ThreadLocalPoolSharedObjectAllocator {
public:
  typedef T value_type;
public:
  const AllocatorPoolInfo& m_poolInfo;
public:
  static oatpp::base::memory::MemoryPool& getPool(const AllocatorPoolInfo& info){
    static thread_local oatpp::base::memory::MemoryPool pool(info.poolName, sizeof(T), info.poolChunkSize);
    return pool;
  }
public:
  ThreadLocalPoolSharedObjectAllocator(const AllocatorPoolInfo& info)
    : m_poolInfo(info)
  {};
  
  template<typename U>
  ThreadLocalPoolSharedObjectAllocator(const ThreadLocalPoolSharedObjectAllocator<U>& other)
    : m_poolInfo(other.m_poolInfo)
  {};
  
  T* allocate(std::size_t n) {
    return static_cast<T*>(getPool(m_poolInfo).obtain());
  }
  
  void deallocate(T* ptr, size_t n) {
    oatpp::base::memory::MemoryPool::free(ptr);
  }
  
};

template <typename T, typename U>
inline bool operator == (const ThreadLocalPoolSharedObjectAllocator<T>&, const ThreadLocalPoolSharedObjectAllocator<U>&) {
  return true;
}

template <typename T, typename U>
inline bool operator != (const ThreadLocalPoolSharedObjectAllocator<T>& a, const ThreadLocalPoolSharedObjectAllocator<U>& b) {
  return !(a == b);
}
  
class AllocationExtras {
public:
  AllocationExtras(v_int32 pExtraWanted)
  : extraWanted(pExtraWanted)
  {}
  const v_int32 extraWanted;
  void* extraPtr;
  v_int32 baseSize;
};
  
template<class T>
class SharedObjectAllocator {
public:
  typedef T value_type;
public:
  AllocationExtras& m_info;
public:
  
  SharedObjectAllocator(AllocationExtras& info)
    : m_info(info)
  {};
  
  template<typename U>
  SharedObjectAllocator(const SharedObjectAllocator<U>& other)
    : m_info(other.m_info)
  {};
  
  T* allocate(std::size_t n) {
    void* mem = ::operator new(sizeof(T) + m_info.extraWanted);
    m_info.baseSize = sizeof(T);
    m_info.extraPtr = &((p_char8) mem)[sizeof(T)];
    return static_cast<T*>(mem);
  }
  
  void deallocate(T* ptr, size_t n) {
    ::operator delete(ptr);
  }
  
};
  
template<class T, class P>
class CustomPoolSharedObjectAllocator {
public:
  typedef T value_type;
public:
  AllocationExtras& m_info;
  P* m_pool;
public:
  
  CustomPoolSharedObjectAllocator(AllocationExtras& info, P* pool)
    : m_info(info)
    , m_pool(pool)
  {};
  
  template<typename U>
  CustomPoolSharedObjectAllocator(const CustomPoolSharedObjectAllocator<U, P>& other)
    : m_info(other.m_info)
    , m_pool(other.m_pool)
  {};
  
  T* allocate(std::size_t n) {
    void* mem = m_pool->obtain();
    m_info.baseSize = sizeof(T);
    m_info.extraPtr = &((p_char8) mem)[sizeof(T)];
    return static_cast<T*>(mem);
  }
  
  void deallocate(T* ptr, size_t n) {
    oatpp::base::memory::MemoryPool::free(ptr);
  }
  
};

template <typename T, typename U>
inline bool operator == (const SharedObjectAllocator<T>&, const SharedObjectAllocator<U>&) {
  return true;
}

template <typename T, typename U>
inline bool operator != (const SharedObjectAllocator<T>& a, const SharedObjectAllocator<U>& b) {
  return !(a == b);
}
  
template<typename T, typename ... Args>
static std::shared_ptr<T> allocateSharedWithExtras(AllocationExtras& extras, Args... args){
  typedef SharedObjectAllocator<T> _Allocator;
  _Allocator allocator(extras);
  return std::allocate_shared<T, _Allocator>(allocator, args...);
}
  
template<typename T, typename P, typename ... Args>
static std::shared_ptr<T> customPoolAllocateSharedWithExtras(AllocationExtras& extras, P* pool, Args... args){
  typedef CustomPoolSharedObjectAllocator<T, P> _Allocator;
  _Allocator allocator(extras, pool);
  return std::allocate_shared<T, _Allocator>(allocator, args...);
}
  
}}}

#endif /* oatpp_base_memory_Allocator_hpp */
