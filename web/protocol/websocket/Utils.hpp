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

#ifndef oatpp_web_protocol_websocket_Utils_hpp
#define oatpp_web_protocol_websocket_Utils_hpp

#include "./Frame.hpp"
#include <random>

namespace oatpp { namespace web { namespace protocol { namespace websocket {
  
class Utils {
private:
  /* Random used to generate message masks */
  static thread_local std::mt19937 RANDOM_GENERATOR;
  static thread_local std::uniform_int_distribution<size_t> RANDOM_DISTRIBUTION;
public:
  static void generateMaskForFrame(Frame::Header& frameHeader);
};
  
}}}}

#endif /* oatpp_web_protocol_websocket_Utils_hpp */
