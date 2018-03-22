//
//  FastQueue.hpp
//  crud
//
//  Created by Leonid on 3/21/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_collection_FastQueue_hpp
#define oatpp_collection_FastQueue_hpp

#include "../base/Environment.hpp"

namespace oatpp { namespace collection {
  
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
    memcmp(newIndex, m_index, m_size);
    
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
  
template<typename T>
class FastQueue {
public:
  
  FastQueue(Bench<T>& pBench)
    : bench(pBench)
    , first(nullptr)
    , last(nullptr)
  {}
  
  ~FastQueue(){
    clear();
  }
  
  Bench<T>& bench;
  T* first;
  T* last;
  
  void pushFront(T* entry) {
    entry->_ref = first;
    first = entry;
    if(last == nullptr) {
      last = first;
    }
  }
  
  void pushBack(T* entry) {
    if(last == nullptr) {
      first = entry;
      last = entry;
    } else {
      last->_ref = entry;
      last = entry;
    }
  }
  
  T* popFront() {
    T* result = first;
    first = first->next;
    if(first == nullptr) {
      last = nullptr;
    }
    return result;
  }
  
  void popFrontNoData() {
    T* result = first;
    first = first->next;
    if(first == nullptr) {
      last = nullptr;
    }
    bench.free(result);
  }
  
  void moveEntry(FastQueue& fromQueue, FastQueue& toQueue, T* entry, T* prevEntry){

    if(prevEntry == nullptr) {
      toQueue.pushFront(fromQueue.popFront());
    } else if(entry->next == nullptr) {
      toQueue.pushBack(entry);
      fromQueue.m_last = prevEntry;
      prevEntry->next = nullptr;
    } else {
      prevEntry->next = entry->next;
      toQueue.pushBack(entry);
    }
    
  }
  
  void clear() {
    T* curr = first;
    while (curr != nullptr) {
      T* next = curr->_ref;
      bench.free(curr);
      curr = next;
    }
    first = nullptr;
    last = nullptr;
  }
  
};
  
}}

#endif /* FastQueue_hpp */
