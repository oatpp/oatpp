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
  public:
    
    Queue();
    Entry* peekFront();
    Entry* popFront();
    void pushFront(Routine* routine);
    void pushFront(Entry* entry);
    void pushBack(Routine* routine);
    void pushBack(Entry* entry);
    static void moveEntryToQueue(Queue& from, Queue& to, Queue::Entry* curr, Queue::Entry* prev);
    
  };
  
private:
  
  void abortCurrentRoutine();
  Routine* returnFromRoutine(Routine* from);
  void returnFromCurrentRoutine();
  void doAction(Action& a);
  void propagateError(Error& error);
  bool auditQueueSlow();
  bool countDownToSleep();
  void checkAudit();
  
private:
  Queue m_queue;
  Queue m_queueSlow;
  v_int32 m_auditTimer; // in iterations
  v_int32 m_sleepCountDown; // in iterations
public:
  
  Processor()
    : m_auditTimer(0)
    , m_sleepCountDown(0)
  {}
  
  void addRoutine(const Routine::Builder& routineBuilder);
  bool iterate();
  
};
  
}}

#endif /* Processor_hpp */
