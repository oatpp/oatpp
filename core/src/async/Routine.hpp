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
  
struct Error {
public:
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
  
class Routine {
  friend Processor;
private:
  Routine* m_parent;
public:
  
  Routine()
    : m_parent(nullptr)
  {}
  
  Stack<Block> blocks;
  
public:
  
  class Builder {
    friend Action;
    friend Processor;
  private:
    mutable Routine* m_routine;
  public:
    
    Builder()
      : m_routine(new Routine())
    {}
    
    Builder(const Builder& other)
      : m_routine(other.m_routine)
    {
      other.m_routine = nullptr;
    }
    
    ~Builder() {
      if(m_routine != nullptr) {
        delete m_routine;
      }
    }
    
    Builder& _then(const Block& block){
      m_routine->blocks.pushBack(block);
      return *this;
    }
    
  };
  
public:
  
  static Builder _do(const Block& block){
    Builder builder;
    builder._then(block);
    return builder;
  }
  
};
  
class Action {
  friend Processor;
public:
  static const v_int32 TYPE_RETRY;
  static const v_int32 TYPE_RETURN;
  static const v_int32 TYPE_ABORT;
  static const v_int32 TYPE_ERROR;
  static const v_int32 TYPE_ROUTINE;
public:
  static Action& _retry() {
    static Action a(TYPE_RETRY);
    return a;
  }
  static Action& _return(){
    static Action a(TYPE_RETURN);
    return a;
  }
  static Action& _abort(){
    static Action a(TYPE_ABORT);
    return a;
  }
private:
  v_int32 m_type;
  Error m_error;
  Routine* m_routine;
private:
  Action(v_int32 type)
    : m_type(type)
  {}
public:
  
  Action(const Error& error)
    : m_type (TYPE_ERROR)
    , m_error(error)
    , m_routine(nullptr)
  {}
  
  Action(const Routine::Builder& routine)
    : m_type(TYPE_ROUTINE)
    , m_error({nullptr, false})
    , m_routine(routine.m_routine)
  {
    routine.m_routine = nullptr;
  }
  
  Action(std::nullptr_t nullp)
    : m_type(TYPE_ROUTINE)
    , m_error({nullptr, false})
    , m_routine(new Routine())
  {}
  
  ~Action() {
    if(m_routine != nullptr){
      delete m_routine;
    }
  }
  
  Error& getError(){
    return m_error;
  }
  
  bool isErrorAction(){
    return m_type == TYPE_ERROR;
  }
  
  v_int32 getType(){
    return m_type;
  }
  
};
  
}}

#endif /* oatpp_async_Block_hpp */
