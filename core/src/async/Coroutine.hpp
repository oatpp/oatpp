//
//  Coroutine.hpp
//  crud
//
//  Created by Leonid on 3/22/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_async_Coroutine_hpp
#define oatpp_async_Coroutine_hpp

#include "../collection/FastQueue.hpp"
#include "../base/memory/MemoryPool.hpp"
#include "../base/Environment.hpp"

namespace oatpp { namespace async {

class AbstractCoroutine; // FWD
class Processor2; // FWD
  
class Error2 {
public:

  Error2(const char* pMessage, bool pIsExceptionThrown = false)
    : message(pMessage)
    , isExceptionThrown(pIsExceptionThrown)
  {}
  
  const char* message;
  bool isExceptionThrown;
  
};
  
class Action2 {
  friend Processor2;
  friend AbstractCoroutine;
public:
  typedef Action2 (AbstractCoroutine::*FunctionPtr)();
public:
  static constexpr const v_int32 TYPE_COROUTINE = 0;
  static constexpr const v_int32 TYPE_YIELD_TO = 1;
  static constexpr const v_int32 TYPE_WAIT_RETRY = 2;
  static constexpr const v_int32 TYPE_REPEAT = 3;
  static constexpr const v_int32 TYPE_FINISH = 4;
  static constexpr const v_int32 TYPE_ABORT = 5;
  static constexpr const v_int32 TYPE_ERROR = 6;
public:
  static const Action2 _WAIT_RETRY;
  static const Action2 _REPEAT;
  static const Action2 _FINISH;
  static const Action2 _ABORT;
private:
  v_int32 m_type;
  AbstractCoroutine* m_coroutine;
  FunctionPtr m_functionPtr;
  Error2 m_error;
public:
  
  Action2(v_int32 type,
          AbstractCoroutine* coroutine,
          FunctionPtr functionPtr)
    : m_type(type)
    , m_coroutine(coroutine)
    , m_functionPtr(functionPtr)
    , m_error(Error2(nullptr))
  {}
  
  Action2(const Error2& error)
    : m_type(TYPE_ERROR)
    , m_coroutine(nullptr)
    , m_functionPtr(nullptr)
    , m_error(error)
  {}
  
  bool isError(){
    return m_type == TYPE_ERROR;
  }
  
};
  
class AbstractCoroutine {
  friend oatpp::collection::FastQueue<AbstractCoroutine>;
  friend Processor2;
public:
  typedef Action2 Action2;
  typedef Action2 (AbstractCoroutine::*FunctionPtr)();
private:
  AbstractCoroutine* _CP = this;
  FunctionPtr _FP = &AbstractCoroutine::act;
  AbstractCoroutine* _ref = nullptr;
  
  const Action2& takeAction(const Action2& action){
    
    switch (action.m_type) {

      case Action2::TYPE_COROUTINE:
          action.m_coroutine->m_parent = _CP;
          _CP = action.m_coroutine;
          _FP = action.m_coroutine->_FP;
        break;
        
      case Action2::TYPE_FINISH:
          if(_CP == this) {
            _CP = nullptr;
            return action;
          }
          do {
            _CP->free();
            _CP = _CP->m_parent; // Should be fine here. As free() - return of the pointer to Bench. (Memory not changed)
            takeAction(_CP->m_savedAction);
          } while (_FP == nullptr && _CP != this);
          if(_FP == nullptr &&_CP == this) {
            _CP = nullptr;
            return action;
          }
        break;
        
      case Action2::TYPE_YIELD_TO:
          _FP = action.m_functionPtr;
        break;
        
      case Action2::TYPE_ABORT:
          while (_CP != this) {
            _CP->free();
            _CP = _CP->m_parent;
          }
          _CP = nullptr;
        break;
        
    };
    
    return action;
    
  }
  
private:
  AbstractCoroutine* m_parent = nullptr;
protected:
  Action2 m_savedAction = Action2::_FINISH;
public:
  
  Action2 iterate() {
    return takeAction(_CP->call(_FP));
  };
  
  Action2 iterate(v_int32 numIterations) {
    Action2 action(Action2::TYPE_FINISH, nullptr, nullptr);
    for(v_int32 i = 0; i < numIterations; i++) {
      action = takeAction(_CP->call(_FP));
      if(action.m_type == Action2::TYPE_WAIT_RETRY || _CP == nullptr) {
        return action;
      }
    }
    return action;
  };
  
  virtual ~AbstractCoroutine(){
  }
  
  virtual Action2 act() = 0;
  virtual Action2 call(FunctionPtr ptr) = 0;
  virtual void free() = 0;
  
  bool finished(){
    return _CP == nullptr;
  }
  
  AbstractCoroutine* getParent(){
    return m_parent;
  }
  
};
 
template<class T>
class Coroutine : public AbstractCoroutine {
public:
  typedef Action2 (T::*Function)();
  typedef oatpp::base::memory::Bench<T> Bench;
public:
  static Bench& getBench(){
    static thread_local Bench bench(512);
    return bench;
  }
public:
  static FunctionPtr castFunctionPtr(Function function){
    return static_cast<FunctionPtr>(function);
  }
public:
  
  virtual Action2 call(FunctionPtr ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }
  
  virtual void free() override {
    Coroutine<T>::getBench().free(static_cast<T*>(this));
  }
  
  template<typename C, typename ... Args>
  Action2 startCoroutine(const Action2& actionOnReturn, Args... args) {
    m_savedAction = actionOnReturn;
    C* coroutine = C::getBench().obtain(args...);
    return Action2(Action2::TYPE_COROUTINE, coroutine, nullptr);
  }
  
  Action2 yieldTo(Function function) const {
    return Action2(Action2::TYPE_YIELD_TO, nullptr, static_cast<FunctionPtr>(function));
  }
  
  const Action2& waitRetry() const {
    return Action2::_WAIT_RETRY;
  }
  
  const Action2& repeat() const {
    return Action2::_REPEAT;
  }
  
  const Action2& finish() const {
    return Action2::_FINISH;
  }
  
  const Action2& abort() const {
    return Action2::_ABORT;
  }
  
  Action2 error(const char* message) {
    return Action2(Error2(message));
  }
  
};
  
}}

#endif /* oatpp_async_Coroutine_hpp */
