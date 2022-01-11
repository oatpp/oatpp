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

#include "Worker.hpp"

namespace oatpp { namespace async { namespace worker {

Worker::Worker(Type type)
  : m_type(type)
{}

void Worker::setCoroutineScheduledAction(CoroutineHandle* coroutine, Action &&action) {
  coroutine->_SCH_A = std::forward<Action>(action);
}

Action& Worker::getCoroutineScheduledAction(CoroutineHandle* coroutine) {
  return coroutine->_SCH_A;
}

Processor* Worker::getCoroutineProcessor(CoroutineHandle* coroutine) {
  return coroutine->_PP;
}

void Worker::dismissAction(Action& action) {
  action.m_type = Action::TYPE_NONE;
}

CoroutineHandle* Worker::nextCoroutine(CoroutineHandle* coroutine) {
  return coroutine->_ref;
}

Worker::Type Worker::getType() {
  return m_type;
}

}}}