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

void Pipe::Reader::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_ioMode = ioMode;
}

oatpp::data::stream::IOMode Pipe::Reader::getInputStreamIOMode() {
  return m_ioMode;
}

void Pipe::Reader::setMaxAvailableToRead(data::v_io_size maxAvailableToRead) {
  m_maxAvailableToRead = maxAvailableToRead;
}
  
data::v_io_size Pipe::Reader::read(void *data, data::v_io_size count) {
  
  if(m_maxAvailableToRead > -1 && count > m_maxAvailableToRead) {
    count = m_maxAvailableToRead;
  }
  
  Pipe& pipe = *m_pipe;
  oatpp::data::v_io_size result;
  
  if(m_ioMode == oatpp::data::stream::IOMode::NON_BLOCKING) {
    std::unique_lock<std::mutex> lock(pipe.m_mutex, std::try_to_lock);
    if(lock.owns_lock()) {
      if (pipe.m_fifo.availableToRead() > 0) {
        result = pipe.m_fifo.read(data, count);
      } else if (pipe.m_open) {
        result = data::IOError::WAIT_RETRY;
      } else {
        result = data::IOError::BROKEN_PIPE;
      }
    } else {
      result = data::IOError::WAIT_RETRY;
    }
  } else {
    std::unique_lock<std::mutex> lock(pipe.m_mutex);
    while (pipe.m_fifo.availableToRead() == 0 && pipe.m_open) {
      pipe.m_conditionRead.wait(lock);
    }
    if (pipe.m_fifo.availableToRead() > 0) {
      result = pipe.m_fifo.read(data, count);
    } else {
      result = data::IOError::BROKEN_PIPE;
    }
  }

  if(result > 0) {
    pipe.m_conditionWrite.notify_one();
    pipe.m_writer.notifyWaitList();
  }
  
  return result;
  
}

oatpp::async::Action Pipe::Reader::suggestInputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  switch (ioResult) {
    case oatpp::data::IOError::WAIT_RETRY: {
      std::unique_lock<std::mutex> lock(m_pipe->m_mutex);
      if (m_pipe->m_fifo.availableToRead() > 0 || !m_pipe->m_open) {
        return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
      }
      return oatpp::async::Action::createWaitListAction(&m_waitList);
    }
    case oatpp::data::IOError::RETRY:
      return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  throw std::runtime_error("[oatpp::network::virtual_::Pipe::Reader::suggestInputStreamAction()]: Error. Unable to suggest async action for I/O result.");

}

void Pipe::Reader::notifyWaitList() {
  m_waitList.notifyAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pipe::Writer::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_ioMode = ioMode;
}

oatpp::data::stream::IOMode Pipe::Writer::getOutputStreamIOMode() {
  return m_ioMode;
}

void Pipe::Writer::setMaxAvailableToWrite(data::v_io_size maxAvailableToWrite) {
  m_maxAvailableToWrtie = maxAvailableToWrite;
}
  
data::v_io_size Pipe::Writer::write(const void *data, data::v_io_size count) {
  
  if(m_maxAvailableToWrtie > -1 && count > m_maxAvailableToWrtie) {
    count = m_maxAvailableToWrtie;
  }

  Pipe& pipe = *m_pipe;
  oatpp::data::v_io_size result;
  
  if(m_ioMode == oatpp::data::stream::IOMode::NON_BLOCKING) {
    std::unique_lock<std::mutex> lock(pipe.m_mutex, std::try_to_lock);
    if(lock.owns_lock()) {
      if (pipe.m_fifo.availableToWrite() > 0) {
        result = pipe.m_fifo.write(data, count);
      } else if (pipe.m_open) {
        result = data::IOError::WAIT_RETRY;
      } else {
        result = data::IOError::BROKEN_PIPE;
      }
    } else {
      result = data::IOError::WAIT_RETRY;
    }
  } else {
    std::unique_lock<std::mutex> lock(pipe.m_mutex);
    while (pipe.m_fifo.availableToWrite() == 0 && pipe.m_open) {
      pipe.m_conditionWrite.wait(lock);
    }
    if (pipe.m_open && pipe.m_fifo.availableToWrite() > 0) {
      result = pipe.m_fifo.write(data, count);
    } else {
      result = data::IOError::BROKEN_PIPE;
    }
  }

  if(result > 0) {
    pipe.m_conditionRead.notify_one();
    pipe.m_reader.notifyWaitList();
  }
  
  return result;
  
}

oatpp::async::Action Pipe::Writer::suggestOutputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  switch (ioResult) {
    case oatpp::data::IOError::WAIT_RETRY: {
      std::unique_lock<std::mutex> lock(m_pipe->m_mutex);
      if (m_pipe->m_fifo.availableToWrite() > 0 || !m_pipe->m_open) {
        return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
      }
      return oatpp::async::Action::createWaitListAction(&m_waitList);
    }
    case oatpp::data::IOError::RETRY:
      return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  throw std::runtime_error("[oatpp::network::virtual_::Pipe::Writer::suggestOutputStreamAction()]: Error. Unable to suggest async action for I/O result.");

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
