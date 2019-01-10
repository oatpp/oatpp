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

#include "WebSocket.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
void WebSocket::packHeaderBits(v_word16& bits, const FrameHeader& frameHeader, v_word8& messageLengthScenario) const {
  
  bits = 0;
  
  if(frameHeader.fin) bits |= 32768;
  if(frameHeader.rsv1) bits |= 16384;
  if(frameHeader.rsv2) bits |= 8192;
  if(frameHeader.rsv3) bits |= 4096;
  
  bits |= (frameHeader.opcode & 15) << 8;
  
  if(frameHeader.hasMask) bits |= 128;
  
  if(frameHeader.payloadLength < 126) {
    bits |= frameHeader.payloadLength & 127;
    messageLengthScenario = 1;
  } else if(frameHeader.payloadLength < 65536) {
    bits |= 126;
     messageLengthScenario = 2;
  } else {
    bits |= 127; // frameHeader.payloadLength > 65535
    messageLengthScenario = 3;
  }
  
}

void WebSocket::unpackHeaderBits(v_word16 bits, FrameHeader& frameHeader, v_word8& messageLen1) const {
  frameHeader.fin = (bits & 32768) > 0; // 32768
  frameHeader.rsv1 = (bits & 16384) > 0; // 16384
  frameHeader.rsv2 = (bits & 8192) > 0; // 8192
  frameHeader.rsv3 = (bits & 4096) > 0; // 4096
  frameHeader.opcode = (bits & 3840) >> 8;
  frameHeader.hasMask = (bits & 128) > 0;
  messageLen1 = (bits & 127);
}
  
bool WebSocket::checkForContinuation(const FrameHeader& frameHeader) {
  if(m_lastOpcode == OPCODE_TEXT || m_lastOpcode == OPCODE_BINARY) {
    return false;
  }
  if(frameHeader.fin) {
    m_lastOpcode = -1;
  } else {
    m_lastOpcode = frameHeader.opcode;
  }
  return true;
}
  
void WebSocket::readFrameHeader(FrameHeader& frameHeader) const {
  
  v_word16 bb;
  auto res = oatpp::data::stream::readExactSizeData(m_connection.get(), &bb, 2);
  if(res != 2) {
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readFrameHeader()]: Error reading frame header");
  }
  
  v_word8 messageLen1;
  unpackHeaderBits(ntohs(bb), frameHeader, messageLen1);
  
  if(messageLen1 < 126) {
    frameHeader.payloadLength = messageLen1;
  } else if(messageLen1 == 126) {
    v_word16 messageLen2;
    res = oatpp::data::stream::readExactSizeData(m_connection.get(), &messageLen2, 2);
    if(res != 2) {
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readFrameHeader()]: Error reading frame header. Reading payload length scenario 2.");
    }
    frameHeader.payloadLength = ntohs(messageLen2);
  } else if(messageLen1 == 127) {
    v_word32 messageLen3[2];
    res = oatpp::data::stream::readExactSizeData(m_connection.get(), &messageLen3, 8);
    if(res != 8) {
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readFrameHeader()]: Error reading frame header. Reading payload length scenario 3.");
    }
    frameHeader.payloadLength = (((v_word64) ntohl(messageLen3[0])) << 32) | ntohl(messageLen3[1]);
  }
  
  if(frameHeader.hasMask) {
    res = oatpp::data::stream::readExactSizeData(m_connection.get(), frameHeader.mask, 4);
    if(res != 4) {
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readFrameHeader()]: Error reading frame header. Reading mask.");
    }
  }
  
  OATPP_LOGD("WebSocket", "fin=%d, opcode=%d, masked=%d, messageLen1=%d",
             frameHeader.fin,
             frameHeader.opcode,
             frameHeader.hasMask,
             frameHeader.payloadLength);
  
  OATPP_LOGD("WebSocket", "rsv1=%d, rsv2=%d, rsv3=%d",
             frameHeader.rsv1,
             frameHeader.rsv2,
             frameHeader.rsv3);
  
}
  
void WebSocket::writeFrameHeader(const FrameHeader& frameHeader) const {

  v_word16 bb;
  v_word8 messageLengthScenario;
  packHeaderBits(bb, frameHeader, messageLengthScenario);
  
  bb = htons(bb);
  
  auto res = oatpp::data::stream::writeExactSizeData(m_connection.get(), &bb, 2);
  if(res != 2) {
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::writeFrameHeader()]: Error writing frame header");
  }
  
  if(messageLengthScenario == 2) {
    v_word16 messageLen2 = htons(frameHeader.payloadLength);
    res = oatpp::data::stream::writeExactSizeData(m_connection.get(), &messageLen2, 2);
    if(res != 2) {
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::writeFrameHeader()]: Error writing frame header. Writing payload length scenario 2.");
    }
  } else if(messageLengthScenario == 3) {
    v_word32 messageLen3[2];
    messageLen3[0] = htonl(frameHeader.payloadLength >> 32);
    messageLen3[1] = htonl(frameHeader.payloadLength & 0xFFFFFFFF);
    res = oatpp::data::stream::writeExactSizeData(m_connection.get(), &messageLen3, 8);
    if(res != 8) {
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::writeFrameHeader()]: Error writing frame header. Writing payload length scenario 3.");
    }
  }
  
  if(frameHeader.hasMask) {
    res = oatpp::data::stream::writeExactSizeData(m_connection.get(), frameHeader.mask, 4);
    if(res != 4) {
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::writeFrameHeader()]: Error writing frame header. Writing mask.");
    }
  }
  
}
  
void WebSocket::readPayload(const FrameHeader& frameHeader, oatpp::data::stream::ChunkedBuffer* shortMessageStream) const {
  
  if(shortMessageStream && frameHeader.payloadLength > 125) {
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readPayload()]: Invalid payloadLength. See RFC-6455, section-5.5.");
  }
  
  v_char8 buffer[oatpp::data::buffer::IOBuffer::BUFFER_SIZE];
  oatpp::os::io::Library::v_size progress = 0;
  
  while (progress < frameHeader.payloadLength) {
    
    oatpp::os::io::Library::v_size desiredSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
    if(desiredSize > frameHeader.payloadLength - progress) {
      desiredSize = frameHeader.payloadLength - progress;
    }
    
    auto res = m_connection->read(buffer, desiredSize);
    
    if(res > 0) {
      
      v_char8 decoded[res];
      for(v_int32 i = 0; i < res; i ++) {
        decoded[i] = buffer[i] ^ frameHeader.mask[(i + progress) % 4];
      }
      if(shortMessageStream) {
        shortMessageStream->write(decoded, res);
      } else if(m_listener) {
        m_listener->readMessage(*this, decoded, res);
      }
      progress += res;
      
    }else { // if res == 0 then probably stream handles read() error incorrectly. trow.
      
      if(res == oatpp::data::stream::Errors::ERROR_IO_RETRY || res == oatpp::data::stream::Errors::ERROR_IO_WAIT_RETRY) {
        continue;
      }
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readPayload()]: Invalid connection state.");
      
    }
  }
  
  /* call listener to inform abount messge end */
  if(shortMessageStream == nullptr && frameHeader.fin && m_listener) {
    m_listener->readMessage(*this, nullptr, 0);
  }
  
}
  
void WebSocket::handleFrame(const FrameHeader& frameHeader) {
  
  switch (frameHeader.opcode) {
    case OPCODE_CONTINUATION:
      if(m_lastOpcode < 0) {
        throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::handleFrame()]: Invalid communication state.");
      }
      readPayload(frameHeader, nullptr);
      break;
      
    case OPCODE_TEXT:
      if(checkForContinuation(frameHeader)) {
        readPayload(frameHeader, nullptr);
      } else {
        throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::handleFrame()]: Invalid communication state. OPCODE_CONTINUATION expected");
      }
      break;
      
    case OPCODE_BINARY:
      if(checkForContinuation(frameHeader)) {
        readPayload(frameHeader, nullptr);
      } else {
        throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::handleFrame()]: Invalid communication state. OPCODE_CONTINUATION expected");
      }
      break;
      
    case OPCODE_CLOSE:
      {
        oatpp::data::stream::ChunkedBuffer messageStream;
        readPayload(frameHeader, &messageStream);
        if(m_listener) {
          v_word16 code = 0;
          oatpp::String message;
          if(messageStream.getSize() >= 2) {
            messageStream.readSubstring(&code, 0, 2);
            code = ntohs(code);
            message = messageStream.getSubstring(2, messageStream.getSize() - 2);
          }
          if(!message) {
            message = "";
          }
          m_listener->onClose(*this, code, message);
        }
      }
      break;
      
    case OPCODE_PING:
      {
        oatpp::data::stream::ChunkedBuffer messageStream;
        readPayload(frameHeader, &messageStream);
        if(m_listener) {
          m_listener->onPing(*this, messageStream.toString());
        }
      }
      break;
      
    case OPCODE_PONG:
      {
        oatpp::data::stream::ChunkedBuffer messageStream;
        readPayload(frameHeader, &messageStream);
        if(m_listener) {
          m_listener->onPong(*this, messageStream.toString());
        }
      }
      break;
      
    default:
      throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::handleFrame()]: Unknown frame");
      break;
  }
  
}
  
void WebSocket::iterateFrame() {
  FrameHeader frameHeader;
  readFrameHeader(frameHeader);
  handleFrame(frameHeader);
}
  
void WebSocket::listen() {
  
  m_listening = true;
  
  try {
    FrameHeader frameHeader;
    do {
      readFrameHeader(frameHeader);
      handleFrame(frameHeader);
    } while(frameHeader.opcode != OPCODE_CLOSE && m_listening);
  } catch(...) {
    OATPP_LOGD("[oatpp::web::protocol::websocket::WebSocket::listen()]", "Unhandled error occurred");
  }
  
}
  
void WebSocket::stopListening() const {
  m_listening = false;
}
  
void WebSocket::sendFrame(bool fin, v_word8 opcode, v_int64 messageSize) const {
  
  oatpp::web::protocol::websocket::WebSocket::FrameHeader frame;
  frame.fin = fin;
  frame.rsv1 = false;
  frame.rsv2 = false;
  frame.rsv3 = false;
  frame.opcode = opcode;
  frame.hasMask = false;
  frame.payloadLength = messageSize;
  
  writeFrameHeader(frame);
  
}
  
bool WebSocket::sendOneFrameMessage(v_word8 opcode, const oatpp::String& message) const {
  if(message && message->getSize() > 0) {
    sendFrame(true, opcode, message->getSize());
    auto res = oatpp::data::stream::writeExactSizeData(m_connection.get(), message->getData(), message->getSize());
    if(res != message->getSize()) {
      return false;
    }
  } else {
    sendFrame(true, opcode, 0);
  }
  return true;
}
  
void WebSocket::sendClose(v_word16 code, const oatpp::String& message) const {
  
  code = htons(code);
  
  oatpp::data::stream::ChunkedBuffer buffer;
  buffer.write(&code, 2);
  if(message) {
    buffer.write(message->getData(), message->getSize());
  }
  
  if(!sendOneFrameMessage(OPCODE_CLOSE, buffer.toString())) {
    stopListening();
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::sendClose(...)]: Unknown error while writing to socket.");
  }
  
}

void WebSocket::sendClose() const {
  if(!sendOneFrameMessage(OPCODE_CLOSE, nullptr)) {
    stopListening();
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::sendClose()]: Unknown error while writing to socket.");
  }
}
  
void WebSocket::sendPing(const oatpp::String& message) const {
  if(!sendOneFrameMessage(OPCODE_PING, message)) {
    stopListening();
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::sendPing()]: Unknown error while writing to socket.");
  }
}

void WebSocket::sendPong(const oatpp::String& message) const {
  if(!sendOneFrameMessage(OPCODE_PONG, message)) {
    stopListening();
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::sendPong()]: Unknown error while writing to socket.");
  }
}

void WebSocket::sendOneFrameText(const oatpp::String& message) const {
  if(!sendOneFrameMessage(OPCODE_TEXT, message)) {
    stopListening();
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::sendOneFrameText()]: Unknown error while writing to socket.");
  }
}

void WebSocket::sendOneFrameBinary(const oatpp::String& message) const {
  if(!sendOneFrameMessage(OPCODE_BINARY, message)) {
    stopListening();
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::sendOneFrameBinary()]: Unknown error while writing to socket.");
  }
}
  
}}}}
