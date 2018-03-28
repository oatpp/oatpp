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
#include "../../utils/ConversionUtils.hpp"

namespace oatpp { namespace data{ namespace stream {
  
os::io::Library::v_size IOStream::ERROR_NOTHING_TO_READ = -1001;
os::io::Library::v_size IOStream::ERROR_CLOSED = -1002;
os::io::Library::v_size IOStream::ERROR_TRY_AGAIN = -1003;
  
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
(const std::shared_ptr<OutputStream>& s, const base::String::PtrWrapper& str) {
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
  
void transfer(const std::shared_ptr<InputStream>& fromStream,
              const std::shared_ptr<OutputStream>& toStream,
              oatpp::os::io::Library::v_size transferSize,
              void* buffer,
              oatpp::os::io::Library::v_size bufferSize) {
  
  while (transferSize > 0) {
    oatpp::os::io::Library::v_size desiredReadCount = transferSize;
    if(desiredReadCount > bufferSize){
      desiredReadCount = bufferSize;
    }
    auto readCount = fromStream->read(buffer, desiredReadCount);
    toStream->write(buffer, readCount);
    transferSize -= readCount;
  }
  
}
  
}}}
