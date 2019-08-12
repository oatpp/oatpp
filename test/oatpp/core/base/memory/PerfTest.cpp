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
  
  class TestChild : public oatpp::base::Countable, public TestBase {
  public:

    static void* operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    static void operator delete(void* ptr, std::size_t sz) {
      (void)sz;
      ::operator delete(ptr);
    }

  };
  
  class Task : public oatpp::base::Countable {
  private:
    std::shared_ptr<TestBase> m_shared;
  public:
    
    Task(const std::shared_ptr<TestBase>& shared)
      : m_shared(shared)
    {}
    
    void run() {
      for(v_int32 i = 0; i < 10; i ++) {
        std::shared_ptr<TestBase> shared(new TestChild());
      }
    }
  };
  
}
  
void PerfTest::onRun() {
  
  v_int32 iterations = 1;
  v_int32 threadCount = 100;
  
  for(int i = 0; i < iterations; i++) {

    std::list<std::thread> threads;
    
    for(v_int32 n = 0; n < threadCount; n++) {
      std::shared_ptr<TestBase> shared;
      threads.push_back(std::thread(&Task::run, Task(shared)));
    }

    for(auto& thread : threads) {
      thread.join();
    }
    
  }

}
  
}}}
