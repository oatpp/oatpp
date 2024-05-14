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

#include <chrono>

namespace oatpp { namespace test {

void UnitTest::run(v_int32 times) {
  
  OATPP_LOGi(TAG, "\033[1mSTART\033[0m...")
  
  v_counter objectsCount = oatpp::Environment::getObjectsCount();
  v_counter objectsCreated = oatpp::Environment::getObjectsCreated();

  before();
  
  v_int64 ticks = oatpp::Environment::getMicroTickCount();
  
  for(v_int32 i = 0; i < times; i++){
    onRun();
  }
  
  v_int64 millis = oatpp::Environment::getMicroTickCount() - ticks;

  after();
  
  v_counter leakingObjects = oatpp::Environment::getObjectsCount() - objectsCount;
  v_counter objectsCreatedPerTest = oatpp::Environment::getObjectsCreated() - objectsCreated;
  
  if(leakingObjects == 0){
    OATPP_LOGi(TAG, "\033[1mFINISHED\033[0m - \033[1;32msuccess!\033[0m")
    OATPP_LOGi(TAG, "\033[33m{}(micro), {}(objs)\033[0m\n", millis, objectsCreatedPerTest)
  }else{

    OATPP_LOGe(TAG, "\033[1mFINISHED\033[0m - \033[1;31mfailed\033[0m, leakingObjects = {}", leakingObjects)

    exit(EXIT_FAILURE);

  }
  
}

}}
