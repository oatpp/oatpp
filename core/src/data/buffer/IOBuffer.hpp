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

#ifndef oatpp_data_buffer_IOBuffer_hpp
#define oatpp_data_buffer_IOBuffer_hpp

#include "../../base/memory/ObjectPool.hpp"
#include "../../base/Controllable.hpp"
#include "../../base/SharedWrapper.hpp"

namespace oatpp { namespace data{ namespace buffer {
  
class IOBuffer : public oatpp::base::Controllable {
public:
  OBJECT_POOL(IOBuffer_Pool, IOBuffer, 32)
  SHARED_OBJECT_POOL(Shared_IOBuffer_Pool, IOBuffer, 32)
public:
  static const v_int32 BUFFER_SIZE;
private:
  // TODO FastAlloc
  static oatpp::base::memory::ThreadDistributedMemoryPool& getBufferPool(){
    static oatpp::base::memory::ThreadDistributedMemoryPool pool("IOBuffer_Buffer_Pool", BUFFER_SIZE, 32);
    return pool;
  }
private:
  void* m_entry;
public:
  IOBuffer()
    : m_entry(getBufferPool().obtain())
  {}
public:
  
  static std::shared_ptr<IOBuffer> createShared(){
    return Shared_IOBuffer_Pool::allocateShared();
  }
  
  ~IOBuffer() {
    oatpp::base::memory::MemoryPool::free(m_entry);
  }
  
  void* getData(){
    return m_entry;
  }
  
  v_int32 getSize(){
    return BUFFER_SIZE;
  }
  
};
  
}}}

#endif /* oatpp_data_buffer_IOBuffer_hpp */
