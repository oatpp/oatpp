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
#include "../base/Environment.hpp"

namespace oatpp { namespace async {

class AbstractCoroutine; // FWD
  
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
  friend AbstractCoroutine;
public:
  typedef Action2 (AbstractCoroutine::*FunctionPtr)();
public:
  static const v_int32 TYPE_COROUTINE;
  static const v_int32 TYPE_YIELD_TO;
  static const v_int32 TYPE_WAIT_RETRY;
  static const v_int32 TYPE_REPEAT;
  static const v_int32 TYPE_FINISH;
  static const v_int32 TYPE_ABORT;
  static const v_int32 TYPE_ERROR;
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
public:
  typedef Action2 Action2;
  typedef Action2 (AbstractCoroutine::*FunctionPtr)();
private:
  AbstractCoroutine* _CP = this;
  FunctionPtr _FP = &AbstractCoroutine::act;
  AbstractCoroutine* _ref = nullptr;
  
  const Action2& takeAction(const Action2& action){
    
    if(action.m_type == Action2::TYPE_COROUTINE) {
      action.m_coroutine->m_parent = _CP;
      _CP = action.m_coroutine;
      _CP->m_returnFP = action.m_functionPtr;
      _FP = action.m_coroutine->_FP;
    } else if(action.m_type == Action2::TYPE_FINISH) {
      do {
      _FP = _CP->m_returnFP;
      _CP->free();
      _CP = _CP->m_parent; // Should be fine here. As free() - return of the pointer to Bench. (Memory not changed)
      } while (_FP == nullptr && _CP != nullptr);
    } else if(action.m_type == Action2::TYPE_YIELD_TO) {
      _FP = action.m_functionPtr;
    } else if(action.m_type == Action2::TYPE_ABORT) {
      while (_CP != nullptr) {
        _CP->free();
        _CP = _CP->m_parent;
      }
    }
    
    return action;
    
  }
  
private:
  AbstractCoroutine* m_parent = nullptr;
  FunctionPtr m_returnFP = nullptr; // should be set by processor on startCoroutine
public:
  
  Action2 iterate() {
    //try {
      return takeAction(_CP->call(_FP));
    //} catch (...) {
      
    //}
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
  typedef oatpp::collection::Bench<T> Bench;
public:
  static Bench& getBench(){
    static thread_local Bench bench(1024);
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
  
  template<typename C, typename F, typename ... Args>
  Action2 startCoroutine(F returnToFunction, Args... args) {
    C* coroutine = C::getBench().obtain(args...);
    return Action2(Action2::TYPE_COROUTINE, coroutine, static_cast<FunctionPtr>(returnToFunction));
  }
  
  Action2 yieldTo(Function function) {
    return Action2(Action2::TYPE_YIELD_TO, nullptr, static_cast<FunctionPtr>(function));
  }
  
  Action2 waitRetry() {
    return Action2(Action2::TYPE_WAIT_RETRY, nullptr, nullptr);
  }
  
  Action2 repeat() {
    return Action2(Action2::TYPE_REPEAT, nullptr, nullptr);
  }
  
  Action2 finish() {
    return Action2(Action2::TYPE_FINISH, nullptr, nullptr);
  }
  
  Action2 abort() {
    return Action2(Action2::TYPE_ABORT, nullptr, nullptr);
  }
  
  Action2 error(const char* message) {
    return Action2(Error2(message));
  }
  
};
  
}}

#endif /* oatpp_async_Coroutine_hpp */
