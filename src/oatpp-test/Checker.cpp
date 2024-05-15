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
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace test {

PerformanceChecker::PerformanceChecker(const char* tag)
  : m_tag(tag)
  , m_ticks(oatpp::Environment::getMicroTickCount())
{}

PerformanceChecker::~PerformanceChecker(){
  v_int64 elapsedTicks = oatpp::Environment::getMicroTickCount() - m_ticks;
  OATPP_LOGd(m_tag, "{}(micro)", elapsedTicks)
}

v_int64 PerformanceChecker::getElapsedTicks(){
  return oatpp::Environment::getMicroTickCount() - m_ticks;
}


ThreadLocalObjectsChecker::ThreadLocalObjectsChecker(const char* tag)
  : m_tag(tag)
  , m_objectsCount(oatpp::Environment::getThreadLocalObjectsCount())
  , m_objectsCreated(oatpp::Environment::getThreadLocalObjectsCreated())
{
}

ThreadLocalObjectsChecker::~ThreadLocalObjectsChecker(){
  
  v_counter leakingObjects = oatpp::Environment::getThreadLocalObjectsCount() - m_objectsCount;
  v_counter objectsCreatedPerTest = oatpp::Environment::getThreadLocalObjectsCreated() - m_objectsCreated;
  
  if(leakingObjects == 0){
    OATPP_LOGe(m_tag, "OK:\n created(obj): {}", objectsCreatedPerTest)
  }else{
    OATPP_LOGe(m_tag, "FAILED, leakingObjects = {}", leakingObjects)
    OATPP_ASSERT(false)
  }
  
}
  
}}
