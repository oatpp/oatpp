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

#include "PerfTest.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/concurrency/Thread.hpp"

#include <list>

namespace oatpp { namespace test { namespace memory {
  
namespace {
  
  class TestBase {
  public:
    
    static void* operator new(std::size_t sz) {
      if(sz != sizeof(TestBase)){
        throw std::runtime_error("wrong object size");
      }
      return ::operator new(sz);
    }
    
    static void operator delete(void* ptr, std::size_t sz) {
      if(sz != sizeof(TestBase)){
        oatpp::base::Environment::log(2, "PerfTest::TestBase", "[ERROR|CRITICAL]: MemoryPool malfunction. Deleting object of wrong size"); \
      }
      ::operator delete(ptr);
    }
    
  };
  
  class TestChild : public oatpp::base::Controllable, public TestBase {
  public:
    
    static void* operator new(std::size_t sz) {
      return ::operator new(sz);
    }
    
    static void operator delete(void* ptr, std::size_t sz) {
      ::operator delete(ptr);
    }
    
    
  };
  
  class Task : public oatpp::concurrency::Runnable, public oatpp::base::Controllable {
  private:
    std::shared_ptr<TestBase> m_shared;
  public:
    
    Task(const std::shared_ptr<TestBase>& shared)
      : m_shared(shared)
    {}
    
    static std::shared_ptr<Task> createShared(const std::shared_ptr<TestBase>& shared){
      return std::make_shared<Task>(shared);
    }
    
    void run() override {
      for(v_int32 i = 0; i < 10; i ++) {
        std::shared_ptr<TestBase> shared(new TestChild());
      }
      /*for(v_int32 i = 0; i < 1000000; i ++) {
        std::shared_ptr<TestClass> shared = m_shared;
      }*/
    }
  };
  
}
  
bool PerfTest::onRun() {
  
  v_int32 iterations = 1;
  v_int32 threadCount = 100;
  
  OATPP_LOGD(TAG, "size=%d", sizeof(TestBase));
  
  for(int i = 0; i < iterations; i++) {
  
    auto threads = oatpp::collection::LinkedList<std::shared_ptr<oatpp::concurrency::Thread>>::createShared(); ;
    
    for(v_int32 n = 0; n < threadCount; n++) {
      std::shared_ptr<TestBase> shared;
      std::shared_ptr<Task> task = Task::createShared(shared);
      auto thread = oatpp::concurrency::Thread::createShared(task);
      threads->pushBack(thread);
    }
    
    threads->forEach([](const std::shared_ptr<oatpp::concurrency::Thread>& thread){
      thread->join();
    });
    
    /*auto it = threads.begin();
    while (it != threads.end()) {
      (*it)->join();
      it++;
    }
     */
    
  }
  
  return true;
}
  
}}}
