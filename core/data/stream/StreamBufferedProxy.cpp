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

#include "StreamBufferedProxy.hpp"

namespace oatpp { namespace data{ namespace stream {
  
os::io::Library::v_size OutputStreamBufferedProxy::write(const void *data, os::io::Library::v_size count) {
  if(m_pos == 0){
    
    v_bufferSize spaceLeft = m_bufferSize - m_posEnd;
    if(spaceLeft > count){
      memcpy(&m_buffer[m_posEnd], data, count);
      m_posEnd += (v_bufferSize) count;
      return count;
    }
    
    if(m_posEnd == 0) {
      return m_outputStream->write(data, count);
    }
    
    if(spaceLeft > 0){
      memcpy(&m_buffer[m_posEnd], data, spaceLeft);
      m_posEnd = m_bufferSize;
    }
    
    os::io::Library::v_size writeResult = m_outputStream->write(m_buffer, m_bufferSize);
    
    if(writeResult == m_bufferSize){
      m_posEnd = 0;
      os::io::Library::v_size bigResult = write(&((p_char8) data)[spaceLeft], count - spaceLeft);
      if(bigResult > 0) {
        return bigResult + spaceLeft;
      } else if(bigResult < 0) {
        return bigResult;
      } else {
        return spaceLeft;
      }
    }
    
    if(writeResult > 0){
      m_pos += (v_bufferSize) writeResult;
    } else if(writeResult < 0) {
      return writeResult;
    }
    
    return spaceLeft;
    
  } else {
    auto amount = m_posEnd - m_pos;
    if(amount > 0){
      os::io::Library::v_size result = m_outputStream->write(&m_buffer[m_pos], amount);
      if(result == amount){
        m_pos = 0;
        m_posEnd = 0;
        return write(data, count);
      } else if(result > 0){
        m_pos += (v_bufferSize) result;
        return 0;
      }
      return result;
    }
    m_pos = 0;
    m_posEnd = 0;
    return write(data, count);
  }
}

os::io::Library::v_size OutputStreamBufferedProxy::flush() {
  auto amount = m_posEnd - m_pos;
  if(amount > 0){
    os::io::Library::v_size result = m_outputStream->write(&m_buffer[m_pos], amount);
    if(result == amount){
      m_pos = 0;
      m_posEnd = 0;
    } else if(result > 0){
      m_pos += (v_bufferSize) result;
    }
    return result;
  }
  return 0;
}
  
oatpp::async::Action OutputStreamBufferedProxy::flushAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                            const oatpp::async::Action& actionOnFinish) {
  
  class FlushCoroutine : public oatpp::async::Coroutine<FlushCoroutine> {
  private:
    std::shared_ptr<OutputStreamBufferedProxy> m_stream;
  public:
    
    FlushCoroutine(const std::shared_ptr<OutputStreamBufferedProxy>& stream)
      : m_stream(stream)
    {}
    
    Action act() override {
      
      auto amount = m_stream->m_posEnd - m_stream->m_pos;
      if(amount > 0){
        os::io::Library::v_size result = m_stream->m_outputStream->write(&m_stream->m_buffer[m_stream->m_pos], amount);
        if(result == amount){
          m_stream->m_pos = 0;
          m_stream->m_posEnd = 0;
          return finish();
        } else if(result == IOStream::ERROR_IO_WAIT_RETRY) {
          return waitRetry();
        } else if(result > 0){
          m_stream->m_pos += (v_bufferSize) result;
        }
        return error("OutputStreamBufferedProxy. Failed to flush all data");
      }
      return finish();
      
    }
    
  };
  
  return parentCoroutine->startCoroutine<FlushCoroutine>(actionOnFinish, getSharedPtr<OutputStreamBufferedProxy>());
  
}
  
os::io::Library::v_size InputStreamBufferedProxy::read(void *data, os::io::Library::v_size count) {
  
  if (m_pos == 0 && m_posEnd == 0) {
  
    if(count > m_bufferSize){
      //if(m_hasError){
      //  errno = m_errno;
      //  return -1;
      //}
      return m_inputStream->read(data, count);
    } else {
      //if(m_hasError){
      //  errno = m_errno;
      //  return -1;
      //}
      m_posEnd = (v_bufferSize) m_inputStream->read(m_buffer, m_bufferSize);
      v_bufferSize result;
      if(m_posEnd > count){
        result = (v_bufferSize) count;
        m_pos = result;
      } else {
        result = m_posEnd;
        m_posEnd = 0;
        m_pos = 0;
        if(result < 0) {
          return result;
        }
      }
      std::memcpy(data, m_buffer, result);
      return result;
      
    }
    
  } else {
    v_bufferSize result = m_posEnd - m_pos;
    if(count > result){
      
      std::memcpy(data, &m_buffer[m_pos], result);
      
      m_pos = 0;
      m_posEnd = 0;
      os::io::Library::v_size bigResult = read(&((p_char8) data) [result], count - result);
      if(bigResult > 0){
        return bigResult + result;
      } else if(bigResult < 0) {
        return bigResult;
      }
      
      return result;
      
    } else {
      std::memcpy(data, &m_buffer[m_pos], count);
      m_pos += (v_bufferSize) count;
      if(m_pos == m_posEnd){
        m_pos = 0;
        m_posEnd = 0;
      }
      return count;
    }
  }
  
}
  
}}}
