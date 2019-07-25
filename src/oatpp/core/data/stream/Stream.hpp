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

#ifndef oatpp_data_Stream
#define oatpp_data_Stream

#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"

#include "oatpp/core/data/IODefinitions.hpp"

namespace oatpp { namespace data{ namespace stream {

/**
 * Stream I/O mode.
 */
enum IOMode : v_int32 {

  /**
   * Blocking stream I/O mode.
   */
  BLOCKING = 0,

  /**
   * Non-blocking stream I/O mode.
   */
  NON_BLOCKING = 1
};

/**
 * Output Stream.
 */
class OutputStream {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~OutputStream() = default;

  /**
   * Write data to stream up to count bytes, and return number of bytes actually written. <br>
   * It is a legal case if return result < count. Caller should handle this!
   * @param data - data to write.
   * @param count - number of bytes to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  virtual data::v_io_size write(const void *data, data::v_io_size count) = 0;

  /**
   * Implementation of OutputStream must suggest async actions for I/O results. <br>
   * Suggested Action is used for scheduling coroutines in async::Executor. <br>
   * **Stream MUST always give the same file-handle if applicable**
   * @param ioResult - result of the call to &l:OutputStream::write ();.
   * @return - &id:oatpp::async::Action;.
   */
  virtual oatpp::async::Action suggestOutputStreamAction(data::v_io_size ioResult) = 0;

  /**
   * Set stream I/O mode.
   * @throws
   */
  virtual void setOutputStreamIOMode(IOMode ioMode) = 0;

  /**
   * Get stream I/O mode.
   * @return
   */
  virtual IOMode getOutputStreamIOMode() = 0;

  /**
   * Same as `write((p_char8)data, std::strlen(data));`.
   * @param data - data to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  data::v_io_size write(const char* data){
    return write((p_char8)data, std::strlen(data));
  }

  /**
   * Same as `write(str->getData(), str->getSize());`
   * @param str - data to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  data::v_io_size write(const oatpp::String& str){
    return write(str->getData(), str->getSize());
  }

  /**
   * Same as `write(&c, 1);`.
   * @param c - one char to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  data::v_io_size writeChar(v_char8 c){
    return write(&c, 1);
  }

};

/**
 * Input Stream.
 */
class InputStream {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~InputStream() = default;

  /**
   * Read data from stream up to count bytes, and return number of bytes actually read. <br>
   * It is a legal case if return result < count. Caller should handle this!
   * @param data - buffer to read data to.
   * @param count - size of the buffer.
   * @return - actual number of bytes read.
   */
  virtual data::v_io_size read(void *data, data::v_io_size count) = 0;

  /**
   * Implementation of InputStream must suggest async actions for I/O results. <br>
   * Suggested Action is used for scheduling coroutines in async::Executor. <br>
   * **Stream MUST always give the same file-handle if applicable**
   * @param ioResult - result of the call to &l:InputStream::read ();.
   * @return - &id:oatpp::async::Action;.
   */
  virtual oatpp::async::Action suggestInputStreamAction(data::v_io_size ioResult) = 0;

  /**
   * Set stream I/O mode.
   * @throws
   */
  virtual void setInputStreamIOMode(IOMode ioMode) = 0;

  /**
   * Get stream I/O mode.
   * @return
   */
  virtual IOMode getInputStreamIOMode() = 0;

};

/**
 * I/O Stream.
 */
class IOStream : public InputStream, public OutputStream {
public:
  typedef data::v_io_size v_size;
};

class CompoundIOStream : public oatpp::base::Countable, public IOStream {
public:
  OBJECT_POOL(CompoundIOStream_Pool, CompoundIOStream, 32);
  SHARED_OBJECT_POOL(Shared_CompoundIOStream_Pool, CompoundIOStream, 32);
private:
  std::shared_ptr<OutputStream> m_outputStream;
  std::shared_ptr<InputStream> m_inputStream;
public:
  CompoundIOStream(const std::shared_ptr<OutputStream>& outputStream,
                   const std::shared_ptr<InputStream>& inputStream)
    : m_outputStream(outputStream)
    , m_inputStream(inputStream)
  {}
public:
  
  static std::shared_ptr<CompoundIOStream> createShared(const std::shared_ptr<OutputStream>& outputStream,
                                                        const std::shared_ptr<InputStream>& inputStream){
    return Shared_CompoundIOStream_Pool::allocateShared(outputStream, inputStream);
  }
  
  data::v_io_size write(const void *data, data::v_io_size count) override {
    return m_outputStream->write(data, count);
  }
  
  data::v_io_size read(void *data, data::v_io_size count) override {
    return m_inputStream->read(data, count);
  }

  oatpp::async::Action suggestOutputStreamAction(data::v_io_size ioResult) override {
    return m_outputStream->suggestOutputStreamAction(ioResult);
  }

  oatpp::async::Action suggestInputStreamAction(data::v_io_size ioResult) override {
    return m_inputStream->suggestInputStreamAction(ioResult);
  }

  void setOutputStreamIOMode(IOMode ioMode) override {
    m_outputStream->setOutputStreamIOMode(ioMode);
  }

  IOMode getOutputStreamIOMode() override {
    return m_outputStream->getOutputStreamIOMode();
  }

  void setInputStreamIOMode(IOMode ioMode) override {
    m_inputStream->setInputStreamIOMode(ioMode);
  }

  IOMode getInputStreamIOMode() override {
    return m_inputStream->getInputStreamIOMode();
  }
    
};

/**
 * Streams that guarantee data to be written in exact amount as specified in call to &l:OutputStream::write (); should extend this class.
 */
class ConsistentOutputStream : public OutputStream {
public:

  /**
   * In case of a `ConsistentOutputStream` suggested Action is always &id:oatpp::async::Action::TYPE_REPEAT; if `ioResult` is greater then zero. <br>
   * @param ioResult - result of the call to &l:OutputStream::write ();.
   * @return - &id:oatpp::async::Action;.
   * @throws - `std::runtime_error` if ioResult <= 0.
   */
  oatpp::async::Action suggestOutputStreamAction(data::v_io_size ioResult) override;

  /**
   * Convert value to string and write to stream.
   * @param value
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;. <br>
   */
  data::v_io_size writeAsString(v_int32 value);

  /**
   * Convert value to string and write to stream.
   * @param value
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;. <br>
   */
  data::v_io_size writeAsString(v_int64 value);

  /**
   * Convert value to string and write to stream.
   * @param value
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;. <br>
   */
  data::v_io_size writeAsString(v_float32 value);

  /**
   * Convert value to string and write to stream.
   * @param value
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;. <br>
   */
  data::v_io_size writeAsString(v_float64 value);

  /**
   * Convert value to string and write to stream.
   * @param value
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;. <br>
   */
  data::v_io_size writeAsString(bool value);

};

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const oatpp::String& str);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int8& value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int16& value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int32& value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Int64& value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float32& value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Float64& value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, const Boolean& value);

ConsistentOutputStream& operator << (ConsistentOutputStream& s, const char* str);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_int32 value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_int64 value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_float32 value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, v_float64 value);
ConsistentOutputStream& operator << (ConsistentOutputStream& s, bool value);

/**
 * Convenience structure for stream Async-Inline write operations.
 */
struct AsyncInlineWriteData {

  /**
   * Pointer to current position in the buffer.
   */
  const void* currBufferPtr;

  /**
   * Bytes left to write from the buffer.
   */
  data::v_io_size bytesLeft;

  /**
   * Default constructor.
   */
  AsyncInlineWriteData();

  /**
   * Constructor.
   * @param data
   * @param size
   */
  AsyncInlineWriteData(const void* data, data::v_io_size size);

  /**
   * Set `currBufferPtr` and `bytesLeft` values. <br>
   * @param data - pointer to buffer containing data to be written.
   * @param size - size in bytes of the buffer.
   */
  void set(const void* data, data::v_io_size size);

  /**
   * Increase position in the write buffer by `amount` bytes. <br>
   * This will increase `currBufferPtr` and descrease `bytesLeft` values.
   * @param amount
   */
  void inc(data::v_io_size amount);

  /**
   * Same as `inc(bytesLeft).`
   */
  void setEof();

};

/**
 * Convenience structure for stream Async-Inline read operations.
 */
struct AsyncInlineReadData {

  /**
   * Pointer to current position in the buffer.
   */
  void* currBufferPtr;

  /**
   * Bytes left to read to the buffer.
   */
  data::v_io_size bytesLeft;

  /**
   * Default constructor.
   */
  AsyncInlineReadData();

  /**
   * Constructor.
   * @param data
   * @param size
   */
  AsyncInlineReadData(void* data, data::v_io_size size);

  /**
   * Set `currBufferPtr` and `bytesLeft` values. <br>
   * @param data - pointer to buffer to store read data.
   * @param size - size in bytes of the buffer.
   */
  void set(void* data, data::v_io_size size);

  /**
   * Increase position in the read buffer by `amount` bytes. <br>
   * This will increase `currBufferPtr` and descrease `bytesLeft` values.
   * @param amount
   */
  void inc(data::v_io_size amount);

  /**
   * Same as `inc(bytesLeft).`
   */
  void setEof();

};

/**
 * Callback for stream write operation.
 */
class WriteCallback {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~WriteCallback() = default;

  /**
   * Write callback.
   * @param data - pointer to data.
   * @param count - size of the data in bytes.
   * @return - &id:oatpp::data::v_io_size;.
   */
  virtual data::v_io_size write(const void *data, data::v_io_size count) = 0;
};

/**
 * Callback for stream asynchronous write operation.
 */
class AsyncWriteCallback {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~AsyncWriteCallback() = default;

  /**
   * Async-Inline write callback.
   * @param coroutine - caller coroutine.
   * @param inlineData - &id:oatpp::data::stream::AsyncInlineWriteData;.
   * @param nextAction - next action when write finished.
   * @return - &id:oatpp::async::Action;.
   */
  virtual oatpp::async::Action writeAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                AsyncInlineWriteData& inlineData,
                                                oatpp::async::Action&& nextAction) = 0;
};

/**
 * Convenience callback to use coroutine starter instead of async inline method.
 */
class AsyncWriteCallbackWithCoroutineStarter : public AsyncWriteCallback {
public:

  /**
   * Async-Inline write callback. <br>
   * Calls &l:AsyncWriteCallbackWithCoroutineStarter::writeAsync (); internally.
   * @param coroutine - caller coroutine.
   * @param inlineData - &id:oatpp::data::stream::AsyncInlineWriteData;.
   * @param nextAction - next action when write finished.
   * @return - &id:oatpp::async::Action;.
   */
  oatpp::async::Action writeAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                        AsyncInlineWriteData& inlineData,
                                        oatpp::async::Action&& nextAction) override;

public:

  /**
   * Implement this method! <br>
   * Write Callback. Data should be fully utilized on call to this method - no partial writes for this method. <br>
   * Return Coroutine starter to start data-processing coroutine or, `nullptr` to do nothing.
   * @param data - pointer to data.
   * @param count - data size.
   * @return - data processing Coroutine-Starter. &id:oatpp::async::CoroutineStarter;.
   */
  virtual async::CoroutineStarter writeAsync(const void *data, data::v_io_size count) = 0;

};

/**
 * Default callback for stream write operation. <br>
 * Uses &l:writeExactSizeData (); method underhood.
 */
class DefaultWriteCallback : public WriteCallback {
private:
  OutputStream* m_stream;
public:

  /**
   * Constructor.
   * @param stream - stream to write to.
   */
  DefaultWriteCallback(OutputStream* stream);

  /**
   * Write callback.
   * @param data - pointer to data.
   * @param count - size of the data in bytes.
   * @return - &id:oatpp::data::v_io_size;.
   */
  data::v_io_size write(const void *data, data::v_io_size count) override;
};

/**
 * Default callback for stream asynchronous write operation.
 * Uses &l:writeExactSizeDataAsyncInline (); method underhood.
 */
class DefaultAsyncWriteCallback : public AsyncWriteCallback {
private:
  std::shared_ptr<OutputStream> m_stream;
public:

  /**
   * Constructor.
   * @param stream - stream to write to.
   */
  DefaultAsyncWriteCallback(const std::shared_ptr<OutputStream>& stream);

  /**
   * Async-Inline write callback.
   * @param coroutine - caller coroutine.
   * @param inlineData - &id:oatpp::data::stream::AsyncInlineWriteData;.
   * @param nextAction - next action when write finished.
   * @return - &id:oatpp::async::Action;.
   */
  oatpp::async::Action writeAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                        AsyncInlineWriteData& inlineData,
                                        oatpp::async::Action&& nextAction) override;
};

/**
 * Stream read callback.
 */
class ReadCallback {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~ReadCallback() = default;

  /**
   * Read callback. Override this method! <br>
   * Write up to `count` bytes to `buffer` and return the actual number of bytes written. <br>
   * **This method must return `0` in order to indicate end-of-file.**
   * @param buffer - pointer to buffer.
   * @param count - size of the buffer.
   * @return - actual number of bytes written to buffer. 0 - to indicate end-of-file.
   */
  virtual data::v_io_size read(void *buffer, data::v_io_size count) = 0;

};

/**
 * Callback for stream asynchronous read operation.
 */
class AsyncReadCallback {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~AsyncReadCallback() = default;

  /**
   * Async-Inline read callback.
   * @param coroutine - caller coroutine.
   * @param inlineData - &id:oatpp::data::stream::AsyncInlineReadData;.
   * @param nextAction - next action when read finished.
   * @return - &id:oatpp::async::Action;.
   */
  virtual oatpp::async::Action readAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                               AsyncInlineReadData& inlineData,
                                               oatpp::async::Action&& nextAction) = 0;

};

/**
 * Error of Asynchronous stream transfer.
 */
class AsyncTransferError : public oatpp::async::Error {
public:
  /**
   * Constructor.
   * @param what
   */
  AsyncTransferError(const char* what) : oatpp::async::Error(what) {}
};

/**
 * Read bytes from `fromStream` and write to `writeCallback` using `buffer` of size `bufferSize`
 * transfer up to transferSize or until error if transferSize == 0
 * @param fromStream
 * @param transferSize
 * @param buffer
 * @param bufferSize
 * @return - amount of bytes actually transferred.
 */
oatpp::data::v_io_size transfer(InputStream* fromStream,
                                WriteCallback* writeCallback,
                                oatpp::data::v_io_size transferSize,
                                void* buffer,
                                oatpp::data::v_io_size bufferSize);
  
  
/**
 * Same as transfer but asynchronous
 */
oatpp::async::CoroutineStarter transferAsync(const std::shared_ptr<InputStream>& fromStream,
                                             const std::shared_ptr<AsyncWriteCallback>& writeCallback,
                                             oatpp::data::v_io_size transferSize,
                                             const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer);

  
oatpp::async::Action writeExactSizeDataAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                   oatpp::data::stream::OutputStream* stream,
                                                   AsyncInlineWriteData& inlineData,
                                                   oatpp::async::Action&& nextAction);

oatpp::async::Action readSomeDataAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                             oatpp::data::stream::InputStream* stream,
                                             AsyncInlineReadData& inlineData,
                                             oatpp::async::Action&& nextAction,
                                             bool allowZeroRead = false);

oatpp::async::Action readExactSizeDataAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                                  oatpp::data::stream::InputStream* stream,
                                                  AsyncInlineReadData& inlineData,
                                                  oatpp::async::Action&& nextAction);

/**
 * Read exact amount of bytes to stream
 * returns exact amount of bytes was read.
 * return result can be < size only in case of some disaster like connection reset by peer
 */
oatpp::data::v_io_size readExactSizeData(oatpp::data::stream::InputStream* stream, void* data, data::v_io_size size);
  
/**
 * Write exact amount of bytes to stream.
 * returns exact amount of bytes was written.
 * return result can be < size only in case of some disaster like broken pipe
 */
oatpp::data::v_io_size writeExactSizeData(oatpp::data::stream::OutputStream* stream, const void* data, data::v_io_size size);
  
}}}

#endif /* defined(_data_Stream) */
