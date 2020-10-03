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

#ifndef oatpp_data_buffer_IOBuffer_hpp
#define oatpp_data_buffer_IOBuffer_hpp

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

namespace oatpp { namespace data{ namespace buffer {

/**
 * Predefined buffer implementation for I/O operations.
 * Allocates buffer bytes using &id:oatpp::base::memory::ThreadDistributedMemoryPool;.
 */
class IOBuffer : public oatpp::base::Countable {
public:
  OBJECT_POOL(IOBuffer_Pool, IOBuffer, 32)
  SHARED_OBJECT_POOL(Shared_IOBuffer_Pool, IOBuffer, 32)
public:
  /**
   * Buffer size constant.
   */
  static constexpr v_buff_size BUFFER_SIZE = 4096;
private:
  static oatpp::base::memory::ThreadDistributedMemoryPool& getBufferPool() {
    static auto pool = new oatpp::base::memory::ThreadDistributedMemoryPool("IOBuffer_Buffer_Pool", BUFFER_SIZE, 16);
    return *pool;
  }
private:
  void* m_entry;
public:
  /**
   * Constructor.
   */
  IOBuffer();
public:

  /**
   * Create shared IOBuffer.
   * @return
   */
  static std::shared_ptr<IOBuffer> createShared();

  /**
   * Virtual destructor.
   */
  ~IOBuffer();

  /**
   * Get pointer to buffer data.
   * @return
   */
  void* getData();

  /**
   * Get buffer size.
   * @return - should always return &l:IOBuffer::BUFFER_SIZE;.
   */
  v_buff_size getSize();
  
};
  
}}}

#endif /* oatpp_data_buffer_IOBuffer_hpp */
