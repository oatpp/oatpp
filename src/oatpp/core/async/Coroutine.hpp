/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
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

#include "oatpp/core/async/utils/FastQueue.hpp"

#include "oatpp/core/IODefinitions.hpp"
#include "oatpp/core/base/Environment.hpp"

#include "oatpp/core/Types.hpp"

#include <chrono>
#include <exception>

namespace oatpp { namespace async {

class CoroutineHandle; // FWD
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
  friend CoroutineHandle;
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

  /**
   * Indicate that coroutine should be put on a wait-list provided with a timeout.
   */
  static constexpr const v_int32 TYPE_WAIT_LIST_WITH_TIMEOUT = 10;

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
    oatpp::v_io_handle ioHandle;
    IOEventType ioEventType;
  };

  struct WaitListWithTimeout {
    CoroutineWaitList* waitList;
    v_int64 timeoutTimeSinceEpochMS;
  };

private:
  union Data {
    FunctionPtr fptr;
    AbstractCoroutine* coroutine;
    Error* error;
    IOData ioData;
    v_int64 timePointMicroseconds;
    CoroutineWaitList* waitList;
    WaitListWithTimeout waitListWithTimeout;
  };
private:
  mutable v_int32 m_type;
  Data m_data;
private:
  void free();
protected:
  /*
   * Create Action by type.
   * @param type - Action type.
   */
  Action(v_int32 type);
public:

  /**
   * Default constructor.
   */
  Action();

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
   * @param ioHandle - &id:oatpp::v_io_handle;.
   * @return - Action.
   */
  static Action createIOWaitAction(v_io_handle ioHandle, IOEventType ioEventType);

  /**
   * Create TYPE_IO_REPEAT Action
   * @param ioHandle - &id:oatpp::v_io_handle;.
   * @return - Action.
   */
  static Action createIORepeatAction(v_io_handle ioHandle, IOEventType ioEventType);

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
   * Create TYPE_WAIT_LIST_WITH_TIMEOUT Action.
   * @param waitList - wait-list to put coroutine on.
   * @param timeout - latest time point at which the coroutine should be continued.
   * @return - Action.
   */
  static Action createWaitListActionWithTimeout(CoroutineWaitList* waitList, const std::chrono::steady_clock::time_point& timeout);

  /**
   * Constructor. Create start-coroutine Action.
   * @param coroutine - pointer to &l:AbstractCoroutine;.
   */
  Action(AbstractCoroutine* coroutine);

  /**
   * Constructor. Create yield_to Action.
   * @param functionPtr - pointer to function.
   */
  Action(const FunctionPtr& functionPtr);

  /**
   * Constructor. Create Error Action.
   * @param error - pointer to &id:oatpp::async::Error;.
   */
  Action(Error* error);

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
   * Check if action is of TYPE_NONE.
   * @return
   */
  bool isNone() const;

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
   * @return - &id:oatpp::v_io_handle;.
   */
  oatpp::v_io_handle getIOHandle() const;

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

private:

  void freeCoroutines();

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
 * This class manages coroutines processing state and a chain of coroutine calls.
 */
class CoroutineHandle : public oatpp::base::Countable {
  friend utils::FastQueue<CoroutineHandle>;
  friend Processor;
  friend worker::Worker;
  friend CoroutineWaitList;
public:
  typedef oatpp::async::Action Action;
  typedef oatpp::async::Error Error;
  typedef Action (AbstractCoroutine::*FunctionPtr)();
private:
  Processor* _PP;
  AbstractCoroutine* _CP;
  FunctionPtr _FP;
  oatpp::async::Action _SCH_A;
  CoroutineHandle* _ref;
public:

  CoroutineHandle(Processor* processor, AbstractCoroutine* rootCoroutine);
  ~CoroutineHandle();

  Action takeAction(Action&& action);
  Action iterate();
  Action iterateAndTakeAction();

  bool finished() const;

};

/**
 * Abstract Coroutine. Base class for Coroutines. It provides state management, coroutines stack management and error reporting functionality.
 */
class AbstractCoroutine : public oatpp::base::Countable {
  friend CoroutineStarter;
  friend CoroutineHandle;
public:
  /**
   * Convenience typedef for Action
   */
  typedef oatpp::async::Action Action;
  typedef oatpp::async::Error Error;
  typedef Action (AbstractCoroutine::*FunctionPtr)();
public:

  template<typename ...Args>
  class AbstractMemberCaller {
  public:
    virtual ~AbstractMemberCaller() = default;
    virtual Action call(AbstractCoroutine* coroutine, const Args&... args) = 0;
  };

  template<typename T, typename ...Args>
  class MemberCaller : public AbstractMemberCaller<Args...> {
  public:
    typedef Action (T::*Func)(const Args&...);
  private:
    Func m_func;
  public:

    MemberCaller(Func func)
      : m_func(func)
    {}

    Action call(AbstractCoroutine* coroutine, const Args&... args) override {
      T* _this = static_cast<T*>(coroutine);
      return (_this->*m_func)(args...);
    }

  };

  template<typename T, typename ...Args>
  static std::unique_ptr<AbstractMemberCaller<Args...>> createMemberCaller(Action (T::*func)(Args...)) {
    return std::unique_ptr<AbstractMemberCaller<Args...>>(new MemberCaller<T, Args...>(func));
  }

private:
  AbstractCoroutine* m_parent;
protected:
  Action m_parentReturnAction;
  FunctionPtr m_parentReturnFP;
public:

  /**
   * Constructor.
   */
  AbstractCoroutine();

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
  virtual Action call(const FunctionPtr& ptr) = 0;

  /**
   * Default implementation of handleError(error) function.
   * User may override this function in order to handle errors.
   * @param error - &id:oatpp::async::Error;.
   * @return - Action. If handleError function returns Error,
   * current coroutine will finish, return control to caller coroutine and handleError is called for caller coroutine.
   */
  virtual Action handleError(Error* error);

  /**
   * Get parent coroutine
   * @return - pointer to a parent coroutine
   */
  AbstractCoroutine* getParent() const;

  /**
   * Convenience method to generate Action of `type == Action::TYPE_REPEAT`.
   * @return - repeat Action.
   */
  static Action repeat();

  /**
   * Convenience method to generate Action of `type == Action::TYPE_WAIT_REPEAT`.
   * @return - TYPE_WAIT_REPEAT Action.
   */
  static Action waitRepeat(const std::chrono::duration<v_int64, std::micro>& timeout);

  /**
   * Wait asynchronously for the specified time.
   * @return - repeat Action.
   */
  CoroutineStarter waitFor(const std::chrono::duration<v_int64, std::micro>& timeout);

  /**
   * Convenience method to generate Action of `type == Action::TYPE_IO_WAIT`.
   * @return - TYPE_WAIT_FOR_IO Action.
   */
  static Action ioWait(v_io_handle ioHandle, Action::IOEventType ioEventType);

  /**
   * Convenience method to generate Action of `type == Action::TYPE_IO_WAIT`.
   * @return - TYPE_IO_REPEAT Action.
   */
  static Action ioRepeat(v_io_handle ioHandle, Action::IOEventType ioEventType);

  /**
   * Convenience method to generate error reporting Action.
   * @param error - &id:oatpp:async::Error;.
   * @return - error reporting Action.
   */
  static Action error(Error* error);

  /**
   * Convenience method to generate error reporting Action.
   * @tparam E - Error class type.
   * @tparam Args - Error constructor arguments.
   * @param args - actual error constructor arguments.
   * @return - error reporting &id:oatpp::async::Action;.
   */
  template<class E, typename ... Args>
  Action error(Args... args) {
    return error(new E(args...));
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
    (void)sz;
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
  static CoroutineStarter start(ConstructorArgs&&... args) {
    return new T(std::forward<ConstructorArgs>(args)...);
  }

  /**
   * Call function of Coroutine specified by ptr. <br>
   * Overridden `AbstractCoroutine::call()` method.
   * @param ptr - pointer of the function to call.
   * @return - Action.
   */
  Action call(const FunctionPtr& ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_YIELD_TO`.
   * @param function - pointer to function.
   * @return - yield Action.
   */
  Action yieldTo(const Function& function) const {
    return Action(static_cast<FunctionPtr>(function));
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
template<typename ...Args>
class AbstractCoroutineWithResult : public AbstractCoroutine {
protected:
  std::unique_ptr<AbstractMemberCaller<Args...>> m_parentMemberCaller;
public:

  /**
   * Class representing Coroutine call for result;
   */
  class StarterForResult {
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
      if (this == std::addressof(other)) return *this;
        
      delete m_coroutine;
      m_coroutine = other.m_coroutine;
      other.m_coroutine = nullptr;
      return *this;
    }

    /**
     * Set callback for result and return coroutine starting Action.
     * @tparam C - caller coroutine type.
     * @tparam Args - callback params.
     * @param callback - callback to obtain result.
     * @return - &id:oatpp::async::Action;.
     */
    template<typename C>
    Action callbackTo(Action (C::*callback)(Args...)) {
      if(m_coroutine == nullptr) {
        throw std::runtime_error("[oatpp::async::AbstractCoroutineWithResult::StarterForResult::callbackTo()]: Error. Coroutine is null.");
      }
      m_coroutine->m_parentMemberCaller = createMemberCaller(callback);
      Action result = m_coroutine;
      m_coroutine = nullptr;
      return result;

    }

  };

};

template <typename ...Args>
using
CoroutineStarterForResult = typename AbstractCoroutineWithResult<Args...>::StarterForResult;

/**
 * Coroutine with result template. <br>
 * Example usage:<br>
 * `class CoroutineWithResult : public oatpp::async::CoroutineWithResult<CoroutineWithResult, const char*>`
 * @tparam T - child class type.
 * @tparam Args - return argumet type.
 */
template<class T, typename ...Args>
class CoroutineWithResult : public AbstractCoroutineWithResult<Args...> {
  friend AbstractCoroutine;
public:
  typedef Action (T::*Function)();
public:

  static void* operator new(std::size_t sz) {
    return ::operator new(sz);
  }

  static void operator delete(void* ptr, std::size_t sz) {
    (void)sz;
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
  Action call(const AbstractCoroutine::FunctionPtr& ptr) override {
    Function f = static_cast<Function>(ptr);
    return (static_cast<T*>(this)->*f)();
  }

  /**
   * Convenience method to generate Action of `type == Action::TYPE_YIELD_TO`.
   * @param function - pointer to function.
   * @return - yield Action.
   */
  Action yieldTo(const Function& function) const {
    return Action(static_cast<AbstractCoroutine::FunctionPtr>(function));
  }

  /**
   * Call caller's Callback passing returned value, and generate Action of `type == Action::TYPE_FINISH`.
   * @param args - argumets to be passed to callback.
   * @return - finish Action.
   */
  Action _return(const Args&... args) {
    this->m_parentReturnAction = this->m_parentMemberCaller->call(this->getParent(), args...);
    return Action::createActionByType(Action::TYPE_FINISH);
  }
  
};

}}

#endif /* oatpp_async_Coroutine_hpp */
