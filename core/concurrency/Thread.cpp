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

#include "Thread.hpp"
#include <atomic>

#if defined(_GNU_SOURCE)
  #include <pthread.h>
#endif

namespace oatpp { namespace concurrency {
  
v_int32 Thread::getThreadSuggestedCpuIndex(std::thread::id threadId, v_int32 cpuCount) {
  static std::hash<std::thread::id> hashFunction;
  v_int32 lock = hashFunction(threadId) % OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT;
  return lock % cpuCount;
}

v_int32 Thread::assignThreadToCpu(std::thread::native_handle_type nativeHandle, v_int32 cpuIndex) {
#if defined(_GNU_SOURCE)
  
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpuIndex, &cpuset);
  
  v_int32 result = pthread_setaffinity_np(nativeHandle, sizeof(cpu_set_t), &cpuset);
  
  if (result != 0) {
    OATPP_LOGD("[oatpp::concurrency::Thread::assignThreadToCpu(...)]", "error code - %d", result);
  }
  
  return result;
#else
  return -1;
#endif
}
  
v_int32 Thread::calcHardwareConcurrency() {
#if !defined(OATPP_THREAD_HARDWARE_CONCURRENCY)
  v_int32 concurrency = std::thread::hardware_concurrency();
  if(concurrency == 0) {
    concurrency = 1;
  }
  return concurrency;
#else
  return OATPP_THREAD_HARDWARE_CONCURRENCY;
#endif
}
  
v_int32 Thread::getHardwareConcurrency() {
  static v_int32 concurrency = calcHardwareConcurrency();
  return concurrency;
}
  
}}

