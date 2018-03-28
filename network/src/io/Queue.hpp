//
//  Queue.hpp
//  crud
//
//  Created by Leonid on 3/15/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_web_server_io_Queue_hpp
#define oatpp_web_server_io_Queue_hpp

#include "./IORequest.hpp"

#include "../../../core/src/base/memory/ObjectPool.hpp"
#include "../../../core/src/concurrency/SpinLock.hpp"

namespace oatpp { namespace network { namespace io {
  
class Queue {
public:
  static Queue& getInstance(){
    static Queue queue;
    return queue;
  }
public:
  class Entry {
  public:
    OBJECT_POOL_THREAD_LOCAL(IO_Queue_Entry_Pool, Entry, 32)
  public:
    Entry(IORequest& pRequest, Entry* pNext)
      : request(pRequest)
      , next(pNext)
    {}
    IORequest& request;
    Entry* next;
  };
private:
  oatpp::concurrency::SpinLock::Atom m_atom;
  Entry* m_first;
  Entry* m_last;
public:
  
  Queue()
    : m_atom(false)
    , m_first(nullptr)
    , m_last(nullptr)
  {}
  
  Entry* popFront() {
    oatpp::concurrency::SpinLock lock(m_atom);
    auto result = m_first;
    if(m_first != nullptr) {
      m_first = m_first->next;
      if(m_first == nullptr) {
        m_last = nullptr;
      }
    }
    return result;
  }
  
  void pushBack(IORequest& request) {
    pushBack(new Entry(request, nullptr));
  }
  
  void pushBack(Entry* entry) {
    oatpp::concurrency::SpinLock lock(m_atom);
    entry->next = nullptr;
    if(m_last != nullptr) {
      m_last->next = entry;
      m_last = entry;
    } else {
      m_first = entry;
      m_last = entry;
    }
  }
  
};
  
}}}

#endif /* oatpp_web_server_io_Queue_hpp */
