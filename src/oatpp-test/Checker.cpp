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

#include "Checker.hpp"

namespace oatpp { namespace test {

PerformanceChecker::PerformanceChecker(const char* tag)
  : m_tag(tag)
  , m_ticks(oatpp::base::Environment::getMicroTickCount())
{}

PerformanceChecker::~PerformanceChecker(){
  v_int64 elapsedTicks = oatpp::base::Environment::getMicroTickCount() - m_ticks;
  OATPP_LOGD(m_tag, "%d(micro)", elapsedTicks);
}

v_int64 PerformanceChecker::getElapsedTicks(){
  return oatpp::base::Environment::getMicroTickCount() - m_ticks;
}


ThreadLocalObjectsChecker::ThreadLocalObjectsChecker(const char* tag)
  : m_tag(tag)
  , m_objectsCount(oatpp::base::Environment::getThreadLocalObjectsCount())
  , m_objectsCreated(oatpp::base::Environment::getThreadLocalObjectsCreated())
{
}

ThreadLocalObjectsChecker::~ThreadLocalObjectsChecker(){
  
  v_counter leakingObjects = base::Environment::getThreadLocalObjectsCount() - m_objectsCount;
  v_counter objectsCreatedPerTest = base::Environment::getThreadLocalObjectsCreated() - m_objectsCreated;
  
  if(leakingObjects == 0){
    OATPP_LOGE(m_tag, "OK:\n created(obj): %d", objectsCreatedPerTest);
  }else{
    OATPP_LOGE(m_tag, "FAILED, leakingObjects = %d", leakingObjects);
    OATPP_ASSERT(false);
  }
  
}
  
}}
