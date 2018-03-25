/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "../collection/FastQueue.hpp"
#include "../base/memory/MemoryPool.hpp"
#include "../base/Environment.hpp"

namespace oatpp { namespace async {

class AbstractCoroutine; // FWD
class Processor; // FWD
  
class Error {
public:

  Error(const char* pMessage, bool pIsExceptionThrown = false)
    : message(pMessage)
    , isExceptionThrown(pIsExceptionThrown)
  {}
  
  const char* message;
  bool isExceptionThrown;
  
};
  
class Action {
  friend Processor;
  friend AbstractCoroutine;
public:
  typedef Action (AbstractCoroutine::*FunctionPtr)();
public:
  static constexpr const v_int32 TYPE_COROUTINE = 0;
  static constexpr const v_int32 TYPE_YIELD_TO = 1;
  static constexpr const v_int32 TYPE_WAIT_RETRY = 2;
  static constexpr const v_int32 TYPE_REPEAT = 3;
  static constexpr const v_int32 TYPE_FINISH = 4;
  static constexpr const v_int32 TYPE_ABORT = 5;
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
public:
  
  Action(v_int32 type,
          AbstractCoroutine* coroutine,
          FunctionPtr functionPtr)
    : m_type(type)
    , m_coroutine(coroutine)
    , m_functionPtr(functionPtr)
    , m_error(Error(nullptr))
  {}
  
  Action(const Error& error)
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
  friend Processor;
public:
  typedef Action Action;
  typedef Action (AbstractCoroutine::*FunctionPtr)();
private:
  AbstractCoroutine* _CP = this;
  FunctionPtr _FP = &AbstractCoroutine::act;
  AbstractCoroutine* _ref = nullptr;
  
  const Action& takeAction(const Action& action){
    
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
        
    };
    
    return action;
    
  }
  
private:
  AbstractCoroutine* m_parent = nullptr;
  Action m_savedAction = Action::_FINISH;
public:
  
  Action iterate() {
    return takeAction(_CP->call(_FP));
  };
  
  Action iterate(v_int32 numIterations) {
    Action action(Action::TYPE_FINISH, nullptr, nullptr);
    for(v_int32 i = 0; i < numIterations; i++) {
      action = takeAction(_CP->call(_FP));
      if(action.m_type == Action::TYPE_WAIT_RETRY || _CP == nullptr) {
        return action;
      }
    }
    return action;
  };
  
  virtual ~AbstractCoroutine(){
  }
  
  virtual Action act() = 0;
  virtual Action call(FunctionPtr ptr) = 0;
  virtual void free() = 0;
  
  template<typename C, typename ... Args>
  Action startCoroutine(const Action& actionOnReturn, Args... args) {
    m_savedAction = actionOnReturn;
    C* coroutine = C::getBench().obtain(args...);
    return Action(Action::TYPE_COROUTINE, coroutine, nullptr);
  }
  
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
  typedef Action (T::*Function)();
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
  
  virtual Action call(FunctionPtr ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }
  
  virtual void free() override {
    Coroutine<T>::getBench().free(static_cast<T*>(this));
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
  
}}

#endif /* oatpp_async_Coroutine_hpp */
