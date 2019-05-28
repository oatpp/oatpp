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

#include "oatpp/core/data/IODefinitions.hpp"

#include "oatpp/core/collection/FastQueue.hpp"
#include "oatpp/core/base/memory/MemoryPool.hpp"
#include "oatpp/core/base/Environment.hpp"

#include "oatpp/core/Types.hpp"

#include <chrono>
#include <exception>

namespace oatpp { namespace async {

class AbstractCoroutine; // FWD
class Processor; // FWD
class CoroutineStarter; // FWD
class CoroutineWaitList; // FWD

namespace worker {
  class Worker; // FWD
}

/**
 * Class Action represents an asynchronous action.
 */
class Action {
  friend Processor;
  friend AbstractCoroutine;
  friend CoroutineStarter;
  friend worker::Worker;
public:
  typedef Action (AbstractCoroutine::*FunctionPtr)();
public:

  /**
   * None - invalid Action.
   */
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
   * Indicate that Action is to REPEAT call to current method of Coroutine.
   */
  static constexpr const v_int32 TYPE_REPEAT = 3;

  /**
   * Indicate that Action is to WAIT for some time and then REPEAT call to current method of Coroutine.
   */
  static constexpr const v_int32 TYPE_WAIT_REPEAT = 4;

  /**
   * Indicate that Action is waiting for IO and should be assigned to corresponding worker.
   */
  static constexpr const v_int32 TYPE_IO_WAIT = 5;

  /**
   * Indicate that Action is to repeat previously successful I/O operation.
   */
  static constexpr const v_int32 TYPE_IO_REPEAT = 6;

  /**
   * Indicate that Action is to FINISH current Coroutine and return control to a caller-Coroutine.
   */
  static constexpr const v_int32 TYPE_FINISH = 7;

  /**
   * Indicate that Error occurred.
   */
  static constexpr const v_int32 TYPE_ERROR = 8;

  /**
   * Indicate that coroutine should be put on a wait-list provided.
   */
  static constexpr const v_int32 TYPE_WAIT_LIST = 9;

public:

  /**
   * Event type qualifier for Actions of type &l:Action::TYPE_IO_WAIT;, &l:Action::TYPE_IO_REPEAT;.
   */
  enum IOEventType : v_int32 {
    /**
     * IO event type READ.
     */
    IO_EVENT_READ = 256,

    /**
     * IO event type WRITE.
     */
    IO_EVENT_WRITE = 512

  };

  /**
   * Convenience I/O Action Code.
   * This code is applicable for Action of type TYPE_IO_WAIT only.
   */
  static constexpr const v_int32 CODE_IO_WAIT_READ = TYPE_IO_WAIT | IOEventType::IO_EVENT_READ;

  /**
   * Convenience I/O Action Code.
   * This code is applicable for Action of type TYPE_IO_WAIT only.
   */
  static constexpr const v_int32 CODE_IO_WAIT_WRITE = TYPE_IO_WAIT | IOEventType::IO_EVENT_WRITE;

  /**
   * Convenience I/O Action Code.
   * This code is applicable for Action of type TYPE_IO_WAIT only.
   */
  static constexpr const v_int32 CODE_IO_WAIT_RESCHEDULE = TYPE_IO_WAIT | IOEventType::IO_EVENT_READ | IOEventType::IO_EVENT_WRITE;

  /**
   * Convenience I/O Action Code.
   * This code is applicable for Action of type TYPE_IO_REPEAT only.
   */
  static constexpr const v_int32 CODE_IO_REPEAT_READ = TYPE_IO_REPEAT | IOEventType::IO_EVENT_READ;

  /**
   * Convenience I/O Action Code.
   * This code is applicable for Action of type TYPE_IO_REPEAT only.
   */
  static constexpr const v_int32 CODE_IO_REPEAT_WRITE = TYPE_IO_REPEAT | IOEventType::IO_EVENT_WRITE;

  /**
   * Convenience I/O Action Code.
   * This code is applicable for Action of type TYPE_IO_REPEAT only.
   */
  static constexpr const v_int32 CODE_IO_REPEAT_RESCHEDULE = TYPE_IO_REPEAT | IOEventType::IO_EVENT_READ | IOEventType::IO_EVENT_WRITE;

private:

  struct IOData {
    oatpp::data::v_io_handle ioHandle;
    IOEventType ioEventType;
  };

private:
  union Data {
    FunctionPtr fptr;
    AbstractCoroutine* coroutine;
    IOData ioData;
    v_int64 timePointMicroseconds;
    CoroutineWaitList* waitList;
  };
private:
  mutable v_int32 m_type;
  Data m_data;
protected:
  /*
   * Create Action by type.
   * @param type - Action type.
   */
  Action(v_int32 type);
public:

  /**
   * Clone action.
   * @param action - action to clone.
   * @return - cloned action.
   */
  static Action clone(const Action& action);

  /**
   * Create action of specific type
   * @param type
   * @return
   */
  static Action createActionByType(v_int32 type);

  /**
   * Create TYPE_IO_WAIT Action
   * @param ioHandle - &id:oatpp::data::v_io_handle;.
   * @return - Action.
   */
  static Action createIOWaitAction(data::v_io_handle ioHandle, IOEventType ioEventType);

  /**
   * Create TYPE_IO_REPEAT Action
   * @param ioHandle - &id:oatpp::data::v_io_handle;.
   * @return - Action.
   */
  static Action createIORepeatAction(data::v_io_handle ioHandle, IOEventType ioEventType);

  /**
   * Create TYPE_WAIT_REPEAT Action.
   * @param timePointMicroseconds - time since epoch.
   * @return - Action.
   */
  static Action createWaitRepeatAction(v_int64 timePointMicroseconds);

  /**
   * Create TYPE_WAIT_LIST Action.
   * @param waitList - wait-list to put coroutine on.
   * @return - Action.
   */
  static Action createWaitListAction(CoroutineWaitList* waitList);

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
   * Deleted copy-constructor.
   */
  Action(const Action&) = delete;

  /**
   * Move-constructor.
   * @param other
   */
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
  bool isError() const;

  /**
   * Get Action type.
   * @return - action type.
   */
  v_int32 getType() const;

  /**
   * Get microseconds tick when timer should call coroutine again.
   * This method returns meaningful value only if Action is TYPE_WAIT_REPEAT.
   * @return - microseconds tick.
   */
  v_int64 getTimePointMicroseconds() const;

  /**
   * Get I/O handle which is passed with this action to I/O worker.
   * This method returns meaningful value only if Action is TYPE_IO_WAIT or TYPE_IO_REPEAT.
   * @return - &id:oatpp::data::v_io_handle;.
   */
  oatpp::data::v_io_handle getIOHandle() const;

  /**
   * This method returns meaningful value only if Action is TYPE_IO_WAIT or TYPE_IO_REPEAT.
   * @return - should return one of
   */
  IOEventType getIOEventType() const;

  /**
   * Convenience method to get I/O Event code.
   * @return - `getType() | getIOEventType()`.
   */
  v_int32 getIOEventCode() const;

  
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
   * Pipeline coroutine starter.
   * @param starter - starter to add.
   * @return - this starter.
   */
  CoroutineStarter& next(CoroutineStarter&& starter);

};

/**
 * Abstract Coroutine. Base class for Coroutines. It provides state management, coroutines stack management and error reporting functionality.
 */
class AbstractCoroutine : public oatpp::base::Countable {
  friend oatpp::collection::FastQueue<AbstractCoroutine>;
  friend Processor;
  friend CoroutineStarter;
  friend worker::Worker;
  friend CoroutineWaitList;
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
  Processor* _PP;
  oatpp::async::Action _SCH_A;
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

  Action propagateError() {
    return Action(Action::TYPE_ERROR);
  }

  /**
   * Convenience method to generate error reporting Action.
   * @tparam E - Error class type.
   * @tparam Args - Error constructor arguments.
   * @param args - actual error constructor arguments.
   * @return - error reporting &id:oatpp::async::Action;.
   */
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
   * Convenience method to generate Action of `type == Action::TYPE_REPEAT`.
   * @return - repeat Action.
   */
  Action repeat() const {
    return Action::createActionByType(Action::TYPE_REPEAT);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_WAIT_REPEAT`.
   * @return - TYPE_WAIT_REPEAT Action.
   */
  Action waitRepeat(const std::chrono::duration<v_int64, std::micro>& timeout) const {
    auto startTime = std::chrono::system_clock::now();
    auto end = startTime + timeout;
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch());
    return Action::createWaitRepeatAction(ms.count());
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_IO_WAIT`.
   * @return - TYPE_WAIT_FOR_IO Action.
   */
  Action ioWait(data::v_io_handle ioHandle, Action::IOEventType ioEventType) const {
    return Action::createIOWaitAction(ioHandle, ioEventType);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_IO_WAIT`.
   * @return - TYPE_IO_REPEAT Action.
   */
  Action ioRepeat(data::v_io_handle ioHandle, Action::IOEventType ioEventType) const {
    return Action::createIORepeatAction(ioHandle, ioEventType);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_FINISH`.
   * @return - finish Action.
   */
  Action finish() const {
    return Action::createActionByType(Action::TYPE_FINISH);
  }
  
};

/**
 * Abstract coroutine with result.
 */
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
     * @param other - other starter.
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
      delete m_coroutine;
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

    /**
     * Set callback for result and return coroutine starting Action.
     * @tparam C - caller coroutine type.
     * @param callback - callback to obtain result.
     * @return - &id:oatpp::async::Action;.
     */
    template<class C>
    Action callbackTo(Callback<C> callback) {
      if(m_coroutine == nullptr) {
        throw std::runtime_error("[oatpp::async::AbstractCoroutineWithResult::StarterForResult::callbackTo()]: Error. Coroutine is null.");
      }
      m_coroutine->m_callback = (FunctionPtr)(callback);
      Action result = m_coroutine;
      m_coroutine = nullptr;
      return result;

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
   * @return - &l:AbstractCoroutineWithResult::StarterForResult;.
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
   * Convenience method to generate Action of `type == Action::TYPE_WAIT_REPEAT`.
   * @return - TYPE_WAIT_REPEAT Action.
   */
  Action waitRepeat(const std::chrono::duration<v_int64, std::micro>& timeout) const {
    auto startTime = std::chrono::system_clock::now();
    auto end = startTime + timeout;
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch());
    return Action::createWaitRepeatAction(ms.count());
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_IO_WAIT`.
   * @return - TYPE_WAIT_FOR_IO Action.
   */
  Action ioWait(data::v_io_handle ioHandle, Action::IOEventType ioEventType) const {
    return Action::createIOWaitAction(ioHandle, ioEventType);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_IO_WAIT`.
   * @return - TYPE_IO_REPEAT Action.
   */
  Action ioRepeat(data::v_io_handle ioHandle, Action::IOEventType ioEventType) const {
    return Action::createIORepeatAction(ioHandle, ioEventType);
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_REPEAT`.
   * @return - repeat Action.
   */
  Action repeat() const {
    return Action::createActionByType(Action::TYPE_REPEAT);
  }

  /**
   * Call caller's Callback passing returned value, and generate Action of `type == Action::TYPE_FINISH`.
   * @param args - argumets to be passed to callback.
   * @return - finish Action.
   */
  Action _return(Args... args) {
    m_parentReturnAction = getParent()->callWithParams(m_callback, args...);
    return Action::createActionByType(Action::TYPE_FINISH);
  }
  
};

}}

#endif /* oatpp_async_Coroutine_hpp */
