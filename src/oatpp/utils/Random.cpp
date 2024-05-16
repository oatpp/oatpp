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

#include "Random.hpp"

namespace oatpp { namespace utils {

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  thread_local std::mt19937 Random::RANDOM_GENERATOR(std::random_device{}());
#else
  std::mt19937 Random::RANDOM_GENERATOR (std::random_device{}());
  oatpp::concurrency::SpinLock Random::RANDOM_LOCK;
#endif

void Random::randomBytes(p_char8 buffer, v_buff_size bufferSize) {

#if defined(OATPP_COMPAT_BUILD_NO_THREAD_LOCAL)
  std::lock_guard<oatpp::concurrency::SpinLock> randomLock(RANDOM_LOCK);
#endif

  std::uniform_int_distribution<size_t> distribution(0, 255);

  for(v_buff_size i = 0; i < bufferSize; i ++) {
    buffer[i] = static_cast<v_char8>(distribution(RANDOM_GENERATOR));
  }

}

}}
