/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#ifndef oatpp_concurrency_Thread_hpp
#define oatpp_concurrency_Thread_hpp

#include "./Runnable.hpp"

#include "../base/memory/ObjectPool.hpp"

#include "../base/Controllable.hpp"
#include "../base/PtrWrapper.hpp"

#include <thread>
#include <atomic>

namespace oatpp { namespace concurrency {
  
class Thread : public base::Controllable {
public:
  OBJECT_POOL(Thread_Pool, Thread, 32)
  SHARED_OBJECT_POOL(Shared_Thread_Pool, Thread, 32)
private:
  std::thread m_thread;
public:
  
  Thread(const std::shared_ptr<Runnable>& runnable) {
    m_thread = std::thread([runnable]{
      runnable->run();
    });
  }
  
public:
  
  static std::shared_ptr<Thread> createShared(const std::shared_ptr<Runnable>& runnable){
    return Shared_Thread_Pool::allocateShared(runnable);
  }
  
  void join(){
    m_thread.join();
  }
  
  void detach(){
    m_thread.detach();
  }
  
};

}}

#endif /* concurrency_Thread_hpp */
