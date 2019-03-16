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

#ifndef oatpp_async_Coroutine_hpp
#define oatpp_async_Coroutine_hpp

#include "oatpp/core/collection/FastQueue.hpp"
#include "oatpp/core/base/memory/MemoryPool.hpp"
#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace async {

class AbstractCoroutine; // FWD
class Processor; // FWD

/**
 * Class to hold and communicate errors between Coroutines
 */
class Error {
public:

  /**
   * Constructor.
   * @param pMessage - Error message.
   * @param pIsExceptionThrown - Indicate that this error is a result of thrown exception.
   */
  Error(const char* pMessage, bool pIsExceptionThrown = false);

  /**
   * Error message
   */
  const char* message;

  /**
   * Indicates that this error is a result of thrown exception. Re-throw if true to catch original exception.
   */
  bool isExceptionThrown;
  
};

/**
 * Class Action represents an asynchronous action.
 */
class Action {
  friend Processor;
  friend AbstractCoroutine;
public:
  typedef Action (AbstractCoroutine::*FunctionPtr)();
public:
  /**
   * Indicate that Action is to start coroutine. Value = 0.
   */
  static constexpr const v_int32 TYPE_COROUTINE = 0;

  /**
   * Indicate that Action is to YIELD control to other method of Coroutine. Value = 1.
   */
  static constexpr const v_int32 TYPE_YIELD_TO = 1;

  /**
   * Indicate that Action is to WAIT and then RETRY call to current method of Coroutine. Value = 2.
   */
  static constexpr const v_int32 TYPE_WAIT_RETRY = 2;

  /**
   * Indicate that Action is to REPEAT call to current method of Coroutine. Value = 3.
   */
  static constexpr const v_int32 TYPE_REPEAT = 3;

  /**
   * Indicate that Action is to FINISH current Coroutine and return control to a caller-Coroutine. Value = 4.
   */
  static constexpr const v_int32 TYPE_FINISH = 4;

  /**
   * Deprecated
   */
  static constexpr const v_int32 TYPE_ABORT = 5;

  /**
   * Indicate that Error occurred
   */
  static constexpr const v_int32 TYPE_ERROR = 6;
public:
  static const Action _WAIT_RETRY;
  static const Action _REPEAT;
  static const Action _FINISH;
  static const Action _ABORT;
private:
  v_int32 m_type;
  AbstractCoroutine* m_coroutine;
  FunctionPtr m_functionPtr;
  Error m_error;
protected:
  void free();
public:
  
  Action(v_int32 type,
         AbstractCoroutine* coroutine,
         FunctionPtr functionPtr);
  
  Action(const Error& error);
  bool isError();
  
};
  
class AbstractCoroutine {
  friend oatpp::collection::FastQueue<AbstractCoroutine>;
  friend Processor;
public:
  typedef oatpp::async::Action Action;
  typedef Action (AbstractCoroutine::*FunctionPtr)();
public:
  
  class MemberCaller {
  private:
    void* m_objectPtr;
  public:
    
    MemberCaller(void* objectPtr)
      : m_objectPtr(objectPtr)
    {}
    
    template<typename ReturnType, typename T, typename ...Args>
    ReturnType call(ReturnType (T::*f)(), Args... args){
      MemberCaller* caller = static_cast<MemberCaller*>(m_objectPtr);
      return (caller->*reinterpret_cast<ReturnType (MemberCaller::*)(Args...)>(f))(args...);
    }
    
  };
  
private:
  AbstractCoroutine* _CP = this;
  FunctionPtr _FP = &AbstractCoroutine::act;
  AbstractCoroutine* _ref = nullptr;
  
  Action takeAction(const Action& action){
    
    switch (action.m_type) {

      case Action::TYPE_COROUTINE:
          action.m_coroutine->m_parent = _CP;
          _CP = action.m_coroutine;
          _FP = action.m_coroutine->_FP;
        break;
        
      case Action::TYPE_FINISH:
          if(_CP == this) {
            _CP = nullptr;
            return action;
          }
          {
            AbstractCoroutine* savedCP = _CP;
            _CP = _CP->m_parent;
            _FP = nullptr;
            /* Please note that savedCP->m_parentReturnAction should not be "REPEAT nor WAIT_RETRY" */
            /* as funtion pointer (FP) is invalidated */
            Action a = takeAction(savedCP->m_parentReturnAction);
            savedCP->m_parentReturnAction.m_coroutine = nullptr;
            savedCP->free();
            return a;
          }
        break;
        
      case Action::TYPE_YIELD_TO:
          _FP = action.m_functionPtr;
        break;
        
      case Action::TYPE_ABORT:
          while (_CP != this) {
            _CP->free();
            _CP = _CP->m_parent;
          }
          _CP = nullptr;
        break;
        
      case Action::TYPE_ERROR:
        Action a = action;
        do {
          a = _CP->handleError(a.m_error);
          if(a.m_type == Action::TYPE_ERROR) {
            if(_CP == this) {
              _CP = nullptr;
              return a;
            } else {
              _CP->free();
              _CP = _CP->m_parent;
            }
          } else {
            a = takeAction(a);
          }
        } while (a.m_type == Action::TYPE_ERROR && _CP != nullptr);
        return a;
        break;
        
    };
    
    return action;
    
  }
  
private:
  AbstractCoroutine* m_parent = nullptr;
protected:
  Action m_parentReturnAction = Action::_FINISH;
public:
  
  Action iterate() {
    try {
      return takeAction(_CP->call(_FP));
    } catch (...) {
      return takeAction(Action(Error("Exception", true)));
    }
  };
  
  /*Action iterate(v_int32 numIterations) {
    Action action(Action::TYPE_FINISH, nullptr, nullptr);
    for(v_int32 i = 0; i < numIterations; i++) {
      action = takeAction(_CP->call(_FP));
      if(action.m_type == Action::TYPE_WAIT_RETRY || _CP == nullptr) {
        return action;
      }
    }
    return action;
  };*/
  
  virtual ~AbstractCoroutine(){
    m_parentReturnAction.free();
  }
  
  virtual Action act() = 0;
  virtual Action call(FunctionPtr ptr) = 0;
  
  /**
   *  Internal function. Should free Coroutine on MemoryPool/Bench
   *  free() also calls virtual destructor:
   *  Coroutine::free() --> Bench::free(Coroutine* coroutine) { ... coroutine->~Coroutine(); ... }
   */
  virtual void free() = 0;
  virtual MemberCaller getMemberCaller() const = 0;
  
  virtual Action handleError(const Error& error) {
    return error;
  }
  
  template<typename ...Args>
  Action callWithParams(FunctionPtr ptr, Args... args) {
    return getMemberCaller().call<Action>(ptr, args...);
  }
  
  template<typename C, typename ... Args>
  Action startCoroutine(const Action& actionOnReturn, Args... args) {
    C* coroutine = C::getBench().obtain(args...);
    coroutine->m_parentReturnAction = actionOnReturn;
    return Action(Action::TYPE_COROUTINE, coroutine, nullptr);
  }
  
  template<typename CoroutineType, typename ParentCoroutineType, typename ... CallbackArgs, typename ...Args>
  Action startCoroutineForResult(Action (ParentCoroutineType::*function)(CallbackArgs...), Args... args) {
    CoroutineType* coroutine = CoroutineType::getBench().obtain(args...);
    coroutine->m_callback = reinterpret_cast<FunctionPtr>(function);
    return Action(Action::TYPE_COROUTINE, coroutine, nullptr);
  }
  
  bool finished() const {
    return _CP == nullptr;
  }
  
  AbstractCoroutine* getParent() const {
    return m_parent;
  }
  
};
 
template<class T>
class Coroutine : public AbstractCoroutine {
public:
  typedef Action (T::*Function)();
  typedef oatpp::base::memory::Bench<T> Bench;
public:
  static Bench& getBench(){
    static thread_local Bench bench(512);
    return bench;
  }
public:
  
  Action call(FunctionPtr ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }
  
  void free() override {
    Coroutine<T>::getBench().free(static_cast<T*>(this));
  }
  
  MemberCaller getMemberCaller() const override {
    return MemberCaller((void*) this);
  }
  
  Action yieldTo(Function function) const {
    return Action(Action::TYPE_YIELD_TO, nullptr, static_cast<FunctionPtr>(function));
  }
  
  const Action& waitRetry() const {
    return Action::_WAIT_RETRY;
  }
  
  const Action& repeat() const {
    return Action::_REPEAT;
  }
  
  const Action& finish() const {
    return Action::_FINISH;
  }
  
  const Action& abort() const {
    return Action::_ABORT;
  }
  
  Action error(const char* message) {
    return Action(Error(message));
  }
  
};
  
template<class T, typename ...Args>
class CoroutineWithResult : public AbstractCoroutine {
  friend AbstractCoroutine;
public:
  typedef Action (T::*Function)();
  typedef oatpp::base::memory::Bench<T> Bench;
public:
  static Bench& getBench(){
    static thread_local Bench bench(512);
    return bench;
  }
private:
  FunctionPtr m_callback;
public:
  
  virtual Action call(FunctionPtr ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }
  
  virtual void free() override {
    CoroutineWithResult<T, Args...>::getBench().free(static_cast<T*>(this));
  }
  
  MemberCaller getMemberCaller() const override {
    return MemberCaller((void*) this);
  }
  
  Action yieldTo(Function function) const {
    return Action(Action::TYPE_YIELD_TO, nullptr, static_cast<FunctionPtr>(function));
  }
  
  const Action& waitRetry() const {
    return Action::_WAIT_RETRY;
  }
  
  const Action& repeat() const {
    return Action::_REPEAT;
  }
  
  const Action& _return(Args... args) {
    m_parentReturnAction = getParent()->callWithParams(m_callback, args...);
    return Action::_FINISH;
  }
  
  const Action& abort() const {
    return Action::_ABORT;
  }
  
  Action error(const char* message) {
    return Action(Error(message));
  }
  
};
  
}}

#endif /* oatpp_async_Coroutine_hpp */
