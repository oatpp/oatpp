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

#ifndef oatpp_async_utils_Semaphore_hpp
#define oatpp_async_utils_Semaphore_hpp

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace async { namespace utils {

/**
 * @brief Semaphore.
 */
class Semaphore {
public:
  explicit Semaphore(size_t count = 0) {
    m_count = count;
  }

  ~Semaphore() = default;

  void post(size_t n = 1) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);
    m_count += n;
    if (n == 1) {
      m_cond.notify_one();
    } else {
      m_cond.notify_all();
    }
  }

  void wait() {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);
    while (m_count == 0) {
      m_cond.wait(lock);
    }
    --m_count;
  }

private:
  size_t m_count;
  std::recursive_mutex m_mutex;
  std::condition_variable_any m_cond;
};

}}}

#endif /* oatpp_async_utils_Semaphore_hpp */
