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

#include "MemoryPoolTest.hpp"

#include "oatpp/core/base/memory/MemoryPool.hpp"
#include "oatpp-test/Checker.hpp"

namespace oatpp { namespace test { namespace memory {

namespace {
  
class TestClass{
public:
  
  TestClass(v_int32 p)
    :a (p)
  {
  }
  
  const v_int32 a;
  
};
  
void doGarbageAllocsStdNew(v_int32 garbageNumber){
  
  TestClass** garbage = new TestClass* [garbageNumber];
  
  for(v_int32 i = 0; i < garbageNumber; i++){
    garbage[i] = new TestClass(-100);
  }
  
  for(v_int32 i = 0; i < garbageNumber; i++){
    delete garbage[i];
  }
  
  delete [] garbage;
  
}
  
void testStdNew(v_int32 objectsNumber, v_int32 garbageNumber, v_int32 chunkSize){

  (void)chunkSize;

  TestClass** objects = new TestClass* [objectsNumber];
  
  for(v_int32 i = 0; i < objectsNumber; i++){
    objects[i] = new TestClass(i);
  }
  
  doGarbageAllocsStdNew(garbageNumber);
  
  for(v_int32 i = 0; i < objectsNumber; i++){
    OATPP_ASSERT(objects[i]->a == i);
  }
  
  for(v_int32 i = 0; i < objectsNumber; i++){
    delete objects[i];
  }
  
  doGarbageAllocsStdNew(garbageNumber);
  
  delete [] objects;
  
}
  
void doGarbageAllocs(base::memory::MemoryPool& pool, v_int32 garbageNumber){
  
  TestClass** garbage = new TestClass* [garbageNumber];
  
  for(v_int32 i = 0; i < garbageNumber; i++){
    garbage[i] = new (pool.obtain()) TestClass(-100);
  }
  
  for(v_int32 i = 0; i < garbageNumber; i++){
    oatpp::base::memory::MemoryPool::free(garbage[i]);
  }
  
  delete [] garbage;
  
}

void testPool(v_int32 objectsNumber, v_int32 garbageNumber, v_int32 chunkSize){
  
  static base::memory::MemoryPool pool("MemoryPoolTest::Pool2", sizeof(TestClass), chunkSize);
  
  TestClass** objects = new TestClass* [objectsNumber];
  
  for(v_int32 i = 0; i < objectsNumber; i++){
    objects[i] = new (pool.obtain()) TestClass(i);
  }
  
  doGarbageAllocs(pool, garbageNumber);
  
  for(v_int32 i = 0; i < objectsNumber; i++){
    OATPP_ASSERT(objects[i]->a == i);
  }
  
  for(v_int32 i = 0; i < objectsNumber; i++){
    auto obj = objects[i];
    obj->~TestClass();
    oatpp::base::memory::MemoryPool::free(obj);
  }
  
  doGarbageAllocs(pool, garbageNumber);
#ifndef OATPP_DISABLE_POOL_ALLOCATIONS
  for(v_int32 i = 0; i < objectsNumber; i++){
    OATPP_ASSERT(objects[i]->a == -100);
  }
#else
  OATPP_LOGW("TEST[base::memory::MemoryPoolTest]", "WARNING. 'OATPP_DISABLE_POOL_ALLOCATIONS' flag is ON. Assertions disabled.");
#endif
  delete [] objects;
  
  OATPP_ASSERT(pool.getObjectsCount() == 0);
  
}
  
  void doStdSimpleAlloc(){
    TestClass* obj = new TestClass(10);
    delete obj;
  }
  
  void doPoolSimpleAlloc(){
    static base::memory::MemoryPool pool("MemoryPoolTest::Pool3", sizeof(TestClass), 128);
    TestClass* obj = new (pool.obtain()) TestClass(10);
    obj->~TestClass();
    oatpp::base::memory::MemoryPool::free(obj);
  }
  
  void doStackAlloc(){
    TestClass a(10);
  }
  
}
  
void MemoryPoolTest::onRun() {
  
  const v_int32 objectsNumber = 1000000;
  const v_int32 garbageNumber = 1000000;
  const v_int32 chunkSize = 128;
  
  {
    PerformanceChecker checker("Alloc Time - Pool");
    testPool(objectsNumber, garbageNumber, chunkSize);
  }
  
  {
    PerformanceChecker checker("Alloc Time -  new");
    testStdNew(objectsNumber, garbageNumber, chunkSize);
  }
  
  v_int32 iterationsCount = 10000000;

  {
    PerformanceChecker checker("doStdSimpleAlloc time:");
    for(v_int32 i = 0; i < iterationsCount; i++){
      doStdSimpleAlloc();
    }
  }
  
  {
    PerformanceChecker checker("doPoolSimpleAlloc time:");
    for(v_int32 i = 0; i < iterationsCount; i++){
      doPoolSimpleAlloc();
    }
  }
  
  {
    PerformanceChecker checker("doStackAlloc time:");
    for(v_int32 i = 0; i < iterationsCount; i++){
      doStackAlloc();
    }
  }
  
}

}}}
