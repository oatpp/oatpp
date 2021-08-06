/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#ifndef oatpp_web_protocol_http2_hpack_Huffman_hpp
#define oatpp_web_protocol_http2_hpack_Huffman_hpp

#include <utility>

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/Types.hpp"


namespace oatpp { namespace web { namespace protocol { namespace http2 { namespace hpack {

/**
 * Loosely oriented on nghttp2 (https://github.com/nghttp2/nghttp2)
 */
class Huffman {
 private:
  class Decode {
   public:
    bool accepted;
    bool yield;
    uint32_t id;
    uint32_t symbol;
    Decode(bool pAccepted, bool pYield, uint32_t pId, uint32_t pSymbol)
      : accepted(pAccepted)
      , yield(pYield)
      , id(pId)
      , symbol(pSymbol) {};
  };

  class Symbol {
   public:
    v_io_size bits;
    v_uint32 code;
  };

 private:
  static const Symbol symbol[257];
  static const Decode decode[257][16];

 public:
  static v_io_size encode(Payload &to, p_uint8 src, v_io_size len);
  static v_io_size calculateSize(p_uint8 src, v_io_size len);

};

}}}}}
#endif //oatpp_web_protocol_http2_hpack_Huffman_hpp
