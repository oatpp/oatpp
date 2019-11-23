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

#ifndef oatpp_utils_Random_hpp
#define oatpp_utils_Random_hpp

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/Types.hpp"
#include <random>

namespace oatpp { namespace utils { namespace random {

/**
 * Utility class for random values.
 */
class Random {
private:
#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  static thread_local std::mt19937 RANDOM_GENERATOR;
#else
  static std::mt19937 RANDOM_GENERATOR;
  static oatpp::concurrency::SpinLock RANDOM_LOCK;
#endif
public:

  /**
   * Fill in buffer with random bytes [0..255].
   * @param buffer - pointer to buffer.
   * @param bufferSize - size of the buffer.
   */
  static void randomBytes(p_char8 buffer, v_buff_size bufferSize);

};

}}}

#endif // oatpp_utils_Random_hpp
