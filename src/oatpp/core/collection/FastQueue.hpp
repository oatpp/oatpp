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

#ifndef oatpp_collection_FastQueue_hpp
#define oatpp_collection_FastQueue_hpp

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/base/Environment.hpp"

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
    entry->_ref = nullptr;
    if(last == nullptr) {
      first = entry;
      last = entry;
    } else {
      last->_ref = entry;
      last = entry;
    }
  }
  
  void round(){
    last->_ref = first;
    last = first;
    first = first->_ref;
    last->_ref = nullptr;
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
    delete result;
  }
  
  void removeEntry(T* entry, T* prevEntry){
    
    if(prevEntry == nullptr) {
      popFrontNoData();
    } else if(entry->_ref == nullptr) {
      prevEntry->_ref = nullptr;
      last = prevEntry;
      delete entry;
    } else {
      prevEntry->_ref = entry->_ref;
      delete entry;
    }
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
      delete curr;
      curr = next;
    }
    first = nullptr;
    last = nullptr;
  }
  
};
  
}}

#endif /* FastQueue_hpp */
