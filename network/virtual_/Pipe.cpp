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

#include "Pipe.hpp"

namespace oatpp { namespace network { namespace virtual_ {
  
os::io::Library::v_size Pipe::Reader::read(void *data, os::io::Library::v_size count) {
  
  Pipe& pipe = *m_pipe;
  if(!pipe.m_alive) {
    return oatpp::data::stream::IOStream::ERROR_IO_PIPE;
  }
  
  if(m_nonBlocking) {
    oatpp::concurrency::SpinLock spinLock(pipe.m_atom);
    if(pipe.m_buffer.availableToRead() > 0) {
      auto result = pipe.read(data, count);
      pipe.m_writeCondition.notify_one();
      return result;
    } else {
      return oatpp::data::stream::IOStream::ERROR_IO_WAIT_RETRY;
    }
  }
  
  std::unique_lock<std::mutex> lock(pipe.m_readMutex);
  pipe.m_readCondition.wait(lock, [&pipe] {return (pipe.m_buffer.availableToRead() > 0 || !pipe.m_alive);});
  
  oatpp::concurrency::SpinLock spinLock(pipe.m_atom);
  
  if(!pipe.m_alive) {
    lock.unlock();
    pipe.m_writeCondition.notify_all();
    pipe.m_readCondition.notify_all();
    return oatpp::data::stream::IOStream::ERROR_IO_PIPE;
  }
  
  if(pipe.m_buffer.availableToRead() == 0) {
    return oatpp::data::stream::IOStream::ERROR_IO_RETRY;
  }
  
  auto result = pipe.read(data, count);
  
  lock.unlock();
  pipe.m_writeCondition.notify_one();
  return result;
  
}

os::io::Library::v_size Pipe::Writer::write(const void *data, os::io::Library::v_size count) {
  
  Pipe& pipe = *m_pipe;
  if(!pipe.m_alive) {
    return oatpp::data::stream::IOStream::ERROR_IO_PIPE;
  }
  
  if(m_nonBlocking) {
    oatpp::concurrency::SpinLock spinLock(pipe.m_atom);
    if(pipe.m_buffer.availableToWrite() > 0) {
      auto result = pipe.write(data, count);
      pipe.m_readCondition.notify_one();
      return result;
    } else {
      return oatpp::data::stream::IOStream::ERROR_IO_WAIT_RETRY;
    }
  }
  
  std::unique_lock<std::mutex> lock(pipe.m_writeMutex);
  pipe.m_writeCondition.wait(lock, [&pipe] {return (pipe.m_buffer.availableToWrite() > 0 || !pipe.m_alive);});
  
  oatpp::concurrency::SpinLock spinLock(pipe.m_atom);
  
  if(!pipe.m_alive) {
    lock.unlock();
    pipe.m_writeCondition.notify_all();
    pipe.m_readCondition.notify_all();
    return oatpp::data::stream::IOStream::ERROR_IO_PIPE;
  }
  
  if(pipe.m_buffer.availableToWrite() == 0) {
    return oatpp::data::stream::IOStream::ERROR_IO_RETRY;
  }
  
  auto result = pipe.write(data, count);
  
  lock.unlock();
  pipe.m_readCondition.notify_one();
  return result;
  
}
  
}}}
