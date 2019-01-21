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

#ifndef oatpp_data_buffer_FIFOBuffer_hpp
#define oatpp_data_buffer_FIFOBuffer_hpp

#include "./IOBuffer.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/os/io/Library.hpp"

namespace oatpp { namespace data{ namespace buffer {
  
class FIFOBuffer : public oatpp::base::Controllable {
public:
  OBJECT_POOL(FIFOBuffer_Pool, FIFOBuffer, 32)
  SHARED_OBJECT_POOL(Shared_FIFOBuffer_Pool, FIFOBuffer, 32)
private:
  bool m_canRead;
  os::io::Library::v_size m_readPosition;
  os::io::Library::v_size m_writePosition;
  IOBuffer m_buffer;
  oatpp::concurrency::SpinLock::Atom m_atom;
public:
  FIFOBuffer()
    : m_canRead(false)
    , m_readPosition(0)
    , m_writePosition(0)
    , m_atom(false)
  {}
public:
  
  static std::shared_ptr<FIFOBuffer> createShared(){
    return Shared_FIFOBuffer_Pool::allocateShared();
  }
  
  os::io::Library::v_size availableToRead();
  os::io::Library::v_size availableToWrite();
  
  os::io::Library::v_size read(void *data, os::io::Library::v_size count);
  os::io::Library::v_size write(const void *data, os::io::Library::v_size count);
  
};
  
}}}

#endif /* FIFOBuffer_hpp */
