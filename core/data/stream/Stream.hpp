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

#ifndef oatpp_data_Stream
#define oatpp_data_Stream

#include "oatpp/core/data/buffer/IOBuffer.hpp"

#include "oatpp/core/base/String.hpp"

#include "oatpp/core/async/Coroutine.hpp"

#include "oatpp/core/base/PtrWrapper.hpp"
#include "oatpp/core/os/io/Library.hpp"

namespace oatpp { namespace data{ namespace stream {
  
class OutputStream {
public:
  
  virtual os::io::Library::v_size write(const void *data, os::io::Library::v_size count) = 0;
  
  os::io::Library::v_size write(const char* data){
    return write((p_char8)data, std::strlen(data));
  }
  
  os::io::Library::v_size write(const oatpp::base::PtrWrapper<oatpp::base::String>& str){
    return write(str->getData(), str->getSize());
  }
  
  os::io::Library::v_size writeChar(v_char8 c){
    return write(&c, 1);
  }
  
  os::io::Library::v_size writeAsString(v_int32 value);
  os::io::Library::v_size writeAsString(v_int64 value);
  os::io::Library::v_size writeAsString(v_float32 value);
  os::io::Library::v_size writeAsString(v_float64 value);
  os::io::Library::v_size writeAsString(bool value);
  
};
  
class InputStream {
public:
  virtual os::io::Library::v_size read(void *data, os::io::Library::v_size count) = 0;
};
  
class IOStream : public InputStream, public OutputStream {
public:
  constexpr static os::io::Library::v_size ERROR_IO_NOTHING_TO_READ = -1001;
  constexpr static os::io::Library::v_size ERROR_IO_WAIT_RETRY = -1002;
  constexpr static os::io::Library::v_size ERROR_IO_RETRY = -1003;
  
  constexpr static const char* const ERROR_ASYNC_FAILED_TO_WRITE_DATA = "ERROR_ASYNC_FAILED_TO_WRITE_DATA";
  constexpr static const char* const ERROR_ASYNC_FAILED_TO_READ_DATA = "ERROR_ASYNC_FAILED_TO_READ_DATA";
  
public:
  typedef os::io::Library::v_size v_size;
  
  /**
   *  Async write data withot starting new Coroutine.
   *  Should be called from a separate Coroutine method
   */
  static oatpp::async::Action writeDataAsyncInline(oatpp::data::stream::OutputStream* stream,
                                                   const void*& data,
                                                   os::io::Library::v_size& size,
                                                   const oatpp::async::Action& nextAction);
  
  static oatpp::async::Action readSomeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                                      void*& data,
                                                      os::io::Library::v_size& bytesLeftToRead,
                                                      const oatpp::async::Action& nextAction);
  
  static oatpp::async::Action readExactSizeDataAsyncInline(oatpp::data::stream::InputStream* stream,
                                                           void*& data,
                                                           os::io::Library::v_size& bytesLeftToRead,
                                                           const oatpp::async::Action& nextAction);
  
};

class CompoundIOStream : public oatpp::base::Controllable, public IOStream {
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
  
  os::io::Library::v_size write(const void *data, os::io::Library::v_size count) override {
    return m_outputStream->write(data, count);
  }
  
  os::io::Library::v_size read(void *data, os::io::Library::v_size count) override {
    return m_inputStream->read(data, count);
  }
    
};
  
const std::shared_ptr<OutputStream>& operator <<
(const std::shared_ptr<OutputStream>& s, const base::String::PtrWrapper& str);

const std::shared_ptr<OutputStream>& operator <<
(const std::shared_ptr<OutputStream>& s, const char* str);
  
const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_int32 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_int64 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_float32 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, v_float64 value);

const std::shared_ptr<OutputStream>& operator << (const std::shared_ptr<OutputStream>& s, bool value);
  
void transfer(const std::shared_ptr<InputStream>& fromStream,
              const std::shared_ptr<OutputStream>& toStream,
              oatpp::os::io::Library::v_size transferSize,
              void* buffer,
              oatpp::os::io::Library::v_size bufferSize);
  
oatpp::async::Action transferAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   const oatpp::async::Action& actionOnReturn,
                                   const std::shared_ptr<InputStream>& fromStream,
                                   const std::shared_ptr<OutputStream>& toStream,
                                   oatpp::os::io::Library::v_size transferSize,
                                   const std::shared_ptr<oatpp::data::buffer::IOBuffer>& buffer);
  
}}}

#endif /* defined(_data_Stream) */
