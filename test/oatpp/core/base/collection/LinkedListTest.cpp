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

#include "LinkedListTest.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp-test/Checker.hpp"
#include <list>

namespace oatpp { namespace test { namespace collection {
  
namespace {
  
class TestObject : public oatpp::base::Countable {
public:
  SHARED_OBJECT_POOL(TestObject_Pool2, TestObject, 32)
public:
  TestObject()
  {}
public:
  
  static std::shared_ptr<TestObject> createShared2(){
    return std::make_shared<TestObject>();
  }
  
  static std::shared_ptr<TestObject> createShared(){
    return TestObject_Pool2::allocateShared();
  }
  
};
  
void testListPerformance(v_int32 iterationsCount){
  
  auto list = oatpp::collection::LinkedList<std::shared_ptr<TestObject>>::createShared();
  
  for(v_int32 i = 0; i < iterationsCount; i++){
    list->pushBack(TestObject::createShared());
  }
  
  auto curr = list->getFirstNode();
  while (curr != nullptr) {
    auto data = curr->getData();
    curr = curr->getNext();
  }
  
}
  
void testStdListPerformance(v_int32 iterationsCount){
  
  std::list<std::shared_ptr<TestObject>> list;
  
  for(v_int32 i = 0; i < iterationsCount; i++){
    list.push_back(TestObject::createShared());
  }
  
  auto it = list.begin();
  while (it != list.end()) {
    auto data = *it;
    it++;
  }
  
}
  
}
  
void LinkedListTest::onRun() {
  
  
  v_int32 iterationsCount = 100000;
  
  {
    PerformanceChecker checker("oatpp::collection::LinkedList pushBack time");
    for(v_int32 i = 0; i < 10; i ++) {
      testListPerformance(iterationsCount);
    }
  }
  
  {
    PerformanceChecker checker("std::list pushBack time");
    for(v_int32 i = 0; i < 10; i ++) {
      testStdListPerformance(iterationsCount);
    }
  }

}
  
}}}
