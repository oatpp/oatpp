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

class Pipe : public oatpp::base::Controllable {
public:
  
  class Reader : public oatpp::data::stream::InputStream {
  private:
    Pipe* m_pipe;
    bool m_nonBlocking;
    
    /**
     * this one used for testing purposes only
     */
    os::io::Library::v_size m_maxAvailableToRead;
  public:
    
    Reader(Pipe* pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
      , m_maxAvailableToRead(-1)
    {}
    
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }
    
    /**
     * this one used for testing purposes only
     * set to -1 in order to ignore this value
     */
    void setMaxAvailableToRead(os::io::Library::v_size maxAvailableToRead);
    
    os::io::Library::v_size read(void *data, os::io::Library::v_size count) override;
    
  };
  
  class Writer : public oatpp::data::stream::OutputStream {
  private:
    Pipe* m_pipe;
    bool m_nonBlocking;
    
    /**
     * this one used for testing purposes only
     */
    os::io::Library::v_size m_maxAvailableToWrtie;
  public:
    
    Writer(Pipe* pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
      , m_maxAvailableToWrtie(-1)
    {}
    
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }
    
    /**
     * this one used for testing purposes only
     * set to -1 in order to ignore this value
     */
    void setMaxAvailableToWrite(os::io::Library::v_size maxAvailableToWrite);
    
    os::io::Library::v_size write(const void *data, os::io::Library::v_size count) override;
    
  };
  
private:
  bool m_open;
  Writer m_writer;
  Reader m_reader;
  oatpp::data::buffer::FIFOBuffer m_buffer;
  std::mutex m_mutex;
  std::condition_variable m_conditionRead;
  std::condition_variable m_conditionWrite;
public:
  
  Pipe()
    : m_open(true)
    , m_writer(this)
    , m_reader(this)
  {}
  
  static std::shared_ptr<Pipe> createShared(){
    return std::make_shared<Pipe>();
  }
  
  Writer* getWriter() {
    return &m_writer;
  }
  
  Reader* getReader() {
    return &m_reader;
  }
  
  void close() {
    m_open = false;
  }
  
};
  
}}}

#endif /* oatpp_network_virtual__Pipe_hpp */
