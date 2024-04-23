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

#include "oatpp/base/Countable.hpp"

namespace oatpp { namespace data{ namespace buffer {

/**
 * Predefined buffer implementation for I/O operations.
 * Allocates buffer bytes using &id:oatpp::base::memory::ThreadDistributedMemoryPool;.
 */
class IOBuffer : public oatpp::base::Countable {
public:
  /**
   * Buffer size constant.
   */
  static const v_buff_size BUFFER_SIZE;
private:
  p_char8 m_entry;
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
  ~IOBuffer() override;

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
