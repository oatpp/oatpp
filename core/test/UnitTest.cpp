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

#include "UnitTest.hpp"

#include "../src/base/memory/MemoryPool.hpp" // delete
#include "../src/base/Controllable.hpp" // delete

#include <chrono>

namespace oatpp { namespace test {

v_int64 UnitTest::getTickCount(){
  std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );
  
  return ms.count();
  
}
  
bool UnitTest::run(v_int32 times) {
  
  OATPP_LOGD(TAG, "START...");
  
  v_counter objectsCount = base::Environment::getObjectsCount();
  v_counter objectsCreated = base::Environment::getObjectsCreated();
  
  v_int64 ticks = getTickCount();
  
  bool result = true;
  
  for(v_int32 i = 0; i < times; i++){
    result = onRun();
  }
  
  v_int64 millis = getTickCount() - ticks;
  
  v_counter leakingObjects = base::Environment::getObjectsCount() - objectsCount;
  v_counter objectsCreatedPerTest = base::Environment::getObjectsCreated() - objectsCreated;
  
  if(leakingObjects == 0 && result == true){
    OATPP_LOGD(TAG, "FINISHED - success");
    OATPP_LOGD(TAG, "%d(micro), %d(objs)\n", millis, objectsCreatedPerTest);
  }else{
    result = false;
    OATPP_LOGD(TAG, "FINISHED - failed, leakingObjects = %d", leakingObjects);
    
    auto POOLS = oatpp::base::memory::MemoryPool::POOLS;
    auto it = POOLS.begin();
    while (it != POOLS.end()) {
      auto pool = it->second;
      OATPP_LOGD("Pool", "name: '%s' [%d(objs)]", pool->getName().c_str(), pool->getObjectsCount());
      it ++;
    }
    /*
    leakingObjects = base::Environment::getObjectsCount() - objectsCount;
    base::Environment::log(TAG,
                           "FINISHED - failed, leakingObjects = %d",
                           leakingObjects);
     */
  }
  
  OATPP_LOGD("test", "Pools count=%d", oatpp::base::memory::MemoryPool::POOLS.size());
  
  OATPP_ASSERT(result);
  return result;
  
}

}}
