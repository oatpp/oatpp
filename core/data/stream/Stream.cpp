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

#include "./Stream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace data{ namespace stream {
  
const char* const Errors::ERROR_ASYNC_FAILED_TO_WRITE_DATA = "ERROR_ASYNC_FAILED_TO_WRITE_DATA";
const char* const Errors::ERROR_ASYNC_FAILED_TO_READ_DATA = "ERROR_ASYNC_FAILED_TO_READ_DATA";
  
os::io::Library::v_size OutputStream::writeAsString(v_int32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::int32ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

os::io::Library::v_size OutputStream::writeAsString(v_int64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::int64ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

os::io::Library::v_size OutputStream::writeAsString(v_float32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float32ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

os::io::Library::v_size OutputStream::writeAsString(v_float64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float64ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}
  
os::io::Library::v_size OutputStream::writeAsString(bool value) {
  if(value){
    return write("true", 4);
  } else {
    return write("false", 5);
  }
}
  
// Functions
  
const std::shared_ptr<OutputStream>& operator <<
(const std::shared_ptr<OutputStream>& s, const oatpp::String& str) {
  s->write(str);
  return s;
}

const std::shared_ptr<OutputStream>& operator <<
(const std::shared_ptr<OutputStream>& s, const char* str) {
  s->write(str);
  return s;
}
  
const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_int32 value) {
  s->writeAsString(value);
  return s;
}

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_int64 value) {
  s->writeAsString(value);
  return s;
}

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_float32 value) {
  s->writeAsString(value);
  return s;
}

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_float64 value) {
  s->writeAsString(value);
  return s;
}

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, bool value) {
  if(value) {
    s->OutputStream::write("true");
  } else {
    s->OutputStream::write("false");
  }
  return s;
}
  
oatpp::os::io::Library::v_size transfer(const std::shared_ptr<InputStream>& fromStream,
                                        const std::shared_ptr<OutputStream>& toStream,
                                        oatpp::os::io::Library::v_size transferSize,
                                        void* buffer,
                                        oatpp::os::io::Library::v_size bufferSize) {
  
  oatpp::os::io::Library::v_size progress = 0;
  
  while (transferSize == 0 || progress < transferSize) {
    oatpp::os::io::Library::v_size desiredReadCount = transferSize - progress;
    if(transferSize == 0 || desiredReadCount > bufferSize){
      desiredReadCount = bufferSize;
    }
    auto readResult = fromStream->read(buffer, desiredReadCount);
    if(readResult > 0) {
      auto writeReasul = writeExactSizeData(toStream.get(), buffer, readResult);
      if(writeReasul != readResult) {
        throw std::runtime_error("[oatpp::data::stream::transfer()]: Unknown Error. Can't continue transfer.");
      }
      progress += readResult;
    } else {
      if(readResult == oatpp::data::stream::Errors::ERROR_IO_RETRY || readResult == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
        continue;
      }
      return progress;
    }
  }
  
  return progress;
  
}
  
oatpp::async::Action transferAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   const oatpp::async::Action& actionOnReturn,
                                   const std::shared_ptr<InputStream>& fromStream,
                                   const std::shared_ptr<OutputStream>& toStream,
                                   oatpp::os::io::Library::v_size transferSize,
                                   const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer) {
  
  class TransferCoroutine : public oatpp::async::Coroutine<TransferCoroutine> {
  private:
    std::shared_ptr<InputStream> m_fromStream;
    std::shared_ptr<OutputStream> m_toStream;
    oatpp::os::io::Library::v_size m_transferSize;
    oatpp::os::io::Library::v_size m_progress;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> m_buffer;
    
    oatpp::os::io::Library::v_size m_desiredReadCount;
    void* m_readBufferPtr;
    const void* m_writeBufferPtr;
    oatpp::os::io::Library::v_size m_bytesLeft;
    
  public:
    
    TransferCoroutine(const std::shared_ptr<InputStream>& fromStream,
                      const std::shared_ptr<OutputStream>& toStream,
                      oatpp::os::io::Library::v_size transferSize,
                      const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
      : m_fromStream(fromStream)
      , m_toStream(toStream)
      , m_transferSize(transferSize)
      , m_progress(0)
      , m_buffer(buffer)
    {}
    
    Action act() override {
      /* m_transferSize == 0 - is a legal state. */
      if(m_transferSize > 0) {
        if(m_progress == m_transferSize) {
          return finish();
        } else if(m_progress > m_transferSize) {
          throw std::runtime_error("[oatpp::data::stream::transferAsync{TransferCoroutine::act()}]: Invalid state. m_progress > m_transferSize");
        }
      } else if(m_transferSize < 0) {
        throw std::runtime_error("[oatpp::data::stream::transferAsync{TransferCoroutine::act()}]: Invalid state. m_transferSize < 0");
      }
      
      m_desiredReadCount = m_transferSize - m_progress;
      if(m_transferSize == 0 || m_desiredReadCount > m_buffer->getSize()){
        m_desiredReadCount = m_buffer->getSize();
      }
      
      m_readBufferPtr = m_buffer->getData();
      m_writeBufferPtr = m_buffer->getData();
      m_bytesLeft = m_desiredReadCount;
      
      return yieldTo(&TransferCoroutine::doRead);
    }
    
    Action doRead() {
      return oatpp::data::stream::readSomeDataAsyncInline(m_fromStream.get(),
                                                          m_readBufferPtr,
                                                          m_bytesLeft,
                                                          yieldTo(&TransferCoroutine::prepareWrite));
    }
    
    Action prepareWrite() {
      m_bytesLeft = m_desiredReadCount - m_bytesLeft;
      m_progress += m_bytesLeft;
      return yieldTo(&TransferCoroutine::doWrite);
    }
    
    Action doWrite() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(m_toStream.get(),
                                                                m_writeBufferPtr,
                                                                m_bytesLeft,
                                                                yieldTo(&TransferCoroutine::act));
    }
    
    Action handleError(const oatpp::async::Error& error) override {
      if(!error.isExceptionThrown && error.message == Errors::ERROR_ASYNC_FAILED_TO_READ_DATA) {
        if(m_transferSize == 0) {
          return finish();
        }
      }
      return error;
    }
    
  };
  
  return parentCoroutine->startCoroutine<TransferCoroutine>(actionOnReturn, fromStream, toStream, transferSize, buffer);
  
}
  
oatpp::async::Action writeSomeDataAsyncInline(oatpp::data::stream::OutputStream* stream,
                                              const void*& data,
                                              os::io::Library::v_size& size,
                                              const oatpp::async::Action& nextAction) {
  auto res = stream->write(data, size);
  if(res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
    return oatpp::async::Action::_WAIT_RETRY;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY) {
    return oatpp::async::Action::_REPEAT;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_PIPE) {
    return oatpp::async::Action::_ABORT;
  } else if(res < 0) {
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_FAILED_TO_WRITE_DATA));
  }
  data = &((p_char8) data)[res];
  size = size - res;
  if(res < size && res > 0) {
    return oatpp::async::Action::_REPEAT;
  }
  return nextAction;
}
  
oatpp::async::Action writeExactSizeDataAsyncInline(oatpp::data::stream::OutputStream* stream,
                                              const void*& data,
                                              os::io::Library::v_size& size,
                                              const oatpp::async::Action& nextAction) {
  auto res = stream->write(data, size);
  if(res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
    return oatpp::async::Action::_WAIT_RETRY;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY) {
    return oatpp::async::Action::_REPEAT;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_PIPE) {
    return oatpp::async::Action::_ABORT;
  } else if(res < 0) {
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_FAILED_TO_WRITE_DATA));
  } else if(res == 0) {
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_FAILED_TO_WRITE_DATA));
  }
  data = &((p_char8) data)[res];
  size = size - res;
  if(res < size) {
    return oatpp::async::Action::_REPEAT;
  }
  return nextAction;
}

oatpp::async::Action readSomeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                             void*& data,
                                             os::io::Library::v_size& bytesLeftToRead,
                                             const oatpp::async::Action& nextAction) {
  auto res = stream->read(data, bytesLeftToRead);
  if(res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
    return oatpp::async::Action::_WAIT_RETRY;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY) {
    return oatpp::async::Action::_REPEAT;
  } else if( res < 0) {
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_FAILED_TO_READ_DATA));
  } 
  // res == 0 is not an error here
  data = &((p_char8) data)[res];
  bytesLeftToRead -= res;
  return nextAction;
}

oatpp::async::Action readExactSizeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                                  void*& data,
                                                  os::io::Library::v_size& bytesLeftToRead,
                                                  const oatpp::async::Action& nextAction) {
  auto res = stream->read(data, bytesLeftToRead);
  if(res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
    return oatpp::async::Action::_WAIT_RETRY;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY) {
    return oatpp::async::Action::_REPEAT;
  } else if(res == oatpp::data::stream::Errors::ERROR_IO_PIPE) {
    return oatpp::async::Action::_ABORT;
  } else if( res < 0) {
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_FAILED_TO_READ_DATA));
  } else if(res == 0) { // Connection is probably closed or eof is reached
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_FAILED_TO_READ_DATA));
  }
  data = &((p_char8) data)[res];
  bytesLeftToRead -= res;
  if(res < bytesLeftToRead) {
    return oatpp::async::Action::_REPEAT;
  }
  return nextAction;
}
  
oatpp::os::io::Library::v_size readExactSizeData(oatpp::data::stream::InputStream* stream, void* data, os::io::Library::v_size size) {
  
  char* buffer = (char*) data;
  oatpp::os::io::Library::v_size progress = 0;
  
  while (progress < size) {
    
    auto res = stream->read(&buffer[progress], size - progress);
    
    if(res > 0) {
      progress += res;
    } else { // if res == 0 then probably stream handles read() error incorrectly. return.
      if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY || res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
        continue;
      }
      return progress;
    }
    
  }
  
  return progress;
  
}
  
oatpp::os::io::Library::v_size writeExactSizeData(oatpp::data::stream::OutputStream* stream, const void* data, os::io::Library::v_size size) {
  
  const char* buffer = (char*)data;
  oatpp::os::io::Library::v_size progress = 0;
  
  while (progress < size) {
    
    auto res = stream->write(&buffer[progress], size - progress);
    
    if(res > 0) {
      progress += res;
    } else { // if res == 0 then probably stream handles write() error incorrectly. return.
      if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY || res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
        continue;
      }
      return progress;
    }
    
  }
  
  return progress;
}
  
}}}
