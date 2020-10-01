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

#ifndef oatpp_base_memory_ObjectPool_hpp
#define oatpp_base_memory_ObjectPool_hpp

#include "./Allocator.hpp"
#include "./MemoryPool.hpp"

namespace oatpp { namespace base { namespace memory {

/**
 * Macro to declare object pool class which uses &id:oatpp::base::memory::PoolSharedObjectAllocator; to allocate objects.
 * @param NAME - name of the memory pool.
 * @param TYPE - type of the object.
 * @param CHUNK_SIZE - chunk size for &id:oatpp::base::memory::MemoryPool;.
 */
#define SHARED_OBJECT_POOL(NAME, TYPE, CHUNK_SIZE) \
class NAME { \
public: \
  typedef TYPE ObjectType; \
  typedef oatpp::base::memory::PoolSharedObjectAllocator<TYPE> Allocator; \
public: \
  \
  static const oatpp::base::memory::AllocatorPoolInfo& getPoolInfo(){ \
    static oatpp::base::memory::AllocatorPoolInfo info(#NAME"<"#TYPE">", CHUNK_SIZE); \
    return info; \
  } \
  \
  static Allocator& getAllocator (){ \
    static Allocator allocator(getPoolInfo()); \
    return allocator; \
  } \
  \
  template<typename ... Args> \
  static std::shared_ptr<TYPE> allocateShared(Args... args){ \
    return std::allocate_shared<TYPE, Allocator>(getAllocator(), args...); \
  } \
  \
};

/**
 * Macro to declare object pool class which uses &id:oatpp::base::memory::ThreadLocalPoolSharedObjectAllocator; to allocate objects.
 * @param NAME - name of the memory pool.
 * @param TYPE - type of the object.
 * @param CHUNK_SIZE - chunk size for &id:oatpp::base::memory::MemoryPool;.
 */
#define SHARED_OBJECT_POOL_THREAD_LOCAL(NAME, TYPE, CHUNK_SIZE) \
class NAME { \
public: \
  typedef TYPE ObjectType; \
  typedef oatpp::base::memory::ThreadLocalPoolSharedObjectAllocator<TYPE> Allocator; \
public: \
  \
  static const oatpp::base::memory::AllocatorPoolInfo& getPoolInfo(){ \
    static oatpp::base::memory::AllocatorPoolInfo info(#NAME"<"#TYPE">", CHUNK_SIZE); \
    return info; \
  } \
  \
  static Allocator& getAllocator (){ \
    static Allocator allocator(getPoolInfo()); \
    return allocator; \
  } \
  \
  template<typename ... Args> \
  static std::shared_ptr<TYPE> allocateShared(Args... args){ \
    return std::allocate_shared<TYPE, Allocator>(getAllocator(), args...); \
  } \
  \
};

/**
 * Macro to declare: &id:oatpp::base::memory::MemoryPool; for object, plus class-specific operators
 * `static void* operator new(std::size_t sz)`, `static void operator delete(void* ptr, std::size_t sz)`,
 * `static void* operator new(std::size_t sz, void* entry)`, `static void operator delete(void* ptr, void* entry)`.
 * @param NAME - name of the memory pool.
 * @param TYPE - type of the object.
 * @param CHUNK_SIZE - chunk size for &id:oatpp::base::memory::MemoryPool;.
 */
#define OBJECT_POOL(POOL_NAME, TYPE, CHUNK_SIZE)  \
class POOL_NAME { \
public: \
\
  static oatpp::base::memory::ThreadDistributedMemoryPool& getPool(){ \
    static auto pool = new oatpp::base::memory::ThreadDistributedMemoryPool(#POOL_NAME"<"#TYPE">", sizeof(TYPE), CHUNK_SIZE); \
    return *pool; \
  } \
\
}; \
\
static void* operator new(std::size_t sz) { \
  if(sz != sizeof(TYPE)){ \
    throw std::runtime_error("wrong object size"); \
  } \
  static auto& pool = POOL_NAME::getPool(); \
  return pool.obtain(); \
} \
\
static void operator delete(void* ptr, std::size_t sz) { \
  if(sz != sizeof(TYPE)){ \
    oatpp::base::Environment::log(2, #POOL_NAME, "[ERROR|CRITICAL]: MemoryPool malfunction. Deleting object of wrong size"); \
  } \
  oatpp::base::memory::MemoryPool::free(ptr); \
} \
\
static void* operator new(std::size_t sz, void* entry) { \
  if(sz != sizeof(TYPE)){ \
    throw std::runtime_error("wrong object size"); \
  } \
  return entry; \
} \
\
static void operator delete(void* ptr, void* entry) { \
  (void)ptr; \
  (void)entry; \
}

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  /**
   * Macro to declare: `thread_local` &id:oatpp::base::memory::MemoryPool; for object, plus class-specific operators <br>
   * `static void* operator new(std::size_t sz)`, `static void operator delete(void* ptr, std::size_t sz)`, <br>
   * `static void* operator new(std::size_t sz, void* entry)`, `static void operator delete(void* ptr, void* entry)`. <br>
   * *Memory pool is NOT `thread_local` if built with `-DOATPP_COMPAT_BUILD_NO_THREAD_LOCAL` flag*
   * @param NAME - name of the memory pool.
   * @param TYPE - type of the object.
   * @param CHUNK_SIZE - chunk size for &id:oatpp::base::memory::MemoryPool;.
   */
  #define OBJECT_POOL_THREAD_LOCAL(POOL_NAME, TYPE, CHUNK_SIZE)  \
  class POOL_NAME { \
  public: \
  \
    static oatpp::base::memory::MemoryPool& getPool(){ \
      static thread_local oatpp::base::memory::MemoryPool pool(#POOL_NAME"<"#TYPE">", sizeof(TYPE), CHUNK_SIZE); \
      return pool; \
    } \
  \
  }; \
  \
  static void* operator new(std::size_t sz) { \
    if(sz != sizeof(TYPE)){ \
      throw std::runtime_error("wrong object size"); \
    } \
    static thread_local auto& pool = POOL_NAME::getPool(); \
    return pool.obtain(); \
  } \
  \
  static void operator delete(void* ptr, std::size_t sz) { \
    if(sz != sizeof(TYPE)){ \
      oatpp::base::Environment::log(2, #POOL_NAME, "[ERROR|CRITICAL]: MemoryPool malfunction. Deleting object of wrong size"); \
    } \
    oatpp::base::memory::MemoryPool::free(ptr); \
  } \
  \
  static void* operator new(std::size_t sz, void* entry) { \
    if(sz != sizeof(TYPE)){ \
      throw std::runtime_error("wrong object size"); \
    } \
    return entry; \
  } \
  \
  static void operator delete(void* ptr, void* entry) { \
    (void)ptr; \
    (void)entry; \
  }
#else
  #define OBJECT_POOL_THREAD_LOCAL(POOL_NAME, TYPE, CHUNK_SIZE) \
  class POOL_NAME { \
  public: \
  \
    static oatpp::base::memory::MemoryPool& getPool(){ \
      static auto pool = new oatpp::base::memory::MemoryPool(#POOL_NAME"<"#TYPE">", sizeof(TYPE), CHUNK_SIZE); \
      return *pool; \
    } \
  \
  };
#endif

}}}

#endif /* oatpp_base_memory_ObjectPool_hpp */
