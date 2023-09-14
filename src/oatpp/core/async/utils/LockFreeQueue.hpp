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

#ifndef oatpp_async_utils_LockFreeQueue_hpp
#define oatpp_async_utils_LockFreeQueue_hpp

#include <atomic>

namespace oatpp { namespace async { namespace utils {

/**
 * @brief This class template represents a lock-free multiple producers single
 * consumer queue
 *
 * @tparam T The type of the items in the queue.
 */
template<typename T>
class LockFreeQueue {
public:
  LockFreeQueue() : m_head(new BufferNode), m_tail(m_head.load(std::memory_order_relaxed)) {}

  ~LockFreeQueue() {
    T item;
    while(pop(item)) {}
    BufferNode* front = m_head.load(std::memory_order_relaxed);
    delete front;
  }

  /**
     * @brief Push a item into the queue.
     *
     * @param item
     * @note This method can be called in multiple threads.
   */
  void push(T&& item) {
    BufferNode* node{new BufferNode(std::move(item))};
    BufferNode* prevHead{m_head.exchange(node, std::memory_order_acq_rel)};
    prevHead->m_next.store(node, std::memory_order_release);
  }

  /**
     * @brief Push a item into the queue.
     *
     * @param item
     * @note This method can be called in multiple threads.
   */
  void push(const T& item) {
    BufferNode* node{new BufferNode(std::move(item))};
    BufferNode* prevHead{m_head.exchange(node, std::memory_order_acq_rel)};
    prevHead->m_next.store(node, std::memory_order_release);
  }

  /**
     * @brief Pop a item from the queue.
     *
     * @param item
     * @return false if the queue is empty.
     * @note This method must be called in a single thread.
   */
  bool pop(T& item) {
    BufferNode* tail = m_tail.load(std::memory_order_relaxed);
    BufferNode* next = tail->m_next.load(std::memory_order_acquire);

    if(next == nullptr) {
      return false;
    }
    item = std::move(*(next->m_dataPtr));
    delete next->m_dataPtr;
    m_tail.store(next, std::memory_order_release);
    delete tail;
    return true;
  }

  bool empty() {
    BufferNode* tail = m_tail.load(std::memory_order_relaxed);
    BufferNode *next = tail->m_next.load(std::memory_order_acquire);
    return next == nullptr;
  }

private:
  struct BufferNode {
    BufferNode() = default;
    BufferNode(const T &data) : m_dataPtr(new T(data)) {}
    BufferNode(T &&data) : m_dataPtr(new T(std::move(data))) {}
    T* m_dataPtr;
    std::atomic<BufferNode*> m_next{nullptr};
  };

  std::atomic<BufferNode*> m_head{nullptr};
  std::atomic<BufferNode*> m_tail{nullptr};
};

}}}

#endif
