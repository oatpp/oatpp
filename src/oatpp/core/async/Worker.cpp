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

namespace oatpp { namespace async {

void Worker::setCoroutineScheduledAction(AbstractCoroutine *CP, Action &&action) {
  CP->_SCH_A = std::forward<Action>(action);
}

Processor* Worker::getCoroutineProcessor(AbstractCoroutine* CP) {
  return CP->_PP;
}

v_int64 Worker::getCoroutineTimePoint(AbstractCoroutine* CP) {
  return CP->_SCH_A.m_data.timePointMicroseconds;
}

void Worker::dismissAction(Action& action) {
  action.m_type = Action::TYPE_NONE;
}

AbstractCoroutine* Worker::nextCoroutine(AbstractCoroutine* CP) {
  return CP->_ref;
}

}}