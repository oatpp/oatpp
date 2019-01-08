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

#ifndef oatpp_web_protocol_websocket_WebSocket_hpp
#define oatpp_web_protocol_websocket_WebSocket_hpp

#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
class WebSocket {
public:
  
  static constexpr v_word8 OPCODE_CONTINUATION = 0x0;
  static constexpr v_word8 OPCODE_TEXT = 0x1;
  static constexpr v_word8 OPCODE_BINARY = 0x2;
  static constexpr v_word8 OPCODE_CONNECTION_CLOSE = 0x8;
  static constexpr v_word8 OPCODE_PING = 0x9;
  static constexpr v_word8 OPCODE_PONG = 0xA;
  
public:
  
  class Listener {
  public:
    typedef os::io::Library::v_size v_size;
    typedef oatpp::web::protocol::websocket::WebSocket WebSocket;
  public:
    
    /**
     * Called when WebSocket is connected to client/server
     */
    virtual void onConnected(const WebSocket& webSocket) = 0;
    
    /**
     * Called when "ping" frame received
     */
    virtual void onPing(const WebSocket& webSocket) = 0;
    
    /**
     * Called when "pong" frame received
     */
    virtual void onPong(const WebSocket& webSocket) = 0;
    
    /**
     * Called when "connection close" frame received
     */
    virtual void onConnectionClose(const WebSocket& webSocket) = 0;
    
    /**
     * Called when "message" frame received.
     * When all data of message is read, readMessage is called again with size == 0 to
     * indicate end of the message
     */
    virtual v_size readMessage(const WebSocket& webSocket, p_char8 data, v_size size) = 0;
    
  };
  
public:
  
  struct FrameHeader {
    bool fin;
    bool rsv1;
    bool rsv2;
    bool rsv3;
    v_word8 opcode;
    bool hasMask;
    v_int64 payloadLength;
    v_word8 mask[4] = {0, 0, 0, 0};
  };
  
private:
  void readFrameHeader(FrameHeader& frameHeader);
  void handleFrame(v_int32 opcode, const FrameHeader& frameHeader);
  void readPayload(const FrameHeader& frameHeader, bool callListener);
private:
  std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
  std::shared_ptr<Listener> m_listener;
  v_int32 m_lastOpcode;
public:
  
  WebSocket(const std::shared_ptr<oatpp::data::stream::IOStream>& connection)
    : m_connection(connection)
    , m_listener(nullptr)
    , m_lastOpcode(-1)
  {}
  
  void setListener(const std::shared_ptr<Listener>& listener) {
    m_listener = listener;
  }
  
  void listen();
  
};
  
}}}}

#endif /* oatpp_web_protocol_websocket_WebSocket_hpp */
