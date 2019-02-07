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

#include "./Pipe.hpp"

namespace oatpp { namespace network { namespace virtual_ {
  
class Socket : public oatpp::base::Controllable, public oatpp::data::stream::IOStream {
private:
  std::shared_ptr<Pipe> m_pipeIn;
  std::shared_ptr<Pipe> m_pipeOut;
public:
  Socket(const std::shared_ptr<Pipe>& pipeIn, const std::shared_ptr<Pipe>& pipeOut)
    : m_pipeIn(pipeIn)
    , m_pipeOut(pipeOut)
  {}
public:
  
  static std::shared_ptr<Socket> createShared(const std::shared_ptr<Pipe>& pipeIn, const std::shared_ptr<Pipe>& pipeOut) {
    return std::make_shared<Socket>(pipeIn, pipeOut);
  }
  
  ~Socket() {
    close();
  }
  
  /**
   * this one used for testing purposes only
   * set to -1 in order to ignore this value
   */
  void setMaxAvailableToReadWrtie(data::v_io_size maxToRead, data::v_io_size maxToWrite);
  
  data::v_io_size read(void *data, data::v_io_size count) override;
  data::v_io_size write(const void *data, data::v_io_size count) override;
  
  void setNonBlocking(bool nonBlocking);
  
  void close();
  
};
  
}}}

#endif /* oatpp_network_virtual__Socket_hpp */
