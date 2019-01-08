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
  
  
void WebSocket::readFrameHeader(FrameHeader& frameHeader) {
  
  v_word16 bb;
  auto res = oatpp::data::stream::readExactSizeData(m_connection.get(), &bb, 2);
  if(res != 2) {
    throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::readFrameHeader()]: Error reading frame header");
  }
  
  bb = ntohs(bb);
  
  frameHeader.fin = (bb & 32768) > 0; // 32768
  frameHeader.rsv1 = (bb & 16384) > 0; // 16384
  frameHeader.rsv2 = (bb & 8192) > 0; // 8192
  frameHeader.rsv3 = (bb & 4096) > 0; // 4096
  frameHeader.opcode = (bb & 4095) >> 8;
  frameHeader.hasMask = (bb & 128) > 0;
  v_word8 messageLen1 = (bb & 127);
  
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
  
}
  
void WebSocket::readPayload(const FrameHeader& frameHeader, bool callListener) {
  
  v_char8 buffer[oatpp::data::buffer::IOBuffer::BUFFER_SIZE];
  oatpp::os::io::Library::v_size progress = 0;
  while (progress < frameHeader.payloadLength) {
    oatpp::os::io::Library::v_size desiredSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
    if(desiredSize > frameHeader.payloadLength - progress) {
      desiredSize = frameHeader.payloadLength - progress;
    }
    auto res = m_connection->read(buffer, desiredSize);
    if(res > 0) {
      if(callListener && m_listener) {
        /* decode message and call listener */
        v_char8 decoded[res];
        for(v_int32 i = 0; i < res; i ++) {
          decoded[i] = buffer[i] ^ frameHeader.mask[(i + progress) % 4];
        }
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
  if(callListener && frameHeader.fin && m_listener) {
    m_listener->readMessage(*this, nullptr, 0);
  }
  
}
  
void WebSocket::handleFrame(v_int32 opcode, const FrameHeader& frameHeader) {
  
  switch (opcode) {
    case OPCODE_CONTINUATION:
      if(m_lastOpcode < 0) {
        throw std::runtime_error("[oatpp::web::protocol::websocket::WebSocket::handleFrame()]: Invalid communication state.");
      }
      handleFrame(m_lastOpcode, frameHeader);
      return; // return here
      
    case OPCODE_TEXT:
      readPayload(frameHeader, true);
      break;
      
    case OPCODE_BINARY:
      readPayload(frameHeader, true);
      break;
      
    case OPCODE_CONNECTION_CLOSE:
      readPayload(frameHeader, false);
      if(m_listener) {
        m_listener->onConnectionClose(*this);
      }
      break;
      
    case OPCODE_PING:
      readPayload(frameHeader, false);
      if(m_listener) {
        m_listener->onPing(*this);
      }
      break;
      
    case OPCODE_PONG:
      readPayload(frameHeader, false);
      if(m_listener) {
        m_listener->onPong(*this);
      }
      break;
      
    default:
      OATPP_LOGD("[oatpp::web::protocol::websocket::WebSocket::handleFrame()]", "Unknown frame");
      break;
  }
  
  m_lastOpcode = opcode;
  
}
  
void WebSocket::listen() {
  
  try {
    FrameHeader frameHeader;
    do {
      readFrameHeader(frameHeader);
      handleFrame(frameHeader.opcode, frameHeader);
    } while(frameHeader.opcode != OPCODE_CONNECTION_CLOSE);
  } catch(...) {
    OATPP_LOGD("[oatpp::web::protocol::websocket::WebSocket::listen()]", "Unhandled error occurred");
  }
  
}
  
}}}}
