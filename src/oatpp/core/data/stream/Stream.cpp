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
// WriteCallback

v_io_size WriteCallback::write(data::buffer::InlineWriteData& inlineData, async::Action& action) {
  auto res = write(inlineData.currBufferPtr, inlineData.bytesLeft, action);
  if(res > 0) {
    inlineData.inc(res);
  }
  return res;
}

v_io_size WriteCallback::writeSimple(const void *data, v_buff_size count) {
  async::Action action;
  auto res = write(data, count, action);
  if(!action.isNone()) {
    OATPP_LOGE("[oatpp::data::stream::WriteCallback::writeSimple()]", "Error. writeSimple is called on a stream in Async mode.");
    throw std::runtime_error("[oatpp::data::stream::WriteCallback::writeSimple()]: Error. writeSimple is called on a stream in Async mode.");
  }
  return res;
}

v_io_size WriteCallback::writeExactSizeDataSimple(data::buffer::InlineWriteData& inlineData) {
  auto initialCount = inlineData.bytesLeft;
  while(inlineData.bytesLeft > 0) {
    async::Action action;
    auto res = write(inlineData, action);
    if(!action.isNone()) {
      OATPP_LOGE("[oatpp::data::stream::WriteCallback::writeExactSizeDataSimple()]", "Error. writeExactSizeDataSimple() is called on a stream in Async mode.");
      throw std::runtime_error("[oatpp::data::stream::WriteCallback::writeExactSizeDataSimple()]: Error. writeExactSizeDataSimple() is called on a stream in Async mode.");
    }
    if(res == IOError::BROKEN_PIPE || res == IOError::ZERO_VALUE) {
      break;
    }
  }
  return initialCount - inlineData.bytesLeft;
}

v_io_size WriteCallback::writeExactSizeDataSimple(const void *data, v_buff_size count) {
  data::buffer::InlineWriteData inlineData(data, count);
  return writeExactSizeDataSimple(inlineData);
}

async::Action WriteCallback::writeExactSizeDataAsyncInline(data::buffer::InlineWriteData& inlineData, async::Action&& nextAction) {

  if(inlineData.bytesLeft > 0) {

    async::Action action;
    auto res = write(inlineData, action);

    if (!action.isNone()) {
      return action;
    }

    if (res > 0) {
      return async::Action::createActionByType(async::Action::TYPE_REPEAT);
    } else {
      switch (res) {
        case IOError::BROKEN_PIPE:
          return new AsyncIOError(IOError::BROKEN_PIPE);
        case IOError::ZERO_VALUE:
          break;
        case IOError::RETRY_READ:
          return async::Action::createActionByType(async::Action::TYPE_REPEAT);
        case IOError::RETRY_WRITE:
          return async::Action::createActionByType(async::Action::TYPE_REPEAT);
        default:
          OATPP_LOGE("[oatpp::data::stream::writeExactSizeDataAsyncInline()]", "Error. Unknown IO result.");
          return new async::Error(
            "[oatpp::data::stream::writeExactSizeDataAsyncInline()]: Error. Unknown IO result.");
      }
    }

  }

  return std::forward<async::Action>(nextAction);

}

async::CoroutineStarter WriteCallback::writeExactSizeDataAsync(const void* data, v_buff_size size) {

  class WriteDataCoroutine : public oatpp::async::Coroutine<WriteDataCoroutine> {
  private:
    WriteCallback* m_this;
    data::buffer::InlineWriteData m_inlineData;
  public:

    WriteDataCoroutine(WriteCallback* _this,
                       const void* data, v_buff_size size)
      : m_this(_this)
      , m_inlineData(data, size)
    {}

    Action act() override {
      return m_this->writeExactSizeDataAsyncInline(m_inlineData, finish());
    }

  };

  return WriteDataCoroutine::start(this, data, size);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ReadCallback

v_io_size ReadCallback::read(data::buffer::InlineReadData& inlineData, async::Action& action) {
  auto res = read(inlineData.currBufferPtr, inlineData.bytesLeft, action);
  if(res > 0) {
    inlineData.inc(res);
  }
  return res;
}

v_io_size ReadCallback::readExactSizeDataSimple(data::buffer::InlineReadData& inlineData) {
  auto initialCount = inlineData.bytesLeft;
  while(inlineData.bytesLeft > 0) {
    async::Action action;
    auto res = read(inlineData, action);
    if(!action.isNone()) {
      OATPP_LOGE("[oatpp::data::stream::ReadCallback::readExactSizeDataSimple()]", "Error. readExactSizeDataSimple() is called on a stream in Async mode.");
      throw std::runtime_error("[oatpp::data::stream::ReadCallback::readExactSizeDataSimple()]: Error. readExactSizeDataSimple() is called on a stream in Async mode.");
    }
    if(res <= 0 && res != IOError::RETRY_READ && res != IOError::RETRY_WRITE) {
      break;
    }
  }
  return initialCount - inlineData.bytesLeft;
}

v_io_size ReadCallback::readExactSizeDataSimple(void *data, v_buff_size count) {
  data::buffer::InlineReadData inlineData(data, count);
  return readExactSizeDataSimple(inlineData);
}

async::Action ReadCallback::readExactSizeDataAsyncInline(data::buffer::InlineReadData& inlineData, async::Action&& nextAction) {

  if(inlineData.bytesLeft > 0) {

    async::Action action;
    auto res = read(inlineData, action);

    if (!action.isNone()) {
      return action;
    }

    if (res > 0) {
      return async::Action::createActionByType(async::Action::TYPE_REPEAT);
    } else {
      switch (res) {
        case IOError::BROKEN_PIPE:
          return new AsyncIOError("[oatpp::data::stream::readExactSizeDataAsyncInline()]: IOError::BROKEN_PIPE", IOError::BROKEN_PIPE);
        case IOError::ZERO_VALUE:
          break;
        case IOError::RETRY_READ:
          return async::Action::createActionByType(async::Action::TYPE_REPEAT);
        case IOError::RETRY_WRITE:
          return async::Action::createActionByType(async::Action::TYPE_REPEAT);
        default:
          OATPP_LOGE("[oatpp::data::stream::readExactSizeDataAsyncInline()]", "Error. Unknown IO result.");
          return new async::Error(
            "[oatpp::data::stream::readExactSizeDataAsyncInline()]: Error. Unknown IO result.");
      }
    }

  }

  return std::forward<async::Action>(nextAction);

}

async::Action ReadCallback::readSomeDataAsyncInline(data::buffer::InlineReadData& inlineData, async::Action&& nextAction) {

  if(inlineData.bytesLeft > 0) {

    async::Action action;
    auto res = read(inlineData, action);

    if(!action.isNone()) {
      return action;
    }

    if(res < 0) {
      switch (res) {
        case IOError::BROKEN_PIPE:
          return new AsyncIOError(IOError::BROKEN_PIPE);
//          case IOError::ZERO_VALUE:
//            break;
        case IOError::RETRY_READ:
          return async::Action::createActionByType(async::Action::TYPE_REPEAT);
        case IOError::RETRY_WRITE:
          return async::Action::createActionByType(async::Action::TYPE_REPEAT);
        default:
          OATPP_LOGE("[oatpp::data::stream::readSomeDataAsyncInline()]", "Error. Unknown IO result.");
          return new async::Error(
            "[oatpp::data::stream::readSomeDataAsyncInline()]: Error. Unknown IO result.");
      }
    }

  }

  return std::forward<async::Action>(nextAction);

}

v_io_size ReadCallback::readSimple(void *data, v_buff_size count) {
  async::Action action;
  auto res = read(data, count, action);
  if(!action.isNone()) {
    OATPP_LOGE("[oatpp::data::stream::ReadCallback::readSimple()]", "Error. readSimple is called on a stream in Async mode.");
    throw std::runtime_error("[oatpp::data::stream::ReadCallback::readSimple()]: Error. readSimple is called on a stream in Async mode.");
  }
  return res;
}

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

v_io_size ConsistentOutputStream::writeAsString(v_int8 value){
  v_char8 a[16];
  auto size = utils::conversion::int32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_uint8 value){
  v_char8 a[16];
  auto size = utils::conversion::uint32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_int16 value){
  v_char8 a[16];
  auto size = utils::conversion::int32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_uint16 value){
  v_char8 a[16];
  auto size = utils::conversion::uint32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_int32 value){
  v_char8 a[16];
  auto size = utils::conversion::int32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_uint32 value){
  v_char8 a[16];
  auto size = utils::conversion::uint32ToCharSequence(value, &a[0], 16);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_int64 value){
  v_char8 a[32];
  auto size = utils::conversion::int64ToCharSequence(value, &a[0], 32);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_uint64 value){
  v_char8 a[32];
  auto size = utils::conversion::uint64ToCharSequence(value, &a[0], 32);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_float32 value){
  v_char8 a[100];
  auto size = utils::conversion::float32ToCharSequence(value, &a[0], 100);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}

v_io_size ConsistentOutputStream::writeAsString(v_float64 value){
  v_char8 a[100];
  auto size = utils::conversion::float64ToCharSequence(value, &a[0], 100);
  if(size > 0){
    return writeSimple(&a[0], size);
  }
  return 0;
}
  
v_io_size ConsistentOutputStream::writeAsString(bool value) {
  if(value){
    return writeSimple("true", 4);
  } else {
    return writeSimple("false", 5);
  }
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
    return operator << (s, *value);
  }
  s.writeSimple("[<Int8(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const UInt8& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<UInt8(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int16& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<Int16(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const UInt16& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<UInt16(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int32& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<Int32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const UInt32& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<UInt32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int64& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<Int64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const UInt64& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<UInt64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float32& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<Float32(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float64& value) {
  if(value.getPtr()) {
    return operator << (s, *value);
  }
  s.writeSimple("[<Float64(null)>]");
  return s;
}

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Boolean& value) {
  if(value.getPtr()) { // use getPtr() here to avoid false to nullptr conversion
    return operator << (s, *value);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataTransferProcessor

StatelessDataTransferProcessor StatelessDataTransferProcessor::INSTANCE;

v_io_size StatelessDataTransferProcessor::suggestInputStreamReadSize() {
  return 32767;
}

v_int32 StatelessDataTransferProcessor::iterate(data::buffer::InlineReadData& dataIn, data::buffer::InlineReadData& dataOut) {

  if(dataOut.bytesLeft > 0) {
    return Error::FLUSH_DATA_OUT;
  }

  if(dataIn.currBufferPtr != nullptr) {

    if(dataIn.bytesLeft == 0){
      return Error::PROVIDE_DATA_IN;
    }

    dataOut = dataIn;
    dataIn.setEof();
    return Error::FLUSH_DATA_OUT;

  }

  dataOut = dataIn;
  dataIn.setEof();
  return Error::FINISHED;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions

v_io_size transfer(const base::ObjectHandle<ReadCallback>& readCallback,
                         const base::ObjectHandle<WriteCallback>& writeCallback,
                         v_io_size transferSize,
                         void* buffer,
                         v_buff_size bufferSize,
                         const base::ObjectHandle<data::buffer::Processor>& processor)
{

  data::buffer::InlineReadData inData;
  data::buffer::InlineReadData outData;

  v_int32 procRes = data::buffer::Processor::Error::PROVIDE_DATA_IN;
  v_io_size progress = 0;

  while(procRes != data::buffer::Processor::Error::FINISHED) {

    if(procRes == data::buffer::Processor::Error::PROVIDE_DATA_IN && inData.bytesLeft == 0) {

      v_buff_size desiredToRead = processor->suggestInputStreamReadSize();

      if (desiredToRead > bufferSize) {
        desiredToRead = bufferSize;
      }

      if(transferSize > 0 && progress + desiredToRead > transferSize) {
        desiredToRead = transferSize - progress;
      }

      v_io_size res = 0;

      if(desiredToRead > 0) {
        res = IOError::RETRY_READ;
        while (res == IOError::RETRY_READ || res == IOError::RETRY_WRITE) {
          res = readCallback->readSimple(buffer, desiredToRead);
        }
      }

      if (res > 0) {
        inData.set(buffer, res);
        progress += res;
      } else {
        inData.set(nullptr, 0);
      }

    }

    procRes = data::buffer::Processor::Error::OK;
    while(procRes == data::buffer::Processor::Error::OK) {
      procRes = processor->iterate(inData, outData);
    }

    switch(procRes) {

      case data::buffer::Processor::Error::PROVIDE_DATA_IN: {
        continue;
      }

      case data::buffer::Processor::Error::FLUSH_DATA_OUT: {
        v_io_size res = IOError::RETRY_WRITE;
        while(res == IOError::RETRY_WRITE || res == IOError::RETRY_READ) {
          res = writeCallback->writeSimple(outData.currBufferPtr, outData.bytesLeft);
        }
        if(res > 0) {
          outData.inc(res);
        } else {
          return progress;
        }
        break;
      }

      case data::buffer::Processor::Error::FINISHED:
        return progress;

      default:
        //throw std::runtime_error("Unknown buffer processor error.");
        return progress;

    }

  }

  return progress;

}

async::CoroutineStarter transferAsync(const base::ObjectHandle<ReadCallback>& readCallback,
                                      const base::ObjectHandle<WriteCallback>& writeCallback,
                                      v_buff_size transferSize,
                                      const base::ObjectHandle<data::buffer::IOBuffer>& buffer,
                                      const base::ObjectHandle<data::buffer::Processor>& processor)
{

  class TransferCoroutine : public oatpp::async::Coroutine<TransferCoroutine> {
  private:
    base::ObjectHandle<ReadCallback> m_readCallback;
    base::ObjectHandle<WriteCallback> m_writeCallback;
    v_buff_size m_transferSize;
    base::ObjectHandle<oatpp::data::buffer::IOBuffer> m_buffer;
    base::ObjectHandle<data::buffer::Processor> m_processor;
  private:
    v_buff_size m_progress;
  private:
    v_int32 m_procRes;
    data::buffer::InlineReadData m_readData;
    data::buffer::InlineWriteData m_writeData;
    data::buffer::InlineReadData m_inData;
    data::buffer::InlineReadData m_outData;
  public:

    TransferCoroutine(const base::ObjectHandle<ReadCallback>& readCallback,
                      const base::ObjectHandle<WriteCallback>& writeCallback,
                      v_buff_size transferSize,
                      const base::ObjectHandle<buffer::IOBuffer>& buffer,
                      const base::ObjectHandle<buffer::Processor>& processor)
      : m_readCallback(readCallback)
      , m_writeCallback(writeCallback)
      , m_transferSize(transferSize)
      , m_buffer(buffer)
      , m_processor(processor)
      , m_progress(0)
      , m_procRes(data::buffer::Processor::Error::PROVIDE_DATA_IN)
      , m_readData(buffer->getData(), buffer->getSize())
    {}

    Action act() override {

      if(m_procRes == data::buffer::Processor::Error::FINISHED) {
        return finish();
      }

      if(m_procRes == data::buffer::Processor::Error::PROVIDE_DATA_IN && m_inData.bytesLeft == 0) {

        auto desiredToRead = m_processor->suggestInputStreamReadSize();

        if (desiredToRead > m_readData.bytesLeft) {
          desiredToRead = m_readData.bytesLeft;
        }

        if(m_transferSize > 0 && m_progress + desiredToRead > m_transferSize) {
          desiredToRead = m_transferSize - m_progress;
        }

        Action action;
        v_io_size res = 0;

        if(desiredToRead > 0) {
          res = m_readCallback->read(m_readData.currBufferPtr, desiredToRead, action);
        }

        if (res > 0) {
          m_readData.inc(res);
          m_inData.set(m_buffer->getData(), m_buffer->getSize() - m_readData.bytesLeft);
          m_progress += res;
        } else {

          switch(res) {

            case IOError::BROKEN_PIPE:
              if(m_transferSize > 0) {
                return error<AsyncTransferError>("[oatpp::data::stream::transferAsync]: Error. ReadCallback. BROKEN_PIPE.");
              }
              m_inData.set(nullptr, 0);
              break;

            case IOError::ZERO_VALUE:
              m_inData.set(nullptr, 0);
              break;

            case IOError::RETRY_READ:
              if(!action.isNone()) {
                return action;
              }
              return repeat();

            case IOError::RETRY_WRITE:
              if(!action.isNone()) {
                return action;
              }
              return repeat();

            default:
              if(m_transferSize > 0) {
                if (!action.isNone()) {
                  return action;
                }
                return error<AsyncTransferError>("[oatpp::data::stream::transferAsync]: Error. ReadCallback. Unknown IO error.");
              }
              m_inData.set(nullptr, 0);

          }

        }

        if(!action.isNone()){
          return action;
        }

      }

      return yieldTo(&TransferCoroutine::process);

    }

    Action process() {

      m_procRes = m_processor->iterate(m_inData, m_outData);

      switch(m_procRes) {

        case data::buffer::Processor::Error::OK:
          return repeat();

        case data::buffer::Processor::Error::PROVIDE_DATA_IN: {
          m_readData.set(m_buffer->getData(), m_buffer->getSize());
          return yieldTo(&TransferCoroutine::act);
        }

        case data::buffer::Processor::Error::FLUSH_DATA_OUT: {
          m_readData.set(m_buffer->getData(), m_buffer->getSize());
          m_writeData.set(m_outData.currBufferPtr, m_outData.bytesLeft);
          m_outData.setEof();
          return yieldTo(&TransferCoroutine::flushData);
        }

        case data::buffer::Processor::Error::FINISHED:
          return finish();

        default:
          return error<AsyncTransferError>("[oatpp::data::stream::transferAsync]: Error. ReadCallback. Unknown processing error.");

      }

    }

    Action flushData() {
      return m_writeCallback->writeExactSizeDataAsyncInline(m_writeData, yieldTo(&TransferCoroutine::act));
    }

  };

  return TransferCoroutine::start(readCallback, writeCallback, transferSize, buffer, processor);

}
  
}}}
