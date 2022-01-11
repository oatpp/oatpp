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

#ifndef oatpp_web_protocol_http_outgoing_Body_hpp
#define oatpp_web_protocol_http_outgoing_Body_hpp

#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Abstract http outgoing body.
 * You may extend this class in order to implement custom body transferring functionality.
 */
class Body : public data::stream::ReadCallback {
protected:
  typedef http::Headers Headers;
public:

  /**
   * Virtual destructor.
   */
  virtual ~Body() = default;

  ///////////////////////////
  // From the ReadCallback //
  //
  //virtual v_io_size read(void *buffer, v_buff_size count, async::Action& action) = 0;

  /**
   * Declare headers describing body.
   * **Note:** Do NOT declare the `Content-Length` header.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  virtual void declareHeaders(Headers& headers) = 0;

  /**
   * Pointer to the body known data.
   * @return - `p_char8`.
   */
  virtual p_char8 getKnownData() = 0;

  /**
   * Should return the known size of the body (if known).
   * If body size is unknown then should return -1.
   * @return - &id:oatpp::v_io_size;.
   */
  virtual v_int64 getKnownSize() = 0;
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Body_hpp */
