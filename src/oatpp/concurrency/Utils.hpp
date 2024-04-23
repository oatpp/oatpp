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

#ifndef oatpp_concurrency_Utils_hpp
#define oatpp_concurrency_Utils_hpp

#include "oatpp/Environment.hpp"
#include <thread>

namespace oatpp { namespace concurrency {

class Utils {
private:
  static v_int32 calcHardwareConcurrency();
public:

  /**
   * Set thread affinity to one CPU.
   * @param nativeHandle - `std::thread::native_handle_type`.
   * @param cpuIndex - index of CPU.
   * @return - zero on success. Negative value on failure.
   * -1 if platform that runs application does not support this call.
   */
  static v_int32 setThreadAffinityToOneCpu(std::thread::native_handle_type nativeHandle, v_int32 cpuIndex);

  /**
   * Set thread affinity [firstCpuIndex..lastCpuIndex].
   * @param nativeHandle - `std::thread::native_handle_type`.
   * @param firstCpuIndex - from CPU-index.
   * @param lastCpuIndex - to CPU-index included.
   * @return - zero on success. Negative value on failure.
   * -1 if platform that runs application does not support this call.
   */
  static v_int32 setThreadAffinityToCpuRange(std::thread::native_handle_type nativeHandle, v_int32 firstCpuIndex, v_int32 lastCpuIndex);

  /**
   * Get hardware concurrency.
   * @return - OATPP_THREAD_HARDWARE_CONCURRENCY config value if set <br>
   * else return std::thread::hardware_concurrency() <br>
   * else return 1. <br>
   */
  static v_int32 getHardwareConcurrency();

};

}}

#endif //oatpp_concurrency_Utils_hpp
