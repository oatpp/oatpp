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

#include "./Stream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace data{ namespace stream {

const char* const Errors::ERROR_ASYNC_BROKEN_PIPE = "[oatpp::data::stream{}]: Error. AsyncIO. Broken pipe.";
const char* const Errors::ERROR_ASYNC_BAD_RESULT = "[oatpp::data::stream{}]: Error. AsyncIO. Bad result code. Operation returned 0.";
const char* const Errors::ERROR_ASYNC_UNKNOWN_CODE = "[oatpp::data::stream{}]: Error. AsyncIO. Unknown error code returned";
  
data::v_io_size OutputStream::writeAsString(v_int32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::int32ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

data::v_io_size OutputStream::writeAsString(v_int64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::int64ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

data::v_io_size OutputStream::writeAsString(v_float32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float32ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

data::v_io_size OutputStream::writeAsString(v_float64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float64ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}
  
data::v_io_size OutputStream::writeAsString(bool value) {
  if(value){
    return write("true", 4);
  } else {
    return write("false", 5);
  }
}
  
// Functions
  
OutputStream& operator << (OutputStream& s, const oatpp::String& str) {
  if(str) {
    s.write(str);
  } else {
    s.write("[<String(null)>]");
  }
  return s;
}

OutputStream& operator << (OutputStream& s, const Int8& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int8(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const Int16& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int16(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const Int32& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int32(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const Int64& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int64(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const Float32& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Float32(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const Float64& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Float64(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const Boolean& value) {
  if(value.getPtr()) { // use getPtr() here to avoid false to nullptr conversion
    return operator << (s, value->getValue());
  }
  s.write("[<Boolean(null)>]");
  return s;
}

OutputStream& operator << (OutputStream& s, const char* str) {
  if(str != nullptr) {
    s.write(str);
  } else {
    s.write("[<char*(null)>]");
  }
  return s;
}
  
OutputStream& operator << (OutputStream& s, v_int32 value) {
  s.writeAsString(value);
  return s;
}

OutputStream& operator << (OutputStream& s, v_int64 value) {
  s.writeAsString(value);
  return s;
}

OutputStream& operator << (OutputStream& s, v_float32 value) {
  s.writeAsString(value);
  return s;
}

OutputStream& operator << (OutputStream& s, v_float64 value) {
  s.writeAsString(value);
  return s;
}

OutputStream& operator << (OutputStream& s, bool value) {
  if(value) {
    s.OutputStream::write("true");
  } else {
    s.OutputStream::write("false");
  }
  return s;
}
  
oatpp::data::v_io_size transfer(const std::shared_ptr<InputStream>& fromStream,
                                        const std::shared_ptr<OutputStream>& toStream,
                                        oatpp::data::v_io_size transferSize,
                                        void* buffer,
                                        oatpp::data::v_io_size bufferSize) {
  
  oatpp::data::v_io_size progress = 0;
  
  while (transferSize == 0 || progress < transferSize) {
    oatpp::data::v_io_size desiredReadCount = transferSize - progress;
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
      if(readResult == data::IOError::RETRY || readResult == data::IOError::WAIT_RETRY) {
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
                                   oatpp::data::v_io_size transferSize,
                                   const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer) {
  
  class TransferCoroutine : public oatpp::async::Coroutine<TransferCoroutine> {
  private:
    std::shared_ptr<InputStream> m_fromStream;
    std::shared_ptr<OutputStream> m_toStream;
    oatpp::data::v_io_size m_transferSize;
    oatpp::data::v_io_size m_progress;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> m_buffer;
    
    oatpp::data::v_io_size m_desiredReadCount;
    void* m_readBufferPtr;
    const void* m_writeBufferPtr;
    oatpp::data::v_io_size m_bytesLeft;
    
  public:
    
    TransferCoroutine(const std::shared_ptr<InputStream>& fromStream,
                      const std::shared_ptr<OutputStream>& toStream,
                      oatpp::data::v_io_size transferSize,
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
      if(!error.isExceptionThrown) {
        if(m_transferSize == 0) {
          return finish();
        }
      }
      return error;
    }
    
  };
  
  return parentCoroutine->startCoroutine<TransferCoroutine>(actionOnReturn, fromStream, toStream, transferSize, buffer);
  
}

namespace {

  oatpp::async::Action asyncActionOnIOError(data::v_io_size res) {
    switch (res) {
      case IOError::WAIT_RETRY:
        return oatpp::async::Action::_WAIT_RETRY;
      case IOError::RETRY:
        return oatpp::async::Action::_REPEAT;
      case IOError::BROKEN_PIPE:
        return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_BROKEN_PIPE));
      case IOError::ZERO_VALUE:
        return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_BAD_RESULT));
    }
    return oatpp::async::Action(oatpp::async::Error(Errors::ERROR_ASYNC_UNKNOWN_CODE));
  }

}
  
oatpp::async::Action writeExactSizeDataAsyncInline(oatpp::data::stream::OutputStream* stream,
                                                   const void*& data,
                                                   data::v_io_size& size,
                                                   const oatpp::async::Action& nextAction) {
  if(size > 0) {
    auto res = stream->write(data, size);
    if(res > 0) {
      data = &((p_char8) data)[res];
      size -= res;
      if (size > 0) {
        return oatpp::async::Action::_REPEAT;
      }
    } else {
      return asyncActionOnIOError(res);
    }
  }
  return nextAction;

}

oatpp::async::Action readSomeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                             void*& data,
                                             data::v_io_size& size,
                                             const oatpp::async::Action& nextAction) {

  if(size > 0) {
    auto res = stream->read(data, size);
    if(res > 0) {
      data = &((p_char8) data)[res];
      size -= res;
    } else {
      return asyncActionOnIOError(res);
    }
  }
  return nextAction;

}

oatpp::async::Action readExactSizeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                                  void*& data,
                                                  data::v_io_size& size,
                                                  const oatpp::async::Action& nextAction) {
  if(size > 0) {
    auto res = stream->read(data, size);
    if(res > 0) {
      data = &((p_char8) data)[res];
      size -= res;
      if (size > 0) {
        return oatpp::async::Action::_REPEAT;
      }
    } else {
      return asyncActionOnIOError(res);
    }
  }
  return nextAction;
}
  
oatpp::data::v_io_size readExactSizeData(oatpp::data::stream::InputStream* stream, void* data, data::v_io_size size) {
  
  char* buffer = (char*) data;
  oatpp::data::v_io_size progress = 0;
  
  while (progress < size) {
    
    auto res = stream->read(&buffer[progress], size - progress);
    
    if(res > 0) {
      progress += res;
    } else { // if res == 0 then probably stream handles read() error incorrectly. return.
      if(res == data::IOError::RETRY || res == data::IOError::WAIT_RETRY) {
        continue;
      }
      return progress;
    }
    
  }
  
  return progress;
  
}
  
oatpp::data::v_io_size writeExactSizeData(oatpp::data::stream::OutputStream* stream, const void* data, data::v_io_size size) {
  
  const char* buffer = (char*)data;
  oatpp::data::v_io_size progress = 0;
  
  while (progress < size) {

    auto res = stream->write(&buffer[progress], size - progress);
    
    if(res > 0) {
      progress += res;
    } else { // if res == 0 then probably stream handles write() error incorrectly. return.
      if(res == data::IOError::RETRY || res == data::IOError::WAIT_RETRY) {
        continue;
      }
      return progress;
    }
    
  }
  
  return progress;
}
  
}}}
