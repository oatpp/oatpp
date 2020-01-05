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

#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"
#include "oatpp/core/data/buffer/Processor.hpp"

#include "oatpp/core/data/IODefinitions.hpp"

namespace oatpp { namespace data{ namespace stream {

/**
 * Stream Type.
 */
enum StreamType : v_int32 {

  /**
   * Finite stream.
   */
  STREAM_FINITE = 0,

  /**
   * Infinite stream.
   */
  STREAM_INFINITE = 1

};


/**
 * Stream Context.
 */
class Context {
public:
  /**
   * Convenience typedef for &id:oatpp::data::share::LazyStringMap;.
   */
  typedef oatpp::data::share::LazyStringMap<oatpp::data::share::StringKeyLabel> Properties;
private:
  Properties m_properties;
protected:
  /**
   * `protected`. Get mutable additional optional context specific properties.
   * @return - &l:Context::Properties;.
   */
  Properties& getMutableProperties();
public:

  /**
   * Default constructor.
   */
  Context() = default;

  /**
   * Constructor.
   * @param properties - &l:Context::Properties;.
   */
  Context(Properties&& properties);

  /**
   * Virtual destructor.
   */
  virtual ~Context() = default;

  /**
   * Initialize stream context.
   */
  virtual void init() = 0;

  /**
   * Initialize stream context in an async manner.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  virtual async::CoroutineStarter initAsync() = 0;

  /**
   * Check if the stream context is initialized.
   * @return - `bool`.
   */
  virtual bool isInitialized() const = 0;

  /**
   * Get stream type.
   * @return - &l:StreamType;.
   */
  virtual StreamType getStreamType() const = 0;

  /**
   * Additional optional context specific properties.
   * @return - &l:Context::Properties;.
   */
  const Properties& getProperties() const;

  inline bool operator == (const Context& other){
    return this == &other;
  }

  inline bool operator != (const Context& other){
    return this != &other;
  }

};

/**
 * The default implementation for context with no initialization.
 */
class DefaultInitializedContext : public oatpp::data::stream::Context {
private:
  StreamType m_streamType;
public:

  /**
   * Constructor.
   * @param streamType - &l:StreamType;.
   */
  DefaultInitializedContext(StreamType streamType);

  /**
   * Constructor.
   * @param streamType - &l:StreamType;.
   * @param properties - &l:Context::Properties;.
   */
  DefaultInitializedContext(StreamType streamType, Properties&& properties);

  /**
   * Initialize stream context. <br>
   * *This particular implementation does nothing.*
   */
  void init() override;

  /**
   * Initialize stream context in an async manner.
   * *This particular implementation does nothing.*
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  async::CoroutineStarter initAsync() override;

  /**
   * Check if the stream context is initialized.
   * *This particular implementation always returns `true`.*
   * @return - `bool`.
   */
  bool isInitialized() const override;

  /**
   * Get stream type.
   * @return - &l:StreamType;.
   */
  StreamType getStreamType() const override;

};

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
  ASYNCHRONOUS = 1
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
   * Write operation callback.
   * @param data - pointer to data.
   * @param count - size of the data in bytes.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written. 0 - to indicate end-of-file.
   */
  virtual data::v_io_size write(const void *data, v_buff_size count, async::Action& action) = 0;

  data::v_io_size write(data::buffer::InlineWriteData& inlineData, async::Action& action);

  data::v_io_size writeSimple(const void *data, v_buff_size count);

  data::v_io_size writeExactSizeDataSimple(data::buffer::InlineWriteData& inlineData);

  data::v_io_size writeExactSizeDataSimple(const void *data, v_buff_size count);

  async::Action writeExactSizeDataAsyncInline(data::buffer::InlineWriteData& inlineData, async::Action&& nextAction);

  async::CoroutineStarter writeExactSizeDataAsync(const void* data, v_buff_size size);

  /**
   * Same as `write((p_char8)data, std::strlen(data));`.
   * @param data - data to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  data::v_io_size writeSimple(const char* data){
    return writeSimple((p_char8)data, std::strlen(data));
  }

  /**
   * Same as `write(str->getData(), str->getSize());`
   * @param str - data to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  data::v_io_size writeSimple(const oatpp::String& str){
    return writeSimple(str->getData(), str->getSize());
  }

  /**
   * Same as `write(&c, 1);`.
   * @param c - one char to write.
   * @return - actual number of bytes written. &id:oatpp::data::v_io_size;.
   */
  data::v_io_size writeCharSimple(v_char8 c){
    return writeSimple(&c, 1);
  }

};

/**
 * Output Stream.
 */
class OutputStream : public WriteCallback {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~OutputStream() = default;

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
   * Get stream context.
   * @return - &l:Context;.
   */
  virtual Context& getOutputStreamContext() = 0;

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
   * Read operation callback.
   * @param buffer - pointer to buffer.
   * @param count - size of the buffer in bytes.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written to buffer. 0 - to indicate end-of-file.
   */
  virtual data::v_io_size read(void *buffer, v_buff_size count, async::Action& action) = 0;

  data::v_io_size read(data::buffer::InlineReadData& inlineData, async::Action& action);

  data::v_io_size readExactSizeDataSimple(data::buffer::InlineReadData& inlineData);

  data::v_io_size readExactSizeDataSimple(void *data, v_buff_size count);

  async::Action readExactSizeDataAsyncInline(data::buffer::InlineReadData& inlineData, async::Action&& nextAction);

  async::Action readSomeDataAsyncInline(data::buffer::InlineReadData& inlineData, async::Action&& nextAction);

  data::v_io_size readSimple(void *data, v_buff_size count);

};

/**
 * Input Stream.
 */
class InputStream : public ReadCallback {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~InputStream() = default;

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

  /**
   * Get stream context.
   * @return - &l:Context;.
   */
  virtual Context& getInputStreamContext() = 0;

};

/**
 * I/O Stream.
 */
class IOStream : public InputStream, public OutputStream {
public:

  /**
   * Init input/output stream contexts.
   */
  void initContexts();

  /**
   * Init input/output stream contexts in an async manner.
   */
  async::CoroutineStarter initContextsAsync();

};

/**
 * Streams that guarantee data to be written in exact amount as specified in call to &l:OutputStream::write (); should extend this class.
 */
class ConsistentOutputStream : public OutputStream {
public:

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
 * Plain data transfer processor.
 * Transfers data as is.
 */
class StatelessDataTransferProcessor : public data::buffer::Processor {
public:
  static StatelessDataTransferProcessor INSTANCE;
public:
  data::v_io_size suggestInputStreamReadSize() override;
  v_int32 iterate(data::buffer::InlineReadData& dataIn, data::buffer::InlineReadData& dataOut) override;
};

data::v_io_size transfer2(const base::ObjectHandle<ReadCallback>& readCallback,
                          const base::ObjectHandle<WriteCallback>& writeCallback,
                          data::v_io_size transferSize,
                          void* buffer,
                          v_buff_size bufferSize,
                          const base::ObjectHandle<data::buffer::Processor>& processor = &StatelessDataTransferProcessor::INSTANCE);

/**
 * Read bytes from `fromStream` and write to `writeCallback` using `buffer` of size `bufferSize`
 * transfer up to transferSize or until error if transferSize == 0
 * @param readCallback
 * @param writeCallback
 * @param transferSize
 * @param buffer
 * @param bufferSize
 * @return - amount of bytes actually transferred.
 */
oatpp::data::v_io_size transfer(ReadCallback* readCallback,
                                WriteCallback* writeCallback,
                                v_buff_size transferSize,
                                void* buffer,
                                v_buff_size bufferSize);
  
  
/**
 * Same as transfer but asynchronous
 */
oatpp::async::CoroutineStarter transferAsync(const std::shared_ptr<ReadCallback>& readCallback,
                                             const std::shared_ptr<WriteCallback>& writeCallback,
                                             v_buff_size transferSize,
                                             const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer);

  
}}}

#endif /* defined(_data_Stream) */
