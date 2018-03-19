//
//  Block.cpp
//  crud
//
//  Created by Leonid on 3/17/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "Routine.hpp"

namespace oatpp { namespace async {

const v_int32 Action::TYPE_NONE = 0;
const v_int32 Action::TYPE_REPEAT = 1;
const v_int32 Action::TYPE_WAIT_RETRY = 2;
const v_int32 Action::TYPE_RETURN = 3;
const v_int32 Action::TYPE_ABORT = 4;
const v_int32 Action::TYPE_ERROR = 5;
const v_int32 Action::TYPE_ROUTINE = 6;
  
RoutineBuilder::RoutineBuilder()
  : m_routine(new Routine())
{}

RoutineBuilder::RoutineBuilder(const RoutineBuilder& other)
  : m_routine(other.m_routine)
{
  other.m_routine = nullptr;
}

RoutineBuilder::~RoutineBuilder() {
  if(m_routine != nullptr) {
    delete m_routine;
  }
}

RoutineBuilder& RoutineBuilder::_then(const Block& block){
  m_routine->blocks.pushBack(block);
  return *this;
}
  
Action& Action::_repeat() {
  static Action a(TYPE_REPEAT);
  return a;
}
Action& Action::_wait_retry() {
  static Action a(TYPE_WAIT_RETRY);
  return a;
}
Action& Action::_return(){
  static Action a(TYPE_RETURN);
  return a;
}
Action& Action::_abort(){
  static Action a(TYPE_ABORT);
  return a;
}

Action::Action(v_int32 type)
  : m_type(type)
  , m_routine(nullptr)
{}
  
Action::Action()
  : m_type(TYPE_NONE)
  , m_routine(nullptr)
{}

Action::Action(const Error& error)
  : m_type (TYPE_ERROR)
  , m_error(error)
  , m_routine(nullptr)
{}
  
Action::Action(const Routine::Builder& routine)
  : m_type(TYPE_ROUTINE)
  , m_error({nullptr, false})
  , m_routine(routine.m_routine)
{
  routine.m_routine = nullptr;
}

Action::Action(std::nullptr_t nullp)
  : m_type(TYPE_ROUTINE)
  , m_error({nullptr, false})
  , m_routine(new Routine())
{}
  
Action::~Action() {
  if(m_routine != nullptr){
    delete m_routine;
  }
}

Error& Action::getError(){
  return m_error;
}

bool Action::isErrorAction(){
  return m_type == TYPE_ERROR;
}

v_int32 Action::getType(){
  return m_type;
}
  
  
}}
