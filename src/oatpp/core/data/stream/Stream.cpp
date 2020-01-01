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
// Context

Context::Context(Properties&& properties)
  : m_properties(std::forward<Properties>(properties))
{}

const Context::Properties& Context::getProperties() const {
  return m_properties;
}

Context::Properties& Context::getMutableProperties() {
  return m_properties;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DefaultInitializedContext

DefaultInitializedContext::DefaultInitializedContext(StreamType streamType)
  : m_streamType(streamType)
{}

DefaultInitializedContext::DefaultInitializedContext(StreamType streamType, Properties&& properties)
  : Context(std::forward<Properties>(properties))
  , m_streamType(streamType)
{}

void DefaultInitializedContext::init() {
  // DO NOTHING
}

async::CoroutineStarter DefaultInitializedContext::initAsync() {
  return nullptr;
}

bool DefaultInitializedContext::isInitialized() const {
  return true;
}

StreamType DefaultInitializedContext::getStreamType() const {
  return m_streamType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IOStream

void IOStream::initContexts() {

  auto& inStreamContext = getInputStreamContext();
  if (!inStreamContext.isInitialized()) {
    inStreamContext.init();
  }

  auto& outStreamContext = getOutputStreamContext();
  if(outStreamContext != inStreamContext && !outStreamContext.isInitialized()) {
    outStreamContext.init();
  }

}

/**
 * Init input/output stream contexts in an async manner.
 */
async::CoroutineStarter IOStream::initContextsAsync() {

  async::CoroutineStarter starter(nullptr);

  auto& inStreamContext = getInputStreamContext();
  if (!inStreamContext.isInitialized()) {
    starter.next(inStreamContext.initAsync());
  }

  auto& outStreamContext = getOutputStreamContext();
  if(outStreamContext != inStreamContext && !outStreamContext.isInitialized()) {
    starter.next(outStreamContext.initAsync());
  }

  return starter;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConsistentOutputStream

data::v_io_size ConsistentOutputStream::writeAsString(v_int32 value){
  v_char8 a[16];
  v_int32 size = utils::conversion::int32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

data::v_io_size ConsistentOutputStream::writeAsString(v_int64 value){
  v_char8 a[32];
  v_int32 size = utils::conversion::int64ToCharSequence(value, &a[0], 32);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

data::v_io_size ConsistentOutputStream::writeAsString(v_float32 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float32ToCharSequence(value, &a[0], 100);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

data::v_io_size ConsistentOutputStream::writeAsString(v_float64 value){
  v_char8 a[100];
  v_int32 size = utils::conversion::float64ToCharSequence(value, &a[0], 100);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}
  
data::v_io_size ConsistentOutputStream::writeAsString(bool value) {
  if(value){
    return writeSimple("true", 4);
  } else {
    return writeSimple("false", 5);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncInlineWriteData

AsyncInlineWriteData::AsyncInlineWriteData()
  : currBufferPtr(nullptr)
  , bytesLeft(0)
{}

AsyncInlineWriteData::AsyncInlineWriteData(const void* data, v_buff_size size)
  : currBufferPtr(data)
  , bytesLeft(size)
{}

void AsyncInlineWriteData::set(const void* data, v_buff_size size) {
  currBufferPtr = data;
  bytesLeft = size;
}

void AsyncInlineWriteData::inc(v_buff_size amount) {
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

AsyncInlineReadData::AsyncInlineReadData(void* data, v_buff_size size)
  : currBufferPtr(data)
  , bytesLeft(size)
{}

void AsyncInlineReadData::set(void* data, v_buff_size size) {
  currBufferPtr = data;
  bytesLeft = size;
}

void AsyncInlineReadData::inc(v_buff_size amount) {
  currBufferPtr = &((p_char8) currBufferPtr)[amount];
  bytesLeft -= amount;
}

void AsyncInlineReadData::setEof() {
  currBufferPtr = &((p_char8) currBufferPtr)[bytesLeft];
  bytesLeft = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions


ConsistentOutputStream& operator << (ConsistentOutputStream& s, const oatpp::String& str) {
  if(str) {
    s.writeSimple(str);
  } else {
    s.writeSimple("[<String(null)>]");
  }
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int8& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Int8(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int16& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Int16(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int32& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Int32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int64& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Int64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float32& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Float32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float64& value) {
  if(value.getPtr()) {
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Float64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Boolean& value) {
  if(value.getPtr()) { // use getPtr() here to avoid false to nullptr conversion
    return operator << (s, value->getValue());
  }
  s.writeSimple("[<Boolean(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const char* str) {
  if(str != nullptr) {
    s.writeSimple(str);
  } else {
    s.writeSimple("[<char*(null)>]");
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
    s.OutputStream::writeSimple("true");
  } else {
    s.OutputStream::writeSimple("false");
  }
  return s;
}
  
oatpp::data::v_io_size transfer(ReadCallback* readCallback,
                                WriteCallback* writeCallback,
                                v_buff_size transferSize,
                                void* buffer,
                                v_buff_size bufferSize)
{

  v_buff_size progress = 0;
  
  while (transferSize == 0 || progress < transferSize) {
    v_buff_size desiredReadCount = transferSize - progress;
    if(transferSize == 0 || desiredReadCount > bufferSize){
      desiredReadCount = bufferSize;
    }
    auto readResult = readCallback->readSimple(buffer, desiredReadCount);
    if(readResult > 0) {

      p_char8 data = (p_char8) buffer;
      v_buff_size bytesLeft = readResult;
      while(bytesLeft > 0) {
        auto writeResult = writeCallback->writeSimple(data, bytesLeft);
        if(writeResult > 0) {
          data = &data[writeResult];
          bytesLeft -= writeResult;
        } else if (// In general case `OutputStream` may return `RETRY_READ`- because of the underlying transport.
                   // Check all retry values here!.
                   writeResult == data::IOError::RETRY_READ || writeResult == data::IOError::RETRY_WRITE) {
          continue;
        } else {
          throw std::runtime_error("[oatpp::data::stream::transfer()]: Unknown Error. Can't continue transfer.");
        }
      }

      progress += readResult;

    } else {
      if(// In general case `OutputStream` may return `RETRY_READ`- because of the underlying transport.
         // Check all retry values here!.
         readResult == data::IOError::RETRY_READ || readResult == data::IOError::RETRY_WRITE) {
        continue;
      }
      return progress;
    }
  }
  
  return progress;
  
}

oatpp::async::CoroutineStarter transferAsync(const std::shared_ptr<ReadCallback>& readCallback,
                                             const std::shared_ptr<WriteCallback>& writeCallback,
                                             v_buff_size transferSize,
                                             const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
{
  
  class TransferCoroutine : public oatpp::async::Coroutine<TransferCoroutine> {
  private:
    std::shared_ptr<ReadCallback> m_readCallback;
    std::shared_ptr<WriteCallback> m_writeCallback;
    v_buff_size m_transferSize;
    v_buff_size m_progress;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> m_buffer;

    v_buff_size m_desiredReadCount;

    AsyncInlineReadData m_inlineReadData;
    AsyncInlineWriteData m_inlineWriteData;
    
  public:
    
    TransferCoroutine(const std::shared_ptr<ReadCallback>& readCallback,
                      const std::shared_ptr<WriteCallback>& writeCallback,
                      v_buff_size transferSize,
                      const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer)
      : m_readCallback(readCallback)
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
      return oatpp::data::stream::readSomeDataAsyncInline(m_readCallback.get(), m_inlineReadData, yieldTo(&TransferCoroutine::prepareWrite));
    }
    
    Action prepareWrite() {

      auto readCount = m_desiredReadCount - m_inlineReadData.bytesLeft;
      m_inlineWriteData.set(m_buffer->getData(), readCount);

      m_progress += readCount;
      return yieldTo(&TransferCoroutine::doWrite);

    }
    
    Action doWrite() {

      if(m_inlineWriteData.bytesLeft > 0) {

        Action action;
        auto res = m_writeCallback->write(m_inlineWriteData.currBufferPtr, m_inlineWriteData.bytesLeft, action);

        if(res > 0) {
          m_inlineWriteData.inc(res);
        } else if(res == data::IOError::ZERO_VALUE || res == data::IOError::BROKEN_PIPE) {
          return finish();
        }

        if(!action.isNone()) {
          return action;
        }

      }

      return yieldTo(&TransferCoroutine::act);

    }
    
    Action handleError(Error* error) override {
      if(m_transferSize == 0) {
        return finish();
      }
      return error;
    }
    
  };
  
  return TransferCoroutine::start(readCallback, writeCallback, transferSize, buffer);
  
}
  
}}}
