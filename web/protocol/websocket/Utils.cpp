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

#include "Utils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace websocket {
 
thread_local std::mt19937 Utils::RANDOM_GENERATOR (std::random_device{}());
thread_local std::uniform_int_distribution<size_t> Utils::RANDOM_DISTRIBUTION (0, 255);

void Utils::generateMaskForFrame(Frame::Header& frameHeader) {
  for(v_int32 i = 0; i < 4; i ++) {
    frameHeader.mask[i] = RANDOM_DISTRIBUTION(RANDOM_GENERATOR);
  }
}
  
}}}}
