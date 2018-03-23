//
//  Processor2.hpp
//  crud
//
//  Created by Leonid on 3/20/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_async_Processor2_hpp
#define oatpp_async_Processor2_hpp

#include "./Coroutine.hpp"
#include "../collection/FastQueue.hpp"

namespace oatpp { namespace async {
  
class Processor2 {
private:
  
  bool checkWaitingQueue() {
    bool hasActions = false;
    AbstractCoroutine* curr = m_waitingQueue.first;
    AbstractCoroutine* prev = nullptr;
    while (curr != nullptr) {
      
      const Action2& action = curr->iterate();
      if(action.m_type != Action2::TYPE_WAIT_RETRY) {
        m_waitingQueue.pushBack(m_activeQueue.popFront());
        hasActions = true;
        oatpp::collection::FastQueue<AbstractCoroutine>::moveEntry(m_waitingQueue, m_activeQueue, curr, prev);
        hasActions = true;
        if(prev != nullptr) {
          curr = prev;
        } else {
          curr = m_waitingQueue.first;
        }
      }
      
      prev = curr;
      if(curr != nullptr) {
        curr = curr->_ref;
      }
      
    }
    return hasActions;
  }
  
private:
  oatpp::collection::FastQueue<AbstractCoroutine> m_activeQueue;
  oatpp::collection::FastQueue<AbstractCoroutine> m_waitingQueue;
public:

  void addCoroutine(AbstractCoroutine* coroutine) {
    m_activeQueue.pushBack(coroutine);
  }
  
  bool iterate(v_int32 numIterations) {
    
    for(v_int32 i = 0; i < numIterations; i++) {
      
      auto CP = m_activeQueue.first;
      if(CP == nullptr) {
        break;
      }
      if(!CP->finished()) {
        const Action2& action = CP->iterate();
        if(action.m_type == Action2::TYPE_WAIT_RETRY) {
          m_waitingQueue.pushBack(m_activeQueue.popFront());
        }
      } else {
        m_activeQueue.popFrontNoData();
      }
      
    }
    
    return (m_activeQueue.first != nullptr);
    
  }
  
  
};
  
}}

#endif /* oatpp_async_Processor2_hpp */
