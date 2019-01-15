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

#include "AsyncWebSocket.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
bool AsyncWebSocket::checkForContinuation(const Frame::Header& frameHeader) {
  if(m_lastOpcode == Frame::OPCODE_TEXT || m_lastOpcode == Frame::OPCODE_BINARY) {
    return false;
  }
  if(frameHeader.fin) {
    m_lastOpcode = -1;
  } else {
    m_lastOpcode = frameHeader.opcode;
  }
  return true;
}
  
oatpp::async::Action AsyncWebSocket::readFrameHeaderAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                          const Action& actionOnReturn,
                                                          const std::shared_ptr<Frame::Header>& frameHeader)
{
  
  class ReadFrameCoroutine : public oatpp::async::Coroutine<ReadFrameCoroutine> {
  private:
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    std::shared_ptr<Frame::Header> m_frameHeader;
  private:
    v_int32 m_lenType;
    v_word16 m_bb;
    v_word16 m_messageLen2;
    v_word32 m_messageLen3 [2];
  private:
    void* m_currData;
    os::io::Library::v_size m_bytesToRead;
  public:
    
    ReadFrameCoroutine(const std::shared_ptr<oatpp::data::stream::IOStream> connection,
                       const std::shared_ptr<Frame::Header>& frameHeader)
      : m_connection(connection)
      , m_frameHeader(frameHeader)
    {
      m_currData = &m_bb;
      m_bytesToRead = 2;
    }
    
    Action act() override {
      return oatpp::data::stream::readExactSizeDataAsyncInline(m_connection.get(), m_currData, m_bytesToRead, yieldTo(&ReadFrameCoroutine::onBbRead));
    }
    
    Action onBbRead() {
      
      v_word8 messageLen1;
      Frame::unpackHeaderBits(ntohs(m_bb), *m_frameHeader, messageLen1);
      
      if(messageLen1 < 126) {
        m_lenType = 1;
        m_frameHeader->payloadLength = messageLen1;
        return yieldTo(&ReadFrameCoroutine::onLenRead);
      } else if(messageLen1 == 126) {
        m_lenType = 2;
        m_currData = &m_messageLen2;
        m_bytesToRead = 2;
        return yieldTo(&ReadFrameCoroutine::readLen);
      } else if(messageLen1 == 127) {
        m_lenType = 3;
        m_currData = m_messageLen3;
        m_bytesToRead = 8;
        return yieldTo(&ReadFrameCoroutine::readLen);
      }
      
      return error("[oatpp::web::protocol::websocket::AsyncWebSocket::readFrameHeaderAsync(){ReadFrameCoroutine}]: Invalid frame payload length.");
      
    }
    
    Action readLen() {
      return oatpp::data::stream::readExactSizeDataAsyncInline(m_connection.get(), m_currData, m_bytesToRead, yieldTo(&ReadFrameCoroutine::onLenRead));
    }
    
    Action onLenRead() {
      
      if(m_lenType == 2) {
        m_frameHeader->payloadLength = ntohs(m_messageLen2);
      } else if(m_lenType == 3) {
        m_frameHeader->payloadLength = (((v_word64) ntohl(m_messageLen3[0])) << 32) | ntohl(m_messageLen3[1]);
      }
      
      if(m_frameHeader->hasMask) {
        m_currData = m_frameHeader->mask;
        m_bytesToRead = 4;
        return yieldTo(&ReadFrameCoroutine::readMask);
      }
      
      return finish();
      
    }
    
    Action readMask() {
      return oatpp::data::stream::readExactSizeDataAsyncInline(m_connection.get(), m_currData, m_bytesToRead, finish());
    }
    
  };
  
  return parentCoroutine->startCoroutine<ReadFrameCoroutine>(actionOnReturn, m_connection, frameHeader);
  
}
  
oatpp::async::Action AsyncWebSocket::readPayloadAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                      const Action& actionOnReturn,
                                                      const std::shared_ptr<Frame::Header>& frameHeader,
                                                      const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& shortMessageStream)
{
  
  class ReadPayloadCoroutine : public oatpp::async::Coroutine<ReadPayloadCoroutine> {
  private:
    std::shared_ptr<AsyncWebSocket> m_socket;
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    std::shared_ptr<Frame::Header> m_frameHeader;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_shortMessageStream;
    std::shared_ptr<Listener> m_listener;
  private:
    p_char8 m_buffer;
    oatpp::os::io::Library::v_size m_progress;
  private:
    void* m_currData;
    os::io::Library::v_size m_bytesToRead;
  public:
    ReadPayloadCoroutine(const std::shared_ptr<AsyncWebSocket>& socket,
                         const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
                         const std::shared_ptr<Frame::Header>& frameHeader,
                         const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& shortMessageStream,
                         const std::shared_ptr<Listener>& listener)
      : m_socket(socket)
      , m_connection(connection)
      , m_frameHeader(frameHeader)
      , m_shortMessageStream(shortMessageStream)
      , m_listener(listener)
      , m_buffer(new v_char8[oatpp::data::buffer::IOBuffer::BUFFER_SIZE])
      , m_progress(0)
    {}
    
    ~ReadPayloadCoroutine() {
      delete [] m_buffer;
    }
    
    Action act() {
      
      if(m_progress < m_frameHeader->payloadLength) {
        
        oatpp::os::io::Library::v_size desiredSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
        if(desiredSize > m_frameHeader->payloadLength - m_progress) {
          desiredSize = m_frameHeader->payloadLength - m_progress;
        }
        
        m_currData = m_buffer;
        m_bytesToRead = desiredSize;
        return yieldTo(&ReadPayloadCoroutine::readData);
        
      }
      
      if(m_shortMessageStream == nullptr && m_frameHeader->fin && m_listener) {
        return m_listener->readMessage(this, finish(), m_socket, nullptr, 0);
      }
      
      return finish();
      
    }
    
    Action readData() {
      return oatpp::data::stream::readSomeDataAsyncInline(m_connection.get(), m_currData, m_bytesToRead, yieldTo(&ReadPayloadCoroutine::onDataRead));
    }
    
    Action onDataRead() {
      
      auto readResult = (v_int64)m_currData - (v_int64)m_buffer;
      
      if(readResult > 0) {
      
        if(m_frameHeader->hasMask) {
          v_char8 decoded[readResult];
          for(v_int32 i = 0; i < readResult; i ++) {
            decoded[i] = m_buffer[i] ^ m_frameHeader->mask[(i + m_progress) % 4];
          }
          
          m_progress += readResult;
          
          if(m_shortMessageStream) {
            /* this is RAM stream. Non-blocking call */
            m_shortMessageStream->write(decoded, readResult);
          } else if(m_listener) {
            return m_listener->readMessage(this, yieldTo(&ReadPayloadCoroutine::act), m_socket, decoded, readResult);
          }
          
        } else {
          
          m_progress += readResult;
          
          if(m_shortMessageStream) {
            /* this is RAM stream. Non-blocking call */
            m_shortMessageStream->write(m_buffer, readResult);
          } else if(m_listener) {
            return m_listener->readMessage(this, yieldTo(&ReadPayloadCoroutine::act), m_socket, m_buffer, readResult);
          }
          
        }
        
        return yieldTo(&ReadPayloadCoroutine::act);
        
      }
      
      throw std::runtime_error("[oatpp::web::protocol::websocket::AsyncWebSocket::readPayloadAsync(){ReadPayloadCoroutine}]: Invalid connection state.");
      
    }
    
  };
  
  if(shortMessageStream && frameHeader->payloadLength > 125) {
    throw std::runtime_error("[oatpp::web::protocol::websocket::AsyncWebSocket::readPayloadAsync()]: Invalid frame payload length. See RFC-6455, section-5.5.");
  }
  
  // Call Coroutine here
  return parentCoroutine->startCoroutine<ReadPayloadCoroutine>(actionOnReturn, getSharedPtr<AsyncWebSocket>(), m_connection, frameHeader, shortMessageStream, m_listener);
  
}
  
}}}}
