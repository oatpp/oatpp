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

#ifndef oatpp_web_protocol_websocket_Frame_hpp
#define oatpp_web_protocol_websocket_Frame_hpp

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
class Frame {
public:
  
  static constexpr v_word8 OPCODE_CONTINUATION = 0x0;
  static constexpr v_word8 OPCODE_TEXT = 0x1;
  static constexpr v_word8 OPCODE_BINARY = 0x2;
  static constexpr v_word8 OPCODE_CLOSE = 0x8;
  static constexpr v_word8 OPCODE_PING = 0x9;
  static constexpr v_word8 OPCODE_PONG = 0xA;
  
public:
  
  struct Header {
    bool fin;
    bool rsv1;
    bool rsv2;
    bool rsv3;
    v_word8 opcode;
    bool hasMask;
    v_int64 payloadLength;
    v_word8 mask[4] = {0, 0, 0, 0};
  };
  
public:
  
  static void packHeaderBits(v_word16& bits, const Header& frameHeader, v_word8& messageLengthScenario);
  static void unpackHeaderBits(v_word16 bits, Header& frameHeader, v_word8& messageLen1);
  
};
  
}}}}

#endif /* oatpp_web_protocol_websocket_Frame_hpp */
