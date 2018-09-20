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

#ifndef oatpp_base_StrBuffer_hpp
#define oatpp_base_StrBuffer_hpp

#include "memory/ObjectPool.hpp"
#include "./Controllable.hpp"

#include <cstring> // c

namespace oatpp { namespace base {

class StrBuffer : public oatpp::base::Controllable {
public:
  
  class SmStringPool {
  private:
    
    class EntryHeader {
    public:
      
      EntryHeader(SmStringPool* pPool, EntryHeader* pNext)
        : pool(pPool)
        , next(pNext)
      {}
      
      SmStringPool* pool;
      EntryHeader* next;
      
    };
    
  private:
    
    void allocChunk() {
      v_int32 entryBlockSize = sizeof(EntryHeader) + m_entrySize;
      v_int32 chunkMemSize = entryBlockSize * m_chunkSize;
      p_char8 mem = new v_char8[chunkMemSize];
      m_chunks.push_back(mem);
      for(v_int32 i = 0; i < m_chunkSize; i++){
        EntryHeader* entry = new (mem + i * entryBlockSize) EntryHeader(this, m_rootEntry);
        m_rootEntry = entry;
      }
    }
    
  private:
    std::string m_name;
    v_int32 m_entrySize;
    v_int32 m_chunkSize;
    std::list<p_char8> m_chunks;
    EntryHeader* m_rootEntry;
    oatpp::concurrency::SpinLock::Atom m_atom;
    v_int32 m_objectsCount;
  public:
    
    SmStringPool(const std::string& name, v_int32 entrySize, v_int32 chunkSize)
      : m_name(name)
      , m_entrySize(entrySize)
      , m_chunkSize(chunkSize)
      , m_rootEntry(nullptr)
      , m_atom(false)
      , m_objectsCount(0)
    {
      allocChunk();
    }
    
    ~SmStringPool() {
      auto it = m_chunks.begin();
      while (it != m_chunks.end()) {
        p_char8 chunk = *it;
        delete [] chunk;
        it++;
      }
    }
    
    void* obtain() {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
      return new v_char8[m_entrySize];
#else
      //oatpp::concurrency::SpinLock lock(m_atom);
      if(m_rootEntry != nullptr) {
        auto entry = m_rootEntry;
        m_rootEntry = m_rootEntry->next;
        ++ m_objectsCount;
        return ((p_char8) entry) + sizeof(EntryHeader);
      } else {
        allocChunk();
        if(m_rootEntry == nullptr) {
          throw std::runtime_error("oatpp::base::memory::MemoryPool: Unable to allocate entry");
        }
        auto entry = m_rootEntry;
        m_rootEntry = m_rootEntry->next;
        ++ m_objectsCount;
        return ((p_char8) entry) + sizeof(EntryHeader);
      }
#endif
    }
    
    void freeByEntryHeader(EntryHeader* entry) {
        oatpp::concurrency::SpinLock lock(m_atom);
        entry->next = m_rootEntry;
        m_rootEntry = entry;
        -- m_objectsCount;
    }
    
    static void free(void* entry) {
#ifdef OATPP_DISABLE_POOL_ALLOCATIONS
      delete [] ((p_char8) entry);
#else
      EntryHeader* header = (EntryHeader*)(((p_char8) entry) - sizeof (EntryHeader));
      header->pool->freeByEntryHeader(header);
#endif
    }
    
    std::string getName(){
      return m_name;
    }
    
    v_int32 getEntrySize(){
      return m_entrySize;
    }
    
    v_int64 getSize(){
      return m_chunks.size() * m_chunkSize;
    }
    
    v_int32 getObjectsCount(){
      return m_objectsCount;
    }
    
  };
  
  typedef oatpp::base::memory::AllocationExtras AllocationExtras;
  
  template<class T, class P>
  class SmStringPoolSharedObjectAllocator {
  public:
    typedef T value_type;
  public:
    AllocationExtras& m_info;
    P* m_pool;
  public:
    
    SmStringPoolSharedObjectAllocator(AllocationExtras& info, P* pool)
    : m_info(info)
    , m_pool(pool)
    {};
    
    template<typename U>
    SmStringPoolSharedObjectAllocator(const SmStringPoolSharedObjectAllocator<U, P>& other)
      : m_info(other.m_info)
      , m_pool(other.m_pool)
    {};
    
    T* allocate(std::size_t n) {
      void* mem;
      //OATPP_LOGD("Allocator", "Pool. Size=%d", sizeWanted);
      mem = m_pool->obtain();
      m_info.baseSize = sizeof(T);
      m_info.extraPtr = &((p_char8) mem)[sizeof(T)];
      return static_cast<T*>(mem);
    }
    
    void deallocate(T* ptr, size_t n) {
      //OATPP_LOGD("Allocator", "Free Pool");
      //oatpp::base::memory::MemoryPool::free(ptr);
      SmStringPool::free(ptr);
    }
    
  };
  
  template<typename T, typename P, typename ... Args>
  static std::shared_ptr<T> poolAllocateSharedWithExtras(AllocationExtras& extras, P* pool, Args... args){
    typedef SmStringPoolSharedObjectAllocator<T, P> _Allocator;
    _Allocator allocator(extras, pool);
    return std::allocate_shared<T, _Allocator>(allocator, args...);
  }
  
public:
  OBJECT_POOL_THREAD_LOCAL(StrBuffer_Pool, StrBuffer, 32)
private:
  static SmStringPool* getSmallStringPool() {
    static thread_local SmStringPool pool("Small_String_Pool", 200, 32);
    //static oatpp::base::memory::ThreadDistributedMemoryPool pool("Small_String_Pool", 300, 128);
    return &pool;
  }
private:
  p_char8 m_data;
  v_int32 m_size;
  bool m_hasOwnData;
private:
  
  void set(const void* data, v_int32 size, bool hasOwnData);
  void setAndCopy(const void* data, const void* originData, v_int32 size);
  static std::shared_ptr<StrBuffer> allocShared(const void* data, v_int32 size, bool copyAsOwnData);

  /**
   *  Allocate memory for string or use originData
   *  if copyAsOwnData == false return originData
   */
  static p_char8 allocStrBuffer(const void* originData, v_int32 size, bool copyAsOwnData);
  
public:
  StrBuffer()
    : m_data((p_char8)"[<nullptr>]")
    , m_size(11)
    , m_hasOwnData(false)
  {}
  
  StrBuffer(const void* data, v_int32 size, bool copyAsOwnData)
    : m_data(allocStrBuffer(data, size, copyAsOwnData))
    , m_size(size)
    , m_hasOwnData(copyAsOwnData)
  {}
public:
  
  virtual ~StrBuffer();
  
  static std::shared_ptr<StrBuffer> createShared(v_int32 size);
  static std::shared_ptr<StrBuffer> createShared(const void* data, v_int32 size, bool copyAsOwnData = true);
  static std::shared_ptr<StrBuffer> createShared(const char* data, bool copyAsOwnData = true);
  static std::shared_ptr<StrBuffer> createShared(StrBuffer* other, bool copyAsOwnData = true);
  
  static std::shared_ptr<StrBuffer> createSharedConcatenated(const void* data1, v_int32 size1, const void* data2, v_int32 size2);
  
  static std::shared_ptr<StrBuffer> createFromCString(const char* data, bool copyAsOwnData = true) {
    if(data != nullptr) {
      return allocShared(data, (v_int32) std::strlen(data), copyAsOwnData);
    }
    return nullptr;
  }
  
  p_char8 getData() const;
  v_int32 getSize() const;
  
  const char* c_str() const;
  std::string std_str() const;
  
  bool hasOwnData() const;
  
  /**
   *  (correct for ACII only)
   */
  std::shared_ptr<StrBuffer> toLowerCase() const;
  
  /**
   *  (correct for ACII only)
   */
  std::shared_ptr<StrBuffer> toUpperCase() const;
  
  bool equals(const void* data, v_int32 size) const;
  bool equals(const char* data) const;
  bool equals(StrBuffer* other) const;
  
  bool startsWith(const void* data, v_int32 size) const;
  bool startsWith(const char* data) const;
  bool startsWith(StrBuffer* data) const;
  
public:
  
  static v_int32 compare(const void* data1, const void* data2, v_int32 size);
  static v_int32 compare(StrBuffer* str1, StrBuffer* str2);
  
  static bool equals(const void* data1, const void* data2, v_int32 size);
  static bool equals(const char* data1, const char* data2);
  static bool equals(StrBuffer* str1, StrBuffer* str2);
  
  // Case Insensitive (correct for ASCII only)
  
  static bool equalsCI(const void* data1, const void* data2, v_int32 size);
  static bool equalsCI(const char* data1, const char* data2);
  static bool equalsCI(StrBuffer* str1, StrBuffer* str2);
  
  // Case Insensitive Fast (ASCII only, correct compare if one of strings contains letters only)
  
  static bool equalsCI_FAST(const void* data1, const void* data2, v_int32 size);
  static bool equalsCI_FAST(const char* data1, const char* data2);
  static bool equalsCI_FAST(StrBuffer* str1, StrBuffer* str2);
  static bool equalsCI_FAST(StrBuffer* str1, const char* str2);
  
  /**
   *  lower case chars in the buffer @data (correct for ACII only)
   */
  static void lowerCase(const void* data, v_int32 size);
  
  /**
   *  upper case chars in the buffer @data (correct for ACII only)
   */
  static void upperCase(const void* data, v_int32 size);
  
};
  
}}
  
#endif /* oatpp_base_StrBuffer_hpp */
