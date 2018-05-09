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

#ifndef oatpp_network_virtual__Pipe_hpp
#define oatpp_network_virtual__Pipe_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/data/buffer/FIFOBuffer.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace network { namespace virtual_ {

class Pipe : public oatpp::base::Controllable, public oatpp::data::stream::IOStream {
public:
  OBJECT_POOL(Pipe_Pool, Pipe, 32)
  SHARED_OBJECT_POOL(Shared_Pipe_Pool, Pipe, 32)
public:
  
  class Reader : public oatpp::base::Controllable, public oatpp::data::stream::InputStream {
  public:
    OBJECT_POOL(Pipe_Reader_Pool, Pipe, 32)
    SHARED_OBJECT_POOL(Shared_Pipe_Reader_Pool, Reader, 32)
  private:
    std::shared_ptr<Pipe> m_pipe;
    bool m_nonBlocking;
  public:
    Reader(const std::shared_ptr<Pipe>& pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
    {}
  public:
    
    static std::shared_ptr<Reader> createShared(const std::shared_ptr<Pipe>& pipe, bool nonBlocking = false){
      return Shared_Pipe_Reader_Pool::allocateShared(pipe, nonBlocking);
    }
    
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }
    
    os::io::Library::v_size read(void *data, os::io::Library::v_size count) override;
    
  };
  
  class Writer : public oatpp::base::Controllable, public oatpp::data::stream::OutputStream {
  public:
    OBJECT_POOL(Pipe_Writer_Pool, Pipe, 32)
    SHARED_OBJECT_POOL(Shared_Pipe_Writer_Pool, Writer, 32)
  private:
    std::shared_ptr<Pipe> m_pipe;
    bool m_nonBlocking;
  public:
    Writer(const std::shared_ptr<Pipe>& pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
    {}
  public:
    
    static std::shared_ptr<Writer> createShared(const std::shared_ptr<Pipe>& pipe, bool nonBlocking = false){
      return Shared_Pipe_Writer_Pool::allocateShared(pipe, nonBlocking);
    }
    
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }
    
    os::io::Library::v_size write(const void *data, os::io::Library::v_size count) override;
    
  };
  
private:
  oatpp::data::buffer::FIFOBuffer m_buffer;
  bool m_alive;
  oatpp::concurrency::SpinLock::Atom m_atom;
  std::mutex m_readMutex;
  std::condition_variable m_readCondition;
  std::mutex m_writeMutex;
  std::condition_variable m_writeCondition;
public:
  
  Pipe()
    : m_alive(true)
    , m_atom(false)
  {}
  
  std::shared_ptr<Reader> getReader(bool nonBlocking = false) {
    return Reader::createShared(getSharedPtr<Pipe>(), nonBlocking);
  }
  
  std::shared_ptr<Writer> getWriter(bool nonBlocking = false) {
    return Writer::createShared(getSharedPtr<Pipe>(), nonBlocking);
  }
  
};
  
}}}

#endif /* oatpp_network_virtual__Pipe_hpp */
