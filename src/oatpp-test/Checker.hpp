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

#ifndef oatpp_test_Checker_hpp
#define oatpp_test_Checker_hpp

#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace test {

/**
 * Helper class to check performance of code block.
 */
class PerformanceChecker {
private:
  const char* m_tag;
  v_int64 m_ticks;
public:

  /**
   * Constructor.
   * @param tag - log tag.
   */
  PerformanceChecker(const char* tag);

  /**
   * Non virtual destructor.
   * Will print time elapsed ticks on destruction.
   */
  ~PerformanceChecker();

  /**
   * Get elapsed time from checker creation.
   * @return - ticks in microseconds.
   */
  v_int64 getElapsedTicks();
    
};

/**
 * Helper class to check block of code on memory leaks.
 * Checks &id:oatpp::base::Countable; objects, and objects allocated on memory pools.
 */
class ThreadLocalObjectsChecker {
private:
  class MemoryPoolData {
  public:
    const char* name;
    v_int64 size;
    v_int64 objectsCount;
  };
private:
  const char* m_tag;
  v_counter m_objectsCount;
  v_counter m_objectsCreated;
public:

  /**
   * Constructor.
   * @param tag - log tag.
   */
  ThreadLocalObjectsChecker(const char* tag);

  /**
   * Non virtual destructor.
   * Will halt program execution if memory leaks detected.
   */
  ~ThreadLocalObjectsChecker();
};
  
}}

#endif /* Checker_hpp */
