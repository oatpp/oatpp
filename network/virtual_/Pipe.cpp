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
  
void Pipe::Reader::setMaxAvailableToRead(os::io::Library::v_size maxAvailableToRead) {
  m_maxAvailableToRead = maxAvailableToRead;
}
  
os::io::Library::v_size Pipe::Reader::read(void *data, os::io::Library::v_size count) {
  
  if(m_maxAvailableToRead > -1 && count > m_maxAvailableToRead) {
    count = m_maxAvailableToRead;
  }
  
  Pipe& pipe = *m_pipe;
  oatpp::os::io::Library::v_size result;
  
  if(m_nonBlocking) {
    if(pipe.m_buffer.availableToRead() > 0) {
      result = pipe.m_buffer.read(data, count);
    } else if(pipe.m_open) {
      result = oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY;
    } else {
      result = oatpp::data::stream::Errors::ERROR_IO_PIPE;
    }
  } else {
    std::unique_lock<std::mutex> lock(pipe.m_mutex);
    while (pipe.m_buffer.availableToRead() == 0 && pipe.m_open) {
      pipe.m_conditionWrite.notify_one();
      pipe.m_conditionRead.wait(lock);
    }
    if (pipe.m_buffer.availableToRead() > 0) {
      result = pipe.m_buffer.read(data, count);
    } else {
      result = oatpp::data::stream::Errors::ERROR_IO_PIPE;
    }
  }
  
  pipe.m_conditionWrite.notify_one();
  
  return result;
  
}

void Pipe::Writer::setMaxAvailableToWrite(os::io::Library::v_size maxAvailableToWrite) {
  m_maxAvailableToWrtie = maxAvailableToWrite;
}
  
os::io::Library::v_size Pipe::Writer::write(const void *data, os::io::Library::v_size count) {
  
  if(m_maxAvailableToWrtie > -1 && count > m_maxAvailableToWrtie) {
    count = m_maxAvailableToWrtie;
  }
  
  Pipe& pipe = *m_pipe;
  oatpp::os::io::Library::v_size result;
  
  if(m_nonBlocking) {
    if(pipe.m_buffer.availableToWrite() > 0) {
      result = pipe.m_buffer.write(data, count);
    } else if(pipe.m_open) {
      result = oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY;
    } else {
      result = oatpp::data::stream::Errors::ERROR_IO_PIPE;
    }
  } else {
    std::unique_lock<std::mutex> lock(pipe.m_mutex);
    while (pipe.m_buffer.availableToWrite() == 0 && pipe.m_open) {
      pipe.m_conditionRead.notify_one();
      pipe.m_conditionWrite.wait(lock);
    }
    if (pipe.m_open && pipe.m_buffer.availableToWrite() > 0) {
      result = pipe.m_buffer.write(data, count);
    } else {
      result = oatpp::data::stream::Errors::ERROR_IO_PIPE;
    }
  }
  
  pipe.m_conditionRead.notify_one();
  
  return result;
  
}
  
}}}
