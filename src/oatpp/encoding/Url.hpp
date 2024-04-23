/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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

#ifndef oatpp_encoding_Url_hpp
#define oatpp_encoding_Url_hpp

#include "Hex.hpp"
#include "oatpp/data/stream/Stream.hpp"

namespace oatpp { namespace encoding {

class Url {
public:

  struct Config {

    bool spaceToPlus = false;
    const char* hexAlphabet = Hex::ALPHABET_UPPER;
    bool allowedChars[256];

    Config();

    void allowChar(v_char8 c);
    void allowCharRange(v_char8 from, v_char8 to);

    void disallowChar(v_char8 c);
    void disallowCharRange(v_char8 from, v_char8 to);

  };

public:

  static void encode(data::stream::ConsistentOutputStream* stream, const void* data, v_buff_size size, const Config& config);
  static void decode(data::stream::ConsistentOutputStream* stream, const void* data, v_buff_size size);

  static oatpp::String encode(const oatpp::String& data, const Config& config);
  static oatpp::String decode(const oatpp::String& data);

};

}}

#endif //oatpp_encoding_Url_hpp
