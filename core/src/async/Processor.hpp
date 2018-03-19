//
//  Processor.hpp
//  crud
//
//  Created by Leonid on 3/17/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_async_Processor_hpp
#define oatpp_async_Processor_hpp

#include "./Routine.hpp"
#include "../concurrency/SpinLock.hpp"

namespace oatpp { namespace async {
  
class Processor {
public:
  Processor& getThreadLocalProcessor(){
    static thread_local Processor processor;
    return processor;
  }
private:
  
  class Queue {
  public:
    class Entry {
    public:
      Entry(Routine* pRoutine, Entry* pNext)
        : routine(pRoutine)
        , next(pNext)
      {}
      Routine* routine;
      Entry* next;
    };
  private:
    oatpp::concurrency::SpinLock::Atom m_atom;
    Entry* m_first;
    Entry* m_last;
    v_int32 m_count;
    v_int32 m_max;
  public:
    
    Queue()
      : m_atom(false)
      , m_first(nullptr)
      , m_last(nullptr)
      , m_count(0)
      , m_max(0)
    {}
    
    Entry* peekFront() {
      return m_first;
    }
    
    Entry* popFront() {
      oatpp::concurrency::SpinLock lock(m_atom);
      auto result = m_first;
      if(m_first != nullptr) {
        m_first = m_first->next;
        if(m_first == nullptr) {
          m_last = nullptr;
        }
        m_count --;
      }
      return result;
    }
    
    void pushFront(Routine* routine) {
      pushFront(new Entry(routine, nullptr));
    }
    
    void pushFront(Entry* entry){
      oatpp::concurrency::SpinLock lock(m_atom);
      entry->next = m_first;
      m_first = entry;
      if(m_last == nullptr) {
        m_last = entry;
      }
      m_count ++;
    }
    
    void pushBack(Routine* routine) {
      pushBack(new Entry(routine, nullptr));
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
      m_count ++;
      if(m_count > m_max) {
        m_max = m_count;
        //OATPP_LOGD("queue", "size=%d", m_max);
      }
    }
    
    v_int32 getCount(){
      return m_count;
    }
    
    static void moveEntryToQueue(Queue& from, Queue& to, Queue::Entry* curr, Queue::Entry* prev){
      //OATPP_LOGD("proc", "moved to fast");
      if(prev == nullptr) {
        to.pushFront(from.popFront());
      } else if(curr->next == nullptr) {
        to.pushBack(curr);
        from.m_last = prev;
        prev->next = nullptr;
        from.m_count --;
      } else {
        prev->next = curr->next;
        to.pushBack(curr);
        from.m_count --;
      }
      
    }
    
  };
  
private:
  
  void abortCurrentRoutine(){
    auto entry = m_queue.popFront();
    auto curr = entry->routine;
    while(curr != nullptr) {
      auto parent = curr->m_parent;
      delete curr;
      curr = parent;
    }
    delete entry;
  }
  
  void returnFromCurrentRoutine(){
    //OATPP_LOGD("R", "_return");
    auto entry = m_queue.popFront();
    auto routine = entry->routine->m_parent;
    delete entry->routine;
    if(routine != nullptr) {
      entry->routine = routine;
      routine->blocks.popNoData();
      m_queue.pushBack(entry);
    } else {
      delete entry;
    }
  }
  
  void doAction(Action& a){
    if(a.getType() == Action::TYPE_REPEAT) {
      m_queue.pushBack(m_queue.popFront());
      return;
    } else if(a.getType() == Action::TYPE_WAIT_RETRY) {
      m_queueSlow.pushBack(m_queue.popFront());
      return;
    } else if(a.getType() == Action::TYPE_RETURN) {
      auto entry = m_queue.popFront();
      auto routine = entry->routine->m_parent;
      delete entry->routine;
      delete entry;
      if(routine != nullptr) {
        m_queue.pushBack(routine);
      }
      return;
    } else if(a.getType() == Action::TYPE_ABORT){
      abortCurrentRoutine();
      return;
    } else if(a.getType() == Action::TYPE_ROUTINE) {
      auto entry = m_queue.popFront();
      if(!a.m_routine->blocks.isEmpty()){
        Routine* r = a.m_routine;
        a.m_routine = nullptr;
        r->m_parent = entry->routine;
        entry->routine = r;
      } else {
        entry->routine->blocks.popNoData();
      }
      m_queue.pushBack(entry);
      return;
    }
    throw std::runtime_error("Invalid action type");
  }
  
  void propagateError(Error& error){
    Routine* curr = m_queue.peekFront()->routine;
    while (curr != nullptr) {
      
      if(!curr->blocks.isEmpty()) {
        auto block = curr->blocks.peek();
        if(block.errorHandler != nullptr) {
          try {
            auto action = block.errorHandler(error);
            if(action.isErrorAction()) {
              error = action.getError();
            } else {
              doAction(action);
              return;
            }
          } catch(...) {
            error = {"Unknown", true};
          }
        }
        
      }
      
      auto parent = curr->m_parent;
      auto entry = m_queue.popFront();
      delete curr;
      delete entry;
      curr = parent;
      if(curr != nullptr) {
        m_queue.pushFront(curr);
      }
    }
  }
  
  
  
  bool auditQueueSlow(){
    
    m_auditTimer = 0;
    
    Queue::Entry* curr = m_queueSlow.peekFront();
    Queue::Entry* prev = nullptr;
    bool hasActions = false;
    while (curr != nullptr) {
      
      auto& block = curr->routine->blocks.peek();
      
      try{
        Action action = block.function();
        if(action.getType() != Action::TYPE_WAIT_RETRY){
          curr->routine->pendingAction = action;
          action.null();
          Queue::moveEntryToQueue(m_queueSlow, m_queue, curr, prev);
          hasActions = true;
          if(prev != nullptr) {
            curr = prev;
          } else {
            curr = m_queueSlow.peekFront();
          }
        }
      } catch(...) {
        Error error {"Unknown", true };
        curr->routine->pendingAction = error;
        Queue::moveEntryToQueue(m_queueSlow, m_queue, curr, prev);
        hasActions = true;
        if(prev != nullptr) {
          curr = prev;
        } else {
          curr = m_queueSlow.peekFront();
        }
      }
      
      prev = curr;
      if(curr != nullptr) {
        curr = curr->next;
      }
    }
    
    return hasActions;
    
  }
  
  void checkAudit(){
    m_auditTimer ++;
    if(m_auditTimer > 100) {
      auditQueueSlow();
    }
  }
  
private:
  Queue m_queue;
  Queue m_queueSlow;
  v_int32 m_auditTimer; // in cycles
public:
  
  Processor()
    : m_auditTimer(0)
  {}
  
  void addRoutine(const Routine::Builder& routine){
    m_queue.pushBack(routine.m_routine);
    routine.m_routine = nullptr;
  }
  
  bool iterate() {
    
    auto entry = m_queue.peekFront();
    if(entry != nullptr) {
      
      auto r = entry->routine;
      if(r->blocks.isEmpty()){
        returnFromCurrentRoutine();
        checkAudit();
        return true;
      }
      
      if(!r->pendingAction.isNone()) {
        Action action = r->pendingAction;
        r->pendingAction.null();
        if(action.isErrorAction()){
          propagateError(action.getError());
        } else {
          doAction(action);
        }
        checkAudit();
        return true;
      }
      
      auto& block = r->blocks.peek();
      
      try{
        Action action = block.function();
        if(action.isErrorAction()){
          propagateError(action.getError());
        } else {
          doAction(action);
        }
      } catch(...) {
        Error error {"Unknown", true };
        propagateError(error);
      }
      
      checkAudit();
      return true;
      
    }
    
    return auditQueueSlow();
    
  }
  
};
  
}}

#endif /* Processor_hpp */
