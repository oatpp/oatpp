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

#include "Pipe.hpp"

namespace oatpp { namespace network { namespace virtual_ {

data::stream::DefaultInitializedContext Pipe::Reader::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_INFINITE);

void Pipe::Reader::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_ioMode = ioMode;
}

oatpp::data::stream::IOMode Pipe::Reader::getInputStreamIOMode() {
  return m_ioMode;
}

void Pipe::Reader::setMaxAvailableToRead(v_io_size maxAvailableToRead) {
  m_maxAvailableToRead = maxAvailableToRead;
}
  
v_io_size Pipe::Reader::read(void *data, v_buff_size count, async::Action& action) {
  
  if(m_maxAvailableToRead > -1 && count > m_maxAvailableToRead) {
    count = m_maxAvailableToRead;
  }
  
  Pipe& pipe = *m_pipe;
  oatpp::v_io_size result;
  
  if(m_ioMode == oatpp::data::stream::IOMode::ASYNCHRONOUS) {

    std::lock_guard<std::mutex> lock(pipe.m_mutex);

    if (pipe.m_fifo.availableToRead() > 0) {
      result = pipe.m_fifo.read(data, count);
    } else if (pipe.m_open) {
      action = async::Action::createWaitListAction(&m_waitList);
      result = IOError::RETRY_READ;
    } else {
      result = IOError::BROKEN_PIPE;
    }

  } else {
    std::unique_lock<std::mutex> lock(pipe.m_mutex);
    while (pipe.m_fifo.availableToRead() == 0 && pipe.m_open) {
      pipe.m_conditionRead.wait(lock);
    }
    if (pipe.m_fifo.availableToRead() > 0) {
      result = pipe.m_fifo.read(data, count);
    } else {
      result = IOError::BROKEN_PIPE;
    }
  }

  if(result > 0) {
    pipe.m_conditionWrite.notify_one();
    pipe.m_writer.notifyWaitList();
  }
  
  return result;
  
}

oatpp::data::stream::Context& Pipe::Reader::getInputStreamContext() {
  return DEFAULT_CONTEXT;
}

void Pipe::Reader::notifyWaitList() {
  m_waitList.notifyAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

data::stream::DefaultInitializedContext Pipe::Writer::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_INFINITE);

void Pipe::Writer::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_ioMode = ioMode;
}

oatpp::data::stream::IOMode Pipe::Writer::getOutputStreamIOMode() {
  return m_ioMode;
}

oatpp::data::stream::Context& Pipe::Writer::getOutputStreamContext() {
  return DEFAULT_CONTEXT;
}

void Pipe::Writer::setMaxAvailableToWrite(v_io_size maxAvailableToWrite) {
  m_maxAvailableToWrtie = maxAvailableToWrite;
}
  
v_io_size Pipe::Writer::write(const void *data, v_buff_size count, async::Action& action) {
  
  if(m_maxAvailableToWrtie > -1 && count > m_maxAvailableToWrtie) {
    count = m_maxAvailableToWrtie;
  }

  Pipe& pipe = *m_pipe;
  oatpp::v_io_size result;
  
  if(m_ioMode == oatpp::data::stream::IOMode::ASYNCHRONOUS) {

    std::lock_guard<std::mutex> lock(pipe.m_mutex);

    if(pipe.m_open) {
      if (pipe.m_fifo.availableToWrite() > 0) {
        result = pipe.m_fifo.write(data, count);
      } else {
        action = async::Action::createWaitListAction(&m_waitList);
        result = IOError::RETRY_WRITE;
      }
    } else {
      result = IOError::BROKEN_PIPE;
    }

  } else {
    std::unique_lock<std::mutex> lock(pipe.m_mutex);
    while (pipe.m_fifo.availableToWrite() == 0 && pipe.m_open) {
      pipe.m_conditionWrite.wait(lock);
    }
    if (pipe.m_open && pipe.m_fifo.availableToWrite() > 0) {
      result = pipe.m_fifo.write(data, count);
    } else {
      result = IOError::BROKEN_PIPE;
    }
  }

  if(result > 0) {
    pipe.m_conditionRead.notify_one();
    pipe.m_reader.notifyWaitList();
  }
  
  return result;
  
}

void Pipe::Writer::notifyWaitList() {
  m_waitList.notifyAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Pipe::Pipe()
  : m_open(true)
  , m_writer(this)
  , m_reader(this)
  , m_buffer()
  , m_fifo(m_buffer.getData(), m_buffer.getSize())
{}

std::shared_ptr<Pipe> Pipe::createShared(){
  return std::make_shared<Pipe>();
}

Pipe::~Pipe() {
  close();
}

Pipe::Writer* Pipe::getWriter() {
  return &m_writer;
}

Pipe::Reader* Pipe::getReader() {
  return &m_reader;
}

void Pipe::close() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_open = false;
  }
  m_conditionRead.notify_one();
  m_reader.notifyWaitList();
  m_conditionWrite.notify_one();
  m_writer.notifyWaitList();
}

}}}
