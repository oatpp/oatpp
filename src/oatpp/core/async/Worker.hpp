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

#ifndef oatpp_async_Worker_hpp
#define oatpp_async_Worker_hpp

#include "./Coroutine.hpp"

namespace oatpp { namespace async {

class Worker {
public:

  enum Type : v_int32 {

    /**
     * Worker type - general processor.
     */
    PROCESSOR = 0,

    /**
     * Worker type - timer processor.
     */
    TIMER = 1,

    /**
     * Worker type - I/O processor.
     */
    IO = 2,

    /**
     * Number of types in this enum.
     */
    TYPES_COUNT = 3

  };

private:
  Type m_type;
protected:
  static void setCoroutineScheduledAction(AbstractCoroutine* CP, Action&& action);
  static Action& getCoroutineScheduledAction(AbstractCoroutine* CP);
  static Processor* getCoroutineProcessor(AbstractCoroutine* CP);
  static v_int64 getCoroutineTimePoint(AbstractCoroutine* CP);
  static void dismissAction(Action& action);
  static AbstractCoroutine* nextCoroutine(AbstractCoroutine* CP);
public:

  Worker(Type type);

  virtual ~Worker() = default;

  virtual void pushTasks(oatpp::collection::FastQueue<AbstractCoroutine>& tasks) = 0;
  virtual void pushOneTask(AbstractCoroutine* task) = 0;

  virtual void stop() = 0;

  Type getType();

};

}}

#endif //oatpp_async_Worker_hpp
