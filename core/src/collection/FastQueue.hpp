//
//  FastQueue.hpp
//  crud
//
//  Created by Leonid on 3/21/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_collection_FastQueue_hpp
#define oatpp_collection_FastQueue_hpp

#include "../concurrency/SpinLock.hpp"
#include "../base/Environment.hpp"

namespace oatpp { namespace collection {
  
template<typename T>
class FastQueue {
public:
  
  FastQueue()
    : first(nullptr)
    , last(nullptr)
  {}
  
  ~FastQueue(){
    clear();
  }
  
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
    first = first->_ref;
    if(first == nullptr) {
      last = nullptr;
    }
    return result;
  }
  
  void popFrontNoData() {
    T* result = first;
    first = first->_ref;
    if(first == nullptr) {
      last = nullptr;
    }
    result->free();
  }
  
  static void moveEntry(FastQueue& fromQueue, FastQueue& toQueue, T* entry, T* prevEntry){

    if(prevEntry == nullptr) {
      toQueue.pushFront(fromQueue.popFront());
    } else if(entry->_ref == nullptr) {
      toQueue.pushBack(entry);
      fromQueue.last = prevEntry;
      prevEntry->_ref = nullptr;
    } else {
      prevEntry->_ref = entry->_ref;
      toQueue.pushBack(entry);
    }
    
  }
  
  void clear() {
    T* curr = first;
    while (curr != nullptr) {
      T* next = curr->_ref;
      curr->free();
      curr = next;
    }
    first = nullptr;
    last = nullptr;
  }
  
};
  
}}

#endif /* FastQueue_hpp */
