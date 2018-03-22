//
//  Processor.cpp
//  crud
//
//  Created by Leonid on 3/17/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "Processor.hpp"

namespace oatpp { namespace async {
  
  
Processor::Queue::Queue()
  : m_atom(false)
  , m_first(nullptr)
  , m_last(nullptr)
{}

Processor::Queue::Entry* Processor::Queue::peekFront() {
  return m_first;
}

Processor::Queue::Entry* Processor::Queue::popFront() {
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

void Processor::Queue::pushFront(Routine* routine) {
  pushFront(new Entry(routine, nullptr));
}

void Processor::Queue::pushFront(Entry* entry){
  oatpp::concurrency::SpinLock lock(m_atom);
  entry->next = m_first;
  m_first = entry;
  if(m_last == nullptr) {
    m_last = entry;
  }
}

void Processor::Queue::pushBack(Routine* routine) {
  pushBack(new Entry(routine, nullptr));
}

void Processor::Queue::pushBack(Entry* entry) {
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

void Processor::Queue::moveEntryToQueue(Queue& from, Queue& to, Queue::Entry* curr, Queue::Entry* prev){
  //OATPP_LOGD("proc", "moved to fast");
  if(prev == nullptr) {
    to.pushFront(from.popFront());
  } else if(curr->next == nullptr) {
    to.pushBack(curr);
    from.m_last = prev;
    prev->next = nullptr;
  } else {
    prev->next = curr->next;
    to.pushBack(curr);
  }
  
}
  
// Processor
  
void Processor::abortCurrentRoutine(){
  auto entry = m_queue.popFront();
  auto curr = entry->routine;
  while(curr != nullptr) {
    auto parent = curr->m_parent;
    delete curr;
    curr = parent;
  }
  delete entry;
}

Routine* Processor::returnFromRoutine(Routine* from) {
  
  auto curr = from->m_parent;
  delete from;
  while (curr != nullptr) {
    if(curr->blocks.isEmpty()){
      auto parent = curr->m_parent;
      delete curr;
      curr = parent;
    } else {
      return curr;
    }
  }
  
  return nullptr;
  
}
  
void Processor::returnFromCurrentRoutine(){
  //OATPP_LOGD("R", "_return");
  auto entry = m_queue.popFront();
  entry->routine = returnFromRoutine(entry->routine);
  if(entry->routine != nullptr) {
    entry->routine->blocks.popNoData();
    m_queue.pushBack(entry);
  } else {
    delete entry;
  }
}

void Processor::doAction(Action& a){
  if(a.getType() == Action::TYPE_REPEAT) {
    m_queue.pushBack(m_queue.popFront());
  } else if(a.getType() == Action::TYPE_WAIT_RETRY) {
    m_queueSlow.pushBack(m_queue.popFront());
  } else if(a.getType() == Action::TYPE_RETURN) {
    returnFromCurrentRoutine();
  } else if(a.getType() == Action::TYPE_ABORT){
    abortCurrentRoutine();
  } else if(a.getType() == Action::TYPE_ROUTINE) {
    auto entry = m_queue.popFront();
    if(a.m_routine != nullptr && !a.m_routine->blocks.isEmpty()){
      Routine* r = a.m_routine;
      a.m_routine = nullptr;
      r->m_parent = entry->routine;
      entry->routine = r;
    } else {
      entry->routine->blocks.popNoData();
    }
    m_queue.pushBack(entry);
  } else {
    throw std::runtime_error("Invalid action type");
  }
}

void Processor::propagateError(Error& error){
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



bool Processor::auditQueueSlow(){
  
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

void Processor::checkAudit(){
  m_sleepCountDown = 0;
  m_auditTimer ++;
  if(m_auditTimer > 100) {
    auditQueueSlow();
  }
}
  
bool Processor::countDownToSleep() {
  m_sleepCountDown ++;
  auditQueueSlow();
  if(m_sleepCountDown > 100) {
    return false;
  }
  return true;
}
  
void Processor::addRoutine(const Routine::Builder& routineBuilder){
  m_queue.pushBack(routineBuilder.m_routine);
  routineBuilder.m_routine = nullptr;
}

bool Processor::iterate() {
  
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
  
  return countDownToSleep();
  
}
  
  
}}
