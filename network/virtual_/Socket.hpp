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

#ifndef oatpp_network_virtual__Socket_hpp
#define oatpp_network_virtual__Socket_hpp

#include "Pipe.hpp"

namespace oatpp { namespace network { namespace virtual_ {
  
class Socket : public oatpp::base::Controllable, public oatpp::data::stream::IOStream {
public:
  OBJECT_POOL(Socket_Pool, Socket, 32)
  SHARED_OBJECT_POOL(Shared_Socket_Pool, Socket, 32)
private:
  std::shared_ptr<Pipe::Reader> m_pipeReader;
  std::shared_ptr<Pipe::Writer> m_pipeWriter;
public:
  
  os::io::Library::v_size read(void *data, os::io::Library::v_size count) override {
    if(m_pipeReader) {
      return m_pipeReader->read(data, count);
    }
    return -1;
  }
  
  os::io::Library::v_size write(const void *data, os::io::Library::v_size count) override {
    if(m_pipeWriter) {
      return m_pipeWriter->write(data, count);
    }
    return -1;
  }
  
  bool isConnected() {
    return m_pipeReader && m_pipeWriter;
  }
  
  void close() {
    m_pipeReader.reset();
    m_pipeWriter.reset();
  }
  
};
  
}}}

#endif /* oatpp_network_virtual__Socket_hpp */
