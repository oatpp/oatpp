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

#include "Frame.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {

void Frame::packHeaderBits(v_word16& bits, const Header& frameHeader, v_word8& messageLengthScenario) {
  
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

void Frame::unpackHeaderBits(v_word16 bits, Header& frameHeader, v_word8& messageLen1) {
  frameHeader.fin = (bits & 32768) > 0; // 32768
  frameHeader.rsv1 = (bits & 16384) > 0; // 16384
  frameHeader.rsv2 = (bits & 8192) > 0; // 8192
  frameHeader.rsv3 = (bits & 4096) > 0; // 4096
  frameHeader.opcode = (bits & 3840) >> 8;
  frameHeader.hasMask = (bits & 128) > 0;
  messageLen1 = (bits & 127);
}
  
}}}}
