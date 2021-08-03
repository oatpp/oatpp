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

#ifndef oatpp_web_protocol_http_outgoing_StreamingBody_hpp
#define oatpp_web_protocol_http_outgoing_StreamingBody_hpp

#include "./Body.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Abstract body for streaming data.
 */
class StreamingBody : public Body {
private:
  std::shared_ptr<data::stream::ReadCallback> m_readCallback;
public:

  /**
   * Constructor.
   * @param readCallback
   */
  StreamingBody(const std::shared_ptr<data::stream::ReadCallback>& readCallback);

  /**
   * Proxy method to readCallback::read().
   * @param buffer - pointer to buffer.
   * @param count - size of the buffer in bytes.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written to buffer. 0 - to indicate end-of-file.
   */
  v_io_size read(void *buffer, v_buff_size count, async::Action& action) override;

  /**
   * Override this method to declare additional headers.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  void declareHeaders(Headers& headers) override;

  /**
   * Pointer to the body known data.
   * @return - `nullptr`.
   */
  p_char8 getKnownData() override;

  /**
   * Return known size of the body.
   * @return - `-1`.
   */
  v_int64 getKnownSize() override;

};

}}}}}

#endif // oatpp_web_protocol_http_outgoing_StreamingBody_hpp
