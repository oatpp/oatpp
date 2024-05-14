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

#include "Utils.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace concurrency {

v_int32 Utils::setThreadAffinityToOneCpu(std::thread::native_handle_type nativeHandle, v_int32 cpuIndex) {
  return setThreadAffinityToCpuRange(nativeHandle, cpuIndex, cpuIndex);
}

v_int32 Utils::setThreadAffinityToCpuRange(std::thread::native_handle_type nativeHandle, v_int32 firstCpuIndex, v_int32 lastCpuIndex) {
#if defined(_GNU_SOURCE)

  // NOTE:

  // The below line doesn't compile on Android.
  //result = pthread_setaffinity_np(nativeHandle, sizeof(cpu_set_t), &cpuset);

  // The below line compiles on Android but has not been tested.
  //result = sched_setaffinity(nativeHandle, sizeof(cpu_set_t), &cpuset);

  #if !defined(__ANDROID__) && !defined(OATPP_COMPAT_BUILD_NO_SET_AFFINITY)

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    for(v_int32 i = firstCpuIndex; i <= lastCpuIndex; i++) {
      CPU_SET(static_cast<size_t>(i), &cpuset);
    }

    v_int32 result = pthread_setaffinity_np(nativeHandle, sizeof(cpu_set_t), &cpuset);

    if (result != 0) {
      OATPP_LOGd("[oatpp::concurrency::Thread::assignThreadToCpu(...)]", "error code - {}", result)
    }

    return result;

  #else
    return -1;
  #endif

#else
  (void)nativeHandle;
  (void)firstCpuIndex;
  (void)lastCpuIndex;
  return -1;
#endif
}

v_int32 Utils::calcHardwareConcurrency() {
#if !defined(OATPP_THREAD_HARDWARE_CONCURRENCY)
  v_int32 concurrency = static_cast<v_int32>(std::thread::hardware_concurrency());
  if(concurrency == 0) {
    OATPP_LOGd("[oatpp::concurrency:Thread::calcHardwareConcurrency()]", "Warning - failed to get hardware_concurrency. Setting hardware_concurrency=1")
    concurrency = 1;
  }
  return concurrency;
#else
  return OATPP_THREAD_HARDWARE_CONCURRENCY;
#endif
}

v_int32 Utils::getHardwareConcurrency() {
  static v_int32 concurrency = calcHardwareConcurrency();
  return concurrency;
}

}}
