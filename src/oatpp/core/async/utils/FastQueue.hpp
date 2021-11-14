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

#ifndef oatpp_async_utils_FastQueue_hpp
#define oatpp_async_utils_FastQueue_hpp

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace async { namespace utils {
  
template<typename T>
class FastQueue {
public:
  
  FastQueue()
    : first(nullptr)
    , last(nullptr)
    , count(0)
  {}
  
  ~FastQueue(){
    clear();
  }

  FastQueue(const FastQueue &) = delete;

  FastQueue(FastQueue &&other) noexcept
    : FastQueue() {
      using std::swap;
      swap(first, other.first);
      swap(last, other.last);
      swap(count, other.count);
  }

  FastQueue &operator=(const FastQueue &) = delete;

  FastQueue &operator=(FastQueue &&other) noexcept {
      if (this != std::addressof(other)) {
          using std::swap;
          swap(first, other.first);
          swap(last, other.last);
          swap(count, other.count);
      }
      return *this;
  }

  
  T* first;
  T* last;
  v_int32 count{};

  v_int32 Count() {
      return count;
  }

  bool empty() {
      return count == 0;
  }
  
  void pushFront(T* entry) {
    entry->_ref = first;
    first = entry;
    if(last == nullptr) {
      last = first;
    }
    ++ count;
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
    ++ count;
  }
  
  void round(){
    if(count > 1) {
      last->_ref = first;
      last = first;
      first = first->_ref;
      last->_ref = nullptr;
    }
  }
  
  T* popFront() {
    T* result = first;
    first = first->_ref;
    if(first == nullptr) {
      last = nullptr;
    }
    -- count;
    return result;
  }
  
  void popFrontNoData() {
    T* result = first;
    first = first->_ref;
    if(first == nullptr) {
      last = nullptr;
    }
    delete result;
    -- count;
  }

  static void moveAll(FastQueue& fromQueue, FastQueue& toQueue) {

    if(fromQueue.count > 0) {

      if (toQueue.last == nullptr) {
        toQueue.first = fromQueue.first;
        toQueue.last = fromQueue.last;
      } else {
        toQueue.last->_ref = fromQueue.first;
        toQueue.last = fromQueue.last;
      }

      toQueue.count += fromQueue.count;
      fromQueue.count = 0;

      fromQueue.first = nullptr;
      fromQueue.last = nullptr;

    }

  }

  void cutEntry(T* entry, T* prevEntry){

    if(prevEntry == nullptr) {
      popFront();
    } else {
      prevEntry->_ref = entry->_ref;
      -- count;
      if(prevEntry->_ref == nullptr) {
        last = prevEntry;
      }
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
    count = 0;
  }
  
};
  
}}}

#endif /* oatpp_async_utils_FastQueue_hpp */
