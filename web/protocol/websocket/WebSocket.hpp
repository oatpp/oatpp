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

#include "./Frame.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include <random>

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
class WebSocket {
private:
  /* Random used to generate message masks */
  static thread_local std::mt19937 RANDOM_GENERATOR;
  static thread_local std::uniform_int_distribution<size_t> RANDOM_DISTRIBUTION;
public:
  
  class Listener {
  public:
    typedef os::io::Library::v_size v_size;
    typedef oatpp::web::protocol::websocket::WebSocket WebSocket;
  public:
    
    /**
     * Called when "ping" frame received
     */
    virtual void onPing(const WebSocket& webSocket, const oatpp::String& message) = 0;
    
    /**
     * Called when "pong" frame received
     */
    virtual void onPong(const WebSocket& webSocket, const oatpp::String& message) = 0;
    
    /**
     * Called when "close" frame received
     */
    virtual void onClose(const WebSocket& webSocket, v_word16 code, const oatpp::String& message) = 0;
    
    /**
     * Called when "text" or "binary" frame received.
     * When all data of message is read, readMessage is called again with size == 0 to
     * indicate end of the message
     */
    virtual v_size readMessage(const WebSocket& webSocket, p_char8 data, v_size size) = 0;
    
  };
  
private:
  
  void generateMaskForFrame(Frame::Header& frameHeader) const;
  
  bool checkForContinuation(const Frame::Header& frameHeader);
  void readFrameHeader(Frame::Header& frameHeader) const;
  void handleFrame(const Frame::Header& frameHeader);
  
  /**
   * if(shortMessageStream == nullptr) - read call readMessage() method of listener
   * if(shortMessageStream) - read message to shortMessageStream. Don't call listener
   */
  void readPayload(const Frame::Header& frameHeader, oatpp::data::stream::ChunkedBuffer* shortMessageStream) const;

private:
  std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
  bool m_maskOutgoingMessages;
  std::shared_ptr<Listener> m_listener;
  v_int32 m_lastOpcode;
  mutable bool m_listening;
public:
  
  /**
   * maskOutgoingMessages for servers should be false. For clients should be true
   */
  WebSocket(const std::shared_ptr<oatpp::data::stream::IOStream>& connection, bool maskOutgoingMessages)
    : m_connection(connection)
    , m_maskOutgoingMessages(maskOutgoingMessages)
    , m_listener(nullptr)
    , m_lastOpcode(-1)
    , m_listening(false)
  {}
  
  std::shared_ptr<oatpp::data::stream::IOStream> getConnection() const {
    return m_connection;
  }
  
  void setListener(const std::shared_ptr<Listener>& listener) {
    m_listener = listener;
  }
  
  /**
   * Use this method if you know what you are doing.
   * Read one frame from connection and call corresponding methods of listener.
   * See WebSocket::setListener()
   */
  void iterateFrame(Frame::Header& frameHeader);
  
  /**
   * Blocks until stopListening() is called or error occurred
   * Read incoming frames and call corresponding methods of listener.
   * See WebSocket::setListener()
   */
  void listen();
  
  /**
   * Break listen loop. See WebSocket::listen()
   */
  void stopListening() const;
  
  /**
   * Use this method if you know what you are doing.
   * Send custom frame to peer.
   */
  void writeFrameHeader(const Frame::Header& frameHeader) const;
  
  /**
   * Use this method if you know what you are doing.
   * Send default frame to peer with fin, opcode and messageSize set
   */
  void sendFrameHeader(Frame::Header& frameHeader, bool fin, v_word8 opcode, v_int64 messageSize) const;
  
  /**
   * Send one frame message with custom fin and opcode
   * return true on success, false on error.
   * if false returned socket should be closed manually
   */
  bool sendOneFrame(bool fin, v_word8 opcode, const oatpp::String& message) const;
  
  /**
   * throws on error and closes socket
   */
  void sendClose(v_word16 code, const oatpp::String& message) const;
  
  /**
   * throws on error and closes socket
   */
  void sendClose() const;
  
  /**
   * throws on error and closes socket
   */
  void sendPing(const oatpp::String& message) const;
  
  /**
   * throws on error and closes socket
   */
  void sendPong(const oatpp::String& message) const;
  
  /**
   * throws on error and closes socket
   */
  void sendOneFrameText(const oatpp::String& message) const;
  
  /**
   * throws on error and closes socket
   */
  void sendOneFrameBinary(const oatpp::String& message) const;
  
};
  
}}}}

#endif /* oatpp_web_protocol_websocket_WebSocket_hpp */
