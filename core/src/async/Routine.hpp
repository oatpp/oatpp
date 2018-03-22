//
//  Block.hpp
//  crud
//
//  Created by Leonid on 3/17/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_async_Block_hpp
#define oatpp_async_Block_hpp

#include "Stack.hpp"

#include "../base/Environment.hpp"

#include <functional>

namespace oatpp { namespace async {
  
class Action; // FWD
class Routine; // FWD
  
struct Error {
public:
  
  Error()
    : error(nullptr)
    , isExceptionThrown(false)
  {}
  
  Error(const char* pError, bool isException = false)
    : error(pError)
    , isExceptionThrown(isException)
  {}
  
  const char* error;
  bool isExceptionThrown;
  
};
  
struct Block {
public:
  typedef std::function<Action ()> Function;
  typedef std::function<Action (const Error&)> ErrorHandler;
public:
  Function function;
  ErrorHandler errorHandler;
};
  
class RoutineBuilder {
  friend Action;
  friend Processor;
private:
  mutable Routine* m_routine;
public:
  
  RoutineBuilder();
  RoutineBuilder(const RoutineBuilder& other);
  
  ~RoutineBuilder();
  
  RoutineBuilder& _then(const Block& block);
  
};
  
class Action {
  friend Processor;
  friend Routine;
public:
  static const v_int32 TYPE_NONE;
  static const v_int32 TYPE_REPEAT;
  static const v_int32 TYPE_WAIT_RETRY;
  static const v_int32 TYPE_RETURN;
  static const v_int32 TYPE_ABORT;
  static const v_int32 TYPE_ERROR;
  static const v_int32 TYPE_ROUTINE;
public:
  static Action& _repeat();
  static Action& _wait_retry();
  static Action& _return();
  static Action& _abort();
  static Action& _continue();
private:
  void null(){
    m_type = TYPE_NONE;
    m_routine = nullptr;
  }
private:
  v_int32 m_type;
  Error m_error;
  Routine* m_routine;
private:
  Action(v_int32 type);
  Action();
public:
  
  Action(const Error& error);
  Action(const RoutineBuilder& routine);
  Action(std::nullptr_t nullp);
  
  ~Action();
  
  Error& getError();
  bool isErrorAction();
  v_int32 getType();
  bool isNone() {
    return m_type == TYPE_NONE;
  }
  
};
  
class Routine {
  friend Processor;
public:
  typedef RoutineBuilder Builder;
private:
  Routine* m_parent;
public:
  
  Routine()
    : m_parent(nullptr)
  {}
  
  Stack<Block> blocks;
  Action pendingAction;

public:
  
  static Builder _do(const Block& block){
    Builder builder;
    builder._then(block);
    return builder;
  }
  
};
  
}}

#endif /* oatpp_async_Block_hpp */
