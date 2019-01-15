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

#include "./Utils.hpp"

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
  
oatpp::async::Action AsyncWebSocket::writeFrameHeaderAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                           const Action& actionOnReturn,
                                                           const std::shared_ptr<Frame::Header>& frameHeader)
{
  
  class WriteFrameCoroutine : public oatpp::async::Coroutine<WriteFrameCoroutine> {
  private:
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    std::shared_ptr<Frame::Header> m_frameHeader;
  private:
    v_int32 m_lenType;
    v_word16 m_bb;
    v_word16 m_messageLen2;
    v_word32 m_messageLen3 [2];
    v_word8 m_messageLengthScenario;
  private:
    const void* m_currData;
    os::io::Library::v_size m_bytesToWrite;
  public:
    
    WriteFrameCoroutine(const std::shared_ptr<oatpp::data::stream::IOStream> connection,
                        const std::shared_ptr<Frame::Header>& frameHeader)
      : m_connection(connection)
      , m_frameHeader(frameHeader)
    {
      ;
      Frame::packHeaderBits(m_bb, *m_frameHeader, m_messageLengthScenario);
      m_bb = htons(m_bb);
      m_currData = &m_bb;
      m_bytesToWrite = 2;
    }
    
    Action act() override {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(m_connection.get(), m_currData, m_bytesToWrite, yieldTo(&WriteFrameCoroutine::onBbWritten));
    }
    
    Action onBbWritten() {
      if(m_messageLengthScenario == 2) {
        m_messageLen2 = htons(m_frameHeader->payloadLength);
        m_currData = &m_messageLen2;
        m_bytesToWrite = 2;
        return yieldTo(&WriteFrameCoroutine::writeMessageLen);
      } else if(m_messageLengthScenario == 3) {
        m_messageLen3[0] = htonl(m_frameHeader->payloadLength >> 32);
        m_messageLen3[1] = htonl(m_frameHeader->payloadLength & 0xFFFFFFFF);
        m_currData = m_messageLen3;
        m_bytesToWrite = 8;
        return yieldTo(&WriteFrameCoroutine::writeMessageLen);
      }
      return yieldTo(&WriteFrameCoroutine::onLenWritten);
    }
    
    Action writeMessageLen() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(m_connection.get(), m_currData, m_bytesToWrite, yieldTo(&WriteFrameCoroutine::onLenWritten));
    }
    
    Action onLenWritten() {
      if(m_frameHeader->hasMask) {
        m_currData = m_frameHeader->mask;
        m_bytesToWrite = 4;
        return yieldTo(&WriteFrameCoroutine::writeMask);
      }
      return finish();
    }
    
    Action writeMask() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(m_connection.get(), m_currData, m_bytesToWrite, finish());
    }
    
  };
  
  return parentCoroutine->startCoroutine<WriteFrameCoroutine>(actionOnReturn, m_connection, frameHeader);
  
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
  
oatpp::async::Action AsyncWebSocket::handleFrameAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                      const Action& actionOnReturn,
                                                      const std::shared_ptr<Frame::Header>& frameHeader)
{
  
  class HandleFrameCoroutine : public oatpp::async::Coroutine<HandleFrameCoroutine> {
  private:
    std::shared_ptr<AsyncWebSocket> m_socket;
    std::shared_ptr<Frame::Header> m_frameHeader;
    std::shared_ptr<Listener> m_listener;
  private:
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_shortMessageStream;
  public:
    HandleFrameCoroutine(const std::shared_ptr<AsyncWebSocket>& socket,
                         const std::shared_ptr<Frame::Header>& frameHeader)
      : m_socket(socket)
      , m_frameHeader(frameHeader)
      , m_listener(socket->m_listener)
    {}
    
    Action act() override {
      
      switch (m_frameHeader->opcode) {
        case Frame::OPCODE_CONTINUATION:
          if(m_socket->m_lastOpcode < 0) {
            throw std::runtime_error("[oatpp::web::protocol::websocket::AsyncWebSocket::handleFrameAsync(){HandleFrameCoroutine}]: Invalid communication state.");
          }
          return m_socket->readPayloadAsync(this, finish(), m_frameHeader, nullptr);
          
        case Frame::OPCODE_TEXT:
          if(m_socket->checkForContinuation(*m_frameHeader)) {
            return m_socket->readPayloadAsync(this, finish(), m_frameHeader, nullptr);
          } else {
            throw std::runtime_error("[oatpp::web::protocol::websocket::AsyncWebSocket::handleFrameAsync(){HandleFrameCoroutine}]: Invalid communication state. OPCODE_CONTINUATION expected");
          }
          
        case Frame::OPCODE_BINARY:
          if(m_socket->checkForContinuation(*m_frameHeader)) {
            return m_socket->readPayloadAsync(this, finish(), m_frameHeader, nullptr);
          } else {
            throw std::runtime_error("[oatpp::web::protocol::websocket::AsyncWebSocket::handleFrameAsync(){HandleFrameCoroutine}]: Invalid communication state. OPCODE_CONTINUATION expected");
          }
          
        case Frame::OPCODE_CLOSE:
          m_shortMessageStream = oatpp::data::stream::ChunkedBuffer::createShared();
          return m_socket->readPayloadAsync(this, yieldTo(&HandleFrameCoroutine::onClose), m_frameHeader, m_shortMessageStream);
          
        case Frame::OPCODE_PING:
          m_shortMessageStream = oatpp::data::stream::ChunkedBuffer::createShared();
          return m_socket->readPayloadAsync(this, yieldTo(&HandleFrameCoroutine::onPing), m_frameHeader, m_shortMessageStream);
          
        case Frame::OPCODE_PONG:
          m_shortMessageStream = oatpp::data::stream::ChunkedBuffer::createShared();
          return m_socket->readPayloadAsync(this, yieldTo(&HandleFrameCoroutine::onPong), m_frameHeader, m_shortMessageStream);
          
        default:
          throw std::runtime_error("[oatpp::web::protocol::websocket::AsyncWebSocket::handleFrameAsync(){HandleFrameCoroutine}]: Unknown frame");
          break;
      }
      
    }
    
    Action onClose() {
      if(m_listener) {
        v_word16 code = 0;
        oatpp::String message;
        if(m_shortMessageStream->getSize() >= 2) {
          m_shortMessageStream->readSubstring(&code, 0, 2);
          code = ntohs(code);
          message = m_shortMessageStream->getSubstring(2, m_shortMessageStream->getSize() - 2);
        }
        if(!message) {
          message = "";
        }
        return m_listener->onClose(this, finish(), m_socket, code, message);
      }
      return finish();
    }
    
    Action onPing() {
      if(m_listener) {
        return m_listener->onPing(this, finish(), m_socket, m_shortMessageStream->toString());
      }
      return finish();
    }
    
    Action onPong() {
      if(m_listener) {
        return m_listener->onPong(this, finish(), m_socket, m_shortMessageStream->toString());
      }
      return finish();
    }
    
  };
  
  return parentCoroutine->startCoroutine<HandleFrameCoroutine>(actionOnReturn, getSharedPtr<AsyncWebSocket>(), frameHeader);
  
}
  
oatpp::async::Action AsyncWebSocket::listenAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn) {

  class ListenCoroutine : public oatpp::async::Coroutine<ListenCoroutine> {
  private:
    std::shared_ptr<AsyncWebSocket> m_socket;
    std::shared_ptr<Frame::Header> m_frameHeader;
  public:
    
    ListenCoroutine(const std::shared_ptr<AsyncWebSocket>& socket)
      : m_socket(socket)
      , m_frameHeader(std::make_shared<Frame::Header>())
    {
      m_frameHeader->opcode = -1;
    }
    
    Action act() override {
      if(m_frameHeader->opcode != Frame::OPCODE_CLOSE ) {
        return m_socket->readFrameHeaderAsync(this,
                                              m_socket->handleFrameAsync(this,
                                                                         yieldTo(&ListenCoroutine::act),
                                                                         m_frameHeader),
                                              m_frameHeader);
      }
      return finish();
    }
    
    Action handleError(const async::Error& error) override {
      return finish();
    }
    
  };
  
  return parentCoroutine->startCoroutine<ListenCoroutine>(actionOnReturn, getSharedPtr<AsyncWebSocket>());
  
}
  
oatpp::async::Action AsyncWebSocket::sendFrameHeaderAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                          const Action& actionOnReturn,
                                                          const std::shared_ptr<Frame::Header>& frameHeader,
                                                          bool fin, v_word8 opcode, v_int64 messageSize)
{
  
  frameHeader->fin = fin;
  frameHeader->rsv1 = false;
  frameHeader->rsv2 = false;
  frameHeader->rsv3 = false;
  frameHeader->opcode = opcode;
  frameHeader->hasMask = m_maskOutgoingMessages;
  frameHeader->payloadLength = messageSize;
  
  if(frameHeader->hasMask) {
    Utils::generateMaskForFrame(*frameHeader);
  }
  
  return writeFrameHeaderAsync(parentCoroutine, actionOnReturn, frameHeader);
}
  
oatpp::async::Action AsyncWebSocket::sendOneFrameAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                       const Action& actionOnReturn,
                                                       bool fin, v_word8 opcode, const oatpp::String& message)
{
  
  class SendFrameCoroutine : public oatpp::async::Coroutine<SendFrameCoroutine> {
  private:
    std::shared_ptr<AsyncWebSocket> m_socket;
    bool m_fin;
    v_word8 m_opcode;
    oatpp::String m_message;
    std::shared_ptr<Frame::Header> m_frameHeader;
  private:
    p_char8 m_encoded = nullptr;
  private:
    const void* m_currData;
    os::io::Library::v_size m_bytesToWrite;
  public:
    SendFrameCoroutine(const std::shared_ptr<AsyncWebSocket>& socket,
                       bool fin, v_word8 opcode, const oatpp::String& message)
      : m_socket(socket)
      , m_fin(fin)
      , m_opcode(opcode)
      , m_message(message)
      , m_frameHeader(std::make_shared<Frame::Header>())
    {}
    
    ~SendFrameCoroutine() {
      if(m_encoded != nullptr) {
        delete [] m_encoded;
      }
    }
    
    Action act() override {
      if(m_message && m_message->getSize() > 0) {
        return m_socket->sendFrameHeaderAsync(this, yieldTo(&SendFrameCoroutine::prepareWriteMessage), m_frameHeader, m_fin, m_opcode, m_message->getSize());
      } else {
        return m_socket->sendFrameHeaderAsync(this, finish(), m_frameHeader, m_fin, m_opcode, 0);
      }
    }
    
    Action prepareWriteMessage() {
      if(m_frameHeader->hasMask) {
        m_encoded = new v_char8[m_message->getSize()];
        for(v_int32 i = 0; i < m_message->getSize(); i ++) {
          m_encoded[i] = m_message->getData()[i] ^ m_frameHeader->mask[i % 4];
        }
        m_currData = m_encoded;
        m_bytesToWrite = m_message->getSize();
      } else {
        m_currData = m_message->getData();
        m_bytesToWrite = m_message->getSize();
      }
      return yieldTo(&SendFrameCoroutine::writeMessage);
    }
    
    Action writeMessage() {
      return oatpp::data::stream::writeExactSizeDataAsyncInline(m_socket->m_connection.get(), m_currData, m_bytesToWrite, finish());
    }
    
  };
  
  return parentCoroutine->startCoroutine<SendFrameCoroutine>(actionOnReturn, getSharedPtr<AsyncWebSocket>(), fin, opcode, message);
  
}
  
oatpp::async::Action AsyncWebSocket::sendCloseAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn, v_word16 code, const oatpp::String& message) {

  code = htons(code);
  
  oatpp::data::stream::ChunkedBuffer buffer;
  buffer.write(&code, 2);
  if(message) {
    buffer.write(message->getData(), message->getSize());
  }
  
  return sendOneFrameAsync(parentCoroutine, actionOnReturn, true, Frame::OPCODE_CLOSE, buffer.toString());
  
}

oatpp::async::Action AsyncWebSocket::sendCloseAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn) {
  return sendOneFrameAsync(parentCoroutine, actionOnReturn, true, Frame::OPCODE_CLOSE, nullptr);
}

oatpp::async::Action AsyncWebSocket::sendPingAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn, const oatpp::String& message) {
  return sendOneFrameAsync(parentCoroutine, actionOnReturn, true, Frame::OPCODE_PING, message);
}

oatpp::async::Action AsyncWebSocket::sendPongAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn, const oatpp::String& message) {
  return sendOneFrameAsync(parentCoroutine, actionOnReturn, true, Frame::OPCODE_PONG, message);
}

oatpp::async::Action AsyncWebSocket::sendOneFrameTextAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn, const oatpp::String& message) {
  return sendOneFrameAsync(parentCoroutine, actionOnReturn, true, Frame::OPCODE_TEXT, message);
}

oatpp::async::Action AsyncWebSocket::sendOneFrameBinaryAsync(oatpp::async::AbstractCoroutine* parentCoroutine, const Action& actionOnReturn, const oatpp::String& message) {
  return sendOneFrameAsync(parentCoroutine, actionOnReturn, true, Frame::OPCODE_BINARY, message);
}
  
  
}}}}
