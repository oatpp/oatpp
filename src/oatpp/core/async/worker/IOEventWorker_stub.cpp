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

#include "IOEventWorker.hpp"

#ifdef OATPP_IO_EVENT_INTERFACE_STUB

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// windows based implementation

#include "oatpp/core/async/Processor.hpp"

namespace oatpp { namespace async { namespace worker {

void IOEventWorker::initEventQueue() {
  throw std::runtime_error("[IOEventWorker for Target OS is NOT IMPLEMENTED! Use IOWorker instead.]");
}

void IOEventWorker::triggerWakeup() {
  throw std::runtime_error("[IOEventWorker for Target OS is NOT IMPLEMENTED! Use IOWorker instead.]");
}

void IOEventWorker::setTriggerEvent(p_char8 eventPtr) {
  (void)eventPtr;
  throw std::runtime_error("[IOEventWorker for Target OS is NOT IMPLEMENTED! Use IOWorker instead.]");
}

void IOEventWorker::setCoroutineEvent(CoroutineHandle* coroutine, int operation, p_char8 eventPtr) {
  (void)eventPtr;
  (void)coroutine;
  (void)operation;
  throw std::runtime_error("[IOEventWorker for Target OS is NOT IMPLEMENTED! Use IOWorker instead.]");
}

void IOEventWorker::consumeBacklog() {
  throw std::runtime_error("[IOEventWorker for Target OS is NOT IMPLEMENTED! Use IOWorker instead.]");
}

void IOEventWorker::waitEvents() {
  throw std::runtime_error("[IOEventWorker for Target OS is NOT IMPLEMENTED! Use IOWorker instead.]");
}

}}}

#endif // #ifdef OATPP_IO_EVENT_INTERFACE_STUB
