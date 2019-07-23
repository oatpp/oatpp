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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConsistentOutputStream

oatpp::async::Action ConsistentOutputStream::suggestOutputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  OATPP_LOGE("[oatpp::data::stream::ConsistentOutputStream::suggestOutputStreamAction()]", "Error. ioResult=%d", ioResult);

  const char* message =
    "Error. ConsistentOutputStream::suggestOutputStreamAction() method is called with (ioResult <= 0).\n"
    "ConsistentOutputStream should always fully satisfy call to write() method. Conceptual error.";

  throw std::runtime_error(message);

}

data::v_io_size ConsistentOutputStream::writeAsString(v_int32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::int32ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

data::v_io_size ConsistentOutputStream::writeAsString(v_int64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::int64ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

data::v_io_size ConsistentOutputStream::writeAsString(v_float32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float32ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}

data::v_io_size ConsistentOutputStream::writeAsString(v_float64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float64ToCharSequence(value, &a[0]);
  if(size > 0){
    return write(&a[0], size);
  }
  return 0;
}
  
data::v_io_size ConsistentOutputStream::writeAsString(bool value) {
  if(value){
    return write("true", 4);
  } else {
    return write("false", 5);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncInlineWriteData

AsyncInlineWriteData::AsyncInlineWriteData()
  : currBufferPtr(nullptr)
  , bytesLeft(0)
{}

AsyncInlineWriteData::AsyncInlineWriteData(const void* data, data::v_io_size size)
  : currBufferPtr(data)
  , bytesLeft(size)
{}

void AsyncInlineWriteData::set(const void* data, data::v_io_size size) {
  currBufferPtr = data;
  bytesLeft = size;
}

void AsyncInlineWriteData::inc(data::v_io_size amount) {
  currBufferPtr = &((p_char8) currBufferPtr)[amount];
  bytesLeft -= amount;
}

void AsyncInlineWriteData::setEof() {
  currBufferPtr = &((p_char8) currBufferPtr)[bytesLeft];
  bytesLeft = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncInlineReadData

AsyncInlineReadData::AsyncInlineReadData()
  : currBufferPtr(nullptr)
  , bytesLeft(0)
{}

AsyncInlineReadData::AsyncInlineReadData(void* data, data::v_io_size size)
  : currBufferPtr(data)
  , bytesLeft(size)
{}

void AsyncInlineReadData::set(void* data, data::v_io_size size) {
  currBufferPtr = data;
  bytesLeft = size;
}

void AsyncInlineReadData::inc(data::v_io_size amount) {
  currBufferPtr = &((p_char8) currBufferPtr)[amount];
  bytesLeft -= amount;
}

void AsyncInlineReadData::setEof() {
  currBufferPtr = &((p_char8) currBufferPtr)[bytesLeft];
  bytesLeft = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncWriteCallbackWithCoroutineStarter

oatpp::async::Action AsyncWriteCallbackWithCoroutineStarter::writeAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                                              AsyncInlineWriteData& inlineData,
                                                                              oatpp::async::Action&& nextAction)
{
  auto coroutineStarter = writeAsync(inlineData.currBufferPtr, inlineData.bytesLeft);
  inlineData.setEof();
  return coroutineStarter.next(std::forward<async::Action>(nextAction));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DefaultWriteCallback

DefaultWriteCallback::DefaultWriteCallback(OutputStream* stream)
  : m_stream(stream)
{}

data::v_io_size DefaultWriteCallback::write(const void *data, data::v_io_size count) {
  return writeExactSizeData(m_stream, data, count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DefaultAsyncWriteCallback

DefaultAsyncWriteCallback::DefaultAsyncWriteCallback(const std::shared_ptr<OutputStream>& stream)
  : m_stream(stream)
{}

oatpp::async::Action DefaultAsyncWriteCallback::writeAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                                 AsyncInlineWriteData& inlineData,
                                                                 oatpp::async::Action&& nextAction)
{
  return writeExactSizeDataAsyncInline(coroutine, m_stream.get(), inlineData, std::forward<oatpp::async::Action>(nextAction));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions


ConsistentOutputStream& operator << (ConsistentOutputStream& s, const oatpp::String& str) {
  if(str) {
    s.write(str);
  } else {
    s.write("[<String(null)>]");
  }
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int8& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int8(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int16& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int16(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int32& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int64& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Int64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float32& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Float32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float64& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.write("[<Float64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Boolean& value) {
  if(value.getPtr()) { // use getPtr() here to avoid false to nullptr conversion
    return operator << (s, value->getValue());
  }
  s.write("[<Boolean(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const char* str) {
  if(str != nullptr) {
    s.write(str);
  } else {
    s.write("[<char*(null)>]");
  }
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_int32 value) {
  s.writeAsString(value);
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_int64 value) {
  s.writeAsString(value);
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_float32 value) {
  s.writeAsString(value);
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_float64 value) {
  s.writeAsString(value);
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, bool value) {
  if(value) {
    s.OutputStream::write("true");
  } else {
    s.OutputStream::write("false");
  }
  return s;
}
  
oatpp::data::v_io_size transfer(InputStream* fromStream,
                                WriteCallback* writeCallback,
                                oatpp::data::v_io_size transferSize,
                                void* buffer,
                                oatpp::data::v_io_size bufferSize)
{
  
  oatpp::data::v_io_size progress = 0;
  
  while (transferSize == 0 || progress < transferSize) {
    oatpp::data::v_io_size desiredReadCount = transferSize - progress;
    if(transferSize == 0 || desiredReadCount > bufferSize){
      desiredReadCount = bufferSize;
    }
    auto readResult = fromStream->read(buffer, desiredReadCount);
    if(readResult > 0) {

      p_char8 data = (p_char8) buffer;
      oatpp::data::v_io_size bytesLeft = readResult;
      while(bytesLeft > 0) {
        auto writeResult = writeCallback->write(data, bytesLeft);
        if(writeResult > 0) {
          data = &data[writeResult];
          bytesLeft -= writeResult;
        } else if (writeResult == data::IOError::RETRY || writeResult == data::IOError::WAIT_RETRY) {
          continue;
        } else {
          throw std::runtime_error("[oatpp::data::stream::transfer()]: Unknown Error. Can't continue transfer.");
        }
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

oatpp::async::CoroutineStarter transferAsync(const std::shared_ptr<InputStream>& fromStream,
                                             const std::shared_ptr<AsyncWriteCallback>& writeCallback,
                                             oatpp::data::v_io_size transferSize,
                                             const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
{
  
  class TransferCoroutine : public oatpp::async::Coroutine<TransferCoroutine> {
  private:
    std::shared_ptr<InputStream> m_fromStream;
    std::shared_ptr<AsyncWriteCallback> m_writeCallback;
    oatpp::data::v_io_size m_transferSize;
    oatpp::data::v_io_size m_progress;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> m_buffer;
    
    oatpp::data::v_io_size m_desiredReadCount;

    AsyncInlineReadData m_inlineReadData;
    AsyncInlineWriteData m_inlineWriteData;
    
  public:
    
    TransferCoroutine(const std::shared_ptr<InputStream>& fromStream,
                      const std::shared_ptr<AsyncWriteCallback>& writeCallback,
                      oatpp::data::v_io_size transferSize,
                      const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
      : m_fromStream(fromStream)
      , m_writeCallback(writeCallback)
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
          return error<AsyncTransferError>("[oatpp::data::stream::transferAsync{TransferCoroutine::act()}]: Invalid state. m_progress > m_transferSize");
        }
      } else if(m_transferSize < 0) {
        return error<AsyncTransferError>("[oatpp::data::stream::transferAsync{TransferCoroutine::act()}]: Invalid state. m_transferSize < 0");
      }
      
      m_desiredReadCount = m_transferSize - m_progress;
      if(m_transferSize == 0 || m_desiredReadCount > m_buffer->getSize()){
        m_desiredReadCount = m_buffer->getSize();
      }

      m_inlineReadData.set(m_buffer->getData(), m_desiredReadCount);
      
      return yieldTo(&TransferCoroutine::doRead);
    }
    
    Action doRead() {
      return oatpp::data::stream::readSomeDataAsyncInline(this, m_fromStream.get(), m_inlineReadData, yieldTo(&TransferCoroutine::prepareWrite));
    }
    
    Action prepareWrite() {

      auto readCount = m_desiredReadCount - m_inlineReadData.bytesLeft;
      m_inlineWriteData.set(m_buffer->getData(), readCount);

      m_progress += readCount;
      return yieldTo(&TransferCoroutine::doWrite);

    }
    
    Action doWrite() {
      return m_writeCallback->writeAsyncInline(this, m_inlineWriteData, yieldTo(&TransferCoroutine::act));
    }
    
    Action handleError(const std::shared_ptr<const Error>& error) override {
      if(m_transferSize == 0) {
        return finish();
      }
      return propagateError();
    }
    
  };
  
  return TransferCoroutine::start(fromStream, writeCallback, transferSize, buffer);
  
}

namespace {

  oatpp::async::Action asyncOutputStreamActionOnIOError(oatpp::async::AbstractCoroutine* coroutine,
                                                        oatpp::data::stream::OutputStream* stream,
                                                        data::v_io_size res)
  {
    switch (res) {
      case IOError::WAIT_RETRY:
        return stream->suggestOutputStreamAction(res);
      case IOError::RETRY:
        return stream->suggestOutputStreamAction(res);
      case IOError::BROKEN_PIPE:
        return coroutine->error(oatpp::data::AsyncIOError::ERROR_BROKEN_PIPE);
      case IOError::ZERO_VALUE:
        return coroutine->error(oatpp::data::AsyncIOError::ERROR_ZERO_VALUE);
    }
    return coroutine->error<AsyncIOError>("Unknown IO Error result", res);
  }

  oatpp::async::Action asyncInputStreamActionOnIOError(oatpp::async::AbstractCoroutine* coroutine,
                                                       oatpp::data::stream::InputStream* stream,
                                                       data::v_io_size res)
  {
    switch (res) {
      case IOError::WAIT_RETRY:
        return stream->suggestInputStreamAction(res);
      case IOError::RETRY:
        return stream->suggestInputStreamAction(res);
      case IOError::BROKEN_PIPE:
        return coroutine->error(oatpp::data::AsyncIOError::ERROR_BROKEN_PIPE);
      case IOError::ZERO_VALUE:
        return coroutine->error(oatpp::data::AsyncIOError::ERROR_ZERO_VALUE);
    }
    return coroutine->error<AsyncIOError>("Unknown IO Error result", res);
  }

}
  
oatpp::async::Action writeExactSizeDataAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                   oatpp::data::stream::OutputStream* stream,
                                                   AsyncInlineWriteData& inlineData,
                                                   oatpp::async::Action&& nextAction) {
  if(inlineData.bytesLeft > 0) {
    auto res = stream->write(inlineData.currBufferPtr, inlineData.bytesLeft);
    if(res > 0) {
      inlineData.inc(res);
      if (inlineData.bytesLeft > 0) {
        return stream->suggestOutputStreamAction(res);
      }
    } else {
      return asyncOutputStreamActionOnIOError(coroutine, stream, res);
    }
  }
  return std::forward<oatpp::async::Action>(nextAction);

}

oatpp::async::Action readSomeDataAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                             oatpp::data::stream::InputStream* stream,
                                             AsyncInlineReadData& inlineData,
                                             oatpp::async::Action&& nextAction,
                                             bool allowZeroRead) {

  if(inlineData.bytesLeft > 0) {
    auto res = stream->read(inlineData.currBufferPtr, inlineData.bytesLeft);
    if(res > 0) {
      inlineData.inc(res);
    } else {
      if(!(allowZeroRead && res == 0)) {
        return asyncInputStreamActionOnIOError(coroutine, stream, res);
      }
    }
  }
  return std::forward<oatpp::async::Action>(nextAction);

}

oatpp::async::Action readExactSizeDataAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                  oatpp::data::stream::InputStream* stream,
                                                  AsyncInlineReadData& inlineData,
                                                  oatpp::async::Action&& nextAction) {
  if(inlineData.bytesLeft > 0) {
    auto res = stream->read(inlineData.currBufferPtr, inlineData.bytesLeft);
    if(res > 0) {
      inlineData.inc(res);
      if (inlineData.bytesLeft > 0) {
        return stream->suggestInputStreamAction(res);
      }
    } else {
      return asyncInputStreamActionOnIOError(coroutine, stream, res);
    }
  }
  return std::forward<oatpp::async::Action>(nextAction);
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
