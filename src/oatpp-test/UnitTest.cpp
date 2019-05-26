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

#include "UnitTest.hpp"

#include "oatpp/core/base/memory/MemoryPool.hpp"

#include <chrono>

namespace oatpp { namespace test {

void UnitTest::run(v_int32 times) {
  
  OATPP_LOGI(TAG, "\033[1mSTART\033[0m...");
  
  v_counter objectsCount = base::Environment::getObjectsCount();
  v_counter objectsCreated = base::Environment::getObjectsCreated();
  
  v_int64 ticks = base::Environment::getMicroTickCount();
  
  for(v_int32 i = 0; i < times; i++){
    onRun();
  }
  
  v_int64 millis = base::Environment::getMicroTickCount() - ticks;
  
  v_counter leakingObjects = base::Environment::getObjectsCount() - objectsCount;
  v_counter objectsCreatedPerTest = base::Environment::getObjectsCreated() - objectsCreated;
  
  if(leakingObjects == 0){
    OATPP_LOGI(TAG, "\033[1mFINISHED\033[0m - \033[1;32msuccess!\033[0m");
    OATPP_LOGI(TAG, "\033[33m%d(micro), %d(objs)\033[0m\n", millis, objectsCreatedPerTest);
  }else{

    OATPP_LOGE(TAG, "\033[1mFINISHED\033[0m - \033[1;31mfailed\033[0m, leakingObjects = %d", leakingObjects);
    
    auto POOLS = oatpp::base::memory::MemoryPool::POOLS;
    auto it = POOLS.begin();
    while (it != POOLS.end()) {
      auto pool = it->second;
      if(pool->getObjectsCount() != 0) {
        OATPP_LOGV("Pool", "name: '%s' [%d(objs)]", pool->getName().c_str(), pool->getObjectsCount());
      }
      it ++;
    }

    exit(EXIT_FAILURE);

  }
  
}

}}
