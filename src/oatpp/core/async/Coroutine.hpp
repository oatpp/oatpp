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

#include "./Error.hpp"
#include "oatpp/core/collection/FastQueue.hpp"
#include "oatpp/core/base/memory/MemoryPool.hpp"
#include "oatpp/core/base/Environment.hpp"

#include "oatpp/core/Types.hpp"

#include <exception>

namespace oatpp { namespace async {

class AbstractCoroutine; // FWD
class Processor; // FWD
class CoroutineStarter; // FWD

/**
 * Class Action represents an asynchronous action.
 */
class Action {
  friend Processor;
  friend AbstractCoroutine;
  friend CoroutineStarter;
public:
  typedef Action (AbstractCoroutine::*FunctionPtr)();
public:

  static constexpr const v_int32 TYPE_NONE = 0;

  /**
   * Indicate that Action is to start coroutine.
   */
  static constexpr const v_int32 TYPE_COROUTINE = 1;

  /**
   * Indicate that Action is to YIELD control to other method of Coroutine.
   */
  static constexpr const v_int32 TYPE_YIELD_TO = 2;

  /**
   * Indicate that Action is to WAIT and then RETRY call to current method of Coroutine.
   */
  static constexpr const v_int32 TYPE_WAIT_RETRY = 3;

  /**
   * Indicate that Action is to REPEAT call to current method of Coroutine.
   */
  static constexpr const v_int32 TYPE_REPEAT = 4;

  /**
   * Indicate that Action is to FINISH current Coroutine and return control to a caller-Coroutine.
   */
  static constexpr const v_int32 TYPE_FINISH = 5;

  /**
   * Indicate that Error occurred.
   */
  static constexpr const v_int32 TYPE_ERROR = 6;

private:
  union Data {
    FunctionPtr fptr;
    AbstractCoroutine* coroutine;
  };
private:
  mutable v_int32 m_type;
  Data m_data;
public:

  static Action clone(const Action& action);

  /**
   * Constructor. Create start-coroutine Action.
   * @param coroutine - pointer to &l:AbstractCoroutine;.
   */
  Action(AbstractCoroutine* coroutine);

  /**
   * Constructor. Create yield_to Action.
   * @param functionPtr - pointer to function.
   */
  Action(FunctionPtr functionPtr);

  /**
   * Create Action by type.
   * @param type - Action type.
   */
  Action(v_int32 type);

  /**
   * Deleted copy-constructor.
   */
  Action(const Action&) = delete;

  Action(Action&& other);

  /**
   * Non-virtual destructor.
   */
  ~Action();

  /*
   * Deleted copy-assignment operator.
   */
  Action& operator=(const Action&) = delete;

  /*
   * Move assignment operator.
   */
  Action& operator=(Action&& other);

  /**
   * Check if action is an error reporting action.
   * @return `true` if action is an error reporting action.
   */
  bool isError();

  /**
   * Get Action type.
   * @return - action type.
   */
  v_int32 getType();
  
};

/**
 * CoroutineStarter of Coroutine calls.
 */
class CoroutineStarter {
private:
  AbstractCoroutine* m_first;
  AbstractCoroutine* m_last;
public:

  /**
   * Constructor.
   * @param coroutine - coroutine.
   */
  CoroutineStarter(AbstractCoroutine* coroutine);

  /**
   * Deleted copy-constructor.
   */
  CoroutineStarter(const CoroutineStarter&) = delete;

  /**
   * Move constructor.
   * @param other - other starter.
   */
  CoroutineStarter(CoroutineStarter&& other);

  /**
   * Non-virtual destructor.
   */
  ~CoroutineStarter();

  /*
   * Deleted copy-assignment operator.
   */
  CoroutineStarter& operator=(const CoroutineStarter&) = delete;

  /*
   * Move assignment operator.
   */
  CoroutineStarter& operator=(CoroutineStarter&& other);

  /**
   * Set final starter action.
   * @param action - &l:Action;.
   * @return - &l:Action;.
   */
  Action next(Action&& action);

  /**
   * Add coroutine starter pipeline to this starter.
   * @param pipeline - pipeline to add.
   * @return - this pipeline.
   */
  CoroutineStarter& next(CoroutineStarter&& starter);

};

/**
 * Abstract Coroutine. Base class for Coroutines. It provides state management, coroutines stack management and error reporting functionality.
 */
class AbstractCoroutine {// : public oatpp::base::Countable {
  friend oatpp::collection::FastQueue<AbstractCoroutine>;
  friend Processor;
  friend CoroutineStarter;
public:
  /**
   * Convenience typedef for Action
   */
  typedef oatpp::async::Action Action;
  typedef oatpp::async::Error Error;
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
  static std::shared_ptr<const Error> ERROR_UNKNOWN;
private:
  AbstractCoroutine* _CP;
  FunctionPtr _FP;
  std::shared_ptr<const Error> _ERR;
  AbstractCoroutine* _ref;
private:
  AbstractCoroutine* m_parent;
  std::shared_ptr<const Error>* m_propagatedError;
protected:
  oatpp::async::Action m_parentReturnAction;
private:
  Action takeAction(Action&& action);
public:

  /**
   * Constructor.
   */
  AbstractCoroutine();

  /**
   * Make one Coroutine iteration.
   * @return - control Action.
   */
  Action iterate();

  /**
   * Virtual Destructor
   */
  virtual ~AbstractCoroutine() = default;

  /**
   * Entrypoint of Coroutine.
   * @return - Action
   */
  virtual Action act() = 0;

  /**
   * Call function of Coroutine specified by ptr.<br>
   * This method is called from iterate().<br>
   * Coroutine keeps track of function ptr and calls corresponding function on each iteration.
   * When Coroutine starts, function ptr points to act().
   * @param ptr - pointer of the function to call.
   * @return - Action.
   */
  virtual Action call(FunctionPtr ptr) = 0;

  virtual MemberCaller getMemberCaller() const = 0;

  /**
   * Default implementation of handleError(error) function.
   * User may override this function in order to handle errors.
   * @param error - error.
   * @return - Action. If handleError function returns Error,
   * current coroutine will finish, return control to caller coroutine and handleError is called for caller coroutine.
   */
  virtual Action handleError(const std::shared_ptr<const Error>& error);
  
  template<typename ...Args>
  Action callWithParams(FunctionPtr ptr, Args... args) {
    return getMemberCaller().call<Action>(ptr, args...);
  }

  /**
   * Check if coroutine is finished
   * @return - true if finished
   */
  bool finished() const;

  /**
   * Get parent coroutine
   * @return - pointer to a parent coroutine
   */
  AbstractCoroutine* getParent() const;

  /**
   * Convenience method to generate error reporting Action.
   * @param message - error message.
   * @return - error reporting Action.
   */
  Action error(const std::shared_ptr<const Error>& error);

  template<class E, typename ... Args>
  Action error(Args... args) {
    *m_propagatedError = std::make_shared<E>(args...);
    return Action(Action::TYPE_ERROR);
  }
  
};

/**
 * Coroutine template. <br>
 * Example usage:<br>
 * `class MyCoroutine : public oatpp::async::Coroutine<MyCoroutine>`
 * @tparam T - child class type
 */
template<class T>
class Coroutine : public AbstractCoroutine {
public:
  typedef Action (T::*Function)();
public:

  static void* operator new(std::size_t sz) {
    return ::operator new(sz);
  }

  static void operator delete(void* ptr, std::size_t sz) {
    ::operator delete(ptr);
  }

public:

  /**
   * Create coroutine and return it's starter
   * @tparam ConstructorArgs - coroutine constructor arguments.
   * @param args - actual coroutine constructor arguments.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  template<typename ...ConstructorArgs>
  static CoroutineStarter start(ConstructorArgs... args) {
    return new T(args...);
  }

  /**
   * Call function of Coroutine specified by ptr. <br>
   * Overridden `AbstractCoroutine::call()` method.
   * @param ptr - pointer of the function to call.
   * @return - Action.
   */
  Action call(FunctionPtr ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }
  
  MemberCaller getMemberCaller() const override {
    return MemberCaller((void*) this);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_YIELD_TO`.
   * @param function - pointer to function.
   * @return - yield Action.
   */
  Action yieldTo(Function function) const {
    return Action(static_cast<FunctionPtr>(function));
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_WAIT_RETRY`.
   * @return - WAIT_RETRY Action.
   */
  Action waitRetry() const {
    return Action::TYPE_WAIT_RETRY;
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_REPEAT`.
   * @return - repeat Action.
   */
  Action repeat() const {
    return Action::TYPE_REPEAT;
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_FINISH`.
   * @return - finish Action.
   */
  Action finish() const {
    return Action::TYPE_FINISH;
  }
  
};

class AbstractCoroutineWithResult : public AbstractCoroutine {
protected:
  FunctionPtr m_callback;
public:

  /**
   * Class representing Coroutine call for result;
   */
  template<typename ...Args >
  class StarterForResult {
  public:

    template <class C>
    using
    Callback = Action (C::*)(Args...);

  private:
    AbstractCoroutineWithResult* m_coroutine;
  public:
    /**
     * Constructor.
     * @param coroutine - coroutine.
     */
    StarterForResult(AbstractCoroutineWithResult* coroutine)
      : m_coroutine(coroutine)
    {}

    /**
     * Deleted copy-constructor.
     */
    StarterForResult(const StarterForResult&) = delete;

    /**
     * Move constructor.
     * @param other - other pipeline.
     */
    StarterForResult(StarterForResult&& other)
      : m_coroutine(other.m_coroutine)
    {
      other.m_coroutine = nullptr;
    }

    /**
     * Non-virtual destructor.
     */
    ~StarterForResult() {
      if(m_coroutine != nullptr) {
        delete m_coroutine;
      }
    }

    /*
     * Deleted copy-assignment operator.
     */
    StarterForResult& operator=(const StarterForResult&) = delete;

    /*
     * Move assignment operator.
     */
    StarterForResult& operator=(StarterForResult&& other) {
      m_coroutine = other.m_coroutine;
      other.m_coroutine = nullptr;
      return *this;
    }

    template<class C>
    Action callbackTo(Callback<C> callback) {
      if(m_coroutine == nullptr) {
        throw std::runtime_error("[oatpp::async::AbstractCoroutineWithResult::StarterForResult::callbackTo()]: Error. Coroutine is null.");
      }
      m_coroutine->m_callback = (FunctionPtr)(callback);
      Action result = m_coroutine;
      m_coroutine = nullptr;
      return std::move(result);

    }

  };

};

template <typename ...Args>
using
CoroutineStarterForResult = typename AbstractCoroutineWithResult::StarterForResult<Args...>;

/**
 * Coroutine with result template. <br>
 * Example usage:<br>
 * `class CoroutineWithResult : public oatpp::async::CoroutineWithResult<CoroutineWithResult, const char*>`
 * @tparam T - child class type.
 * @tparam Args - return argumet type.
 */
template<class T, typename ...Args>
class CoroutineWithResult : public AbstractCoroutineWithResult {
  friend AbstractCoroutine;
public:
  typedef Action (T::*Function)();
public:

  static void* operator new(std::size_t sz) {
    return ::operator new(sz);
  }

  static void operator delete(void* ptr, std::size_t sz) {
    ::operator delete(ptr);
  }
public:

  /**
   * Call coroutine for result.
   * @tparam ConstructorArgs - coroutine consrtructor arguments.
   * @param args - actual constructor arguments.
   * @return - &l:CoroutineWithResult::CoroutineWithResult;.
   */
  template<typename ...ConstructorArgs>
  static CoroutineStarterForResult<Args...> startForResult(ConstructorArgs... args) {
    return new T(args...);
  }

  /**
   * Call function of Coroutine specified by ptr. <br>
   * Overridden AbstractCoroutine::call() method.
   * @param ptr - pointer of the function to call.
   * @return - Action.
   */
  virtual Action call(FunctionPtr ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }
  
  MemberCaller getMemberCaller() const override {
    return MemberCaller((void*) this);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_YIELD_TO`.
   * @param function - pointer to function.
   * @return - yield Action.
   */
  Action yieldTo(Function function) const {
    return Action(static_cast<FunctionPtr>(function));
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_WAIT_RETRY`.
   * @return - WAIT_RETRY Action.
   */
  Action waitRetry() const {
    return Action::TYPE_WAIT_RETRY;
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_REPEAT`.
   * @return - repeat Action.
   */
  Action repeat() const {
    return Action::TYPE_REPEAT;
  }

  /**
   * Deprecated. <br>
   * Call caller's Callback passing returned value, and generate Action of `type == Action::TYPE_FINISH`.
   * @param args - argumets to be passed to callback.
   * @return - finish Action.
   */
  Action _return(Args... args) {
    m_parentReturnAction = getParent()->callWithParams(m_callback, args...);
    return Action::TYPE_FINISH;
  }
  
};

}}

#endif /* oatpp_async_Coroutine_hpp */
