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

#ifndef oatpp_web_protocol_http_outgoing_BufferBody_hpp
#define oatpp_web_protocol_http_outgoing_BufferBody_hpp

#include "./Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Implementation of &id:oatpp::web::protocol::http::outgoing::Body; class.
 * Implements functionality to use &id::oatpp::String; as data source for http body.
 */
class BufferBody : public oatpp::base::Countable, public Body, public std::enable_shared_from_this<BufferBody> {
public:
  OBJECT_POOL(Http_Outgoing_BufferBody_Pool, BufferBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_BufferBody_Pool, BufferBody, 32)
private:
  oatpp::String m_buffer;
public:
  BufferBody(const oatpp::String& buffer);
public:

  /**
   * Create shared BufferBody.
   * @param buffer - &id:oatpp::String;.
   * @return - `std::shared_ptr` to BufferBody.
   */
  static std::shared_ptr<BufferBody> createShared(const oatpp::String& buffer);

  /**
   * Declare `Content-Length` header.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  void declareHeaders(Headers& headers) noexcept override;

  /**
   * Write body data to stream.
   * @param stream - pointer to &id:oatpp::data::stream::OutputStream;.
   */
  void writeToStream(OutputStream* stream) noexcept override;
  
public:

  /**
   * Coroutine used to write &l:BufferBody; to &id:oatpp::data::stream::OutputStream;.
   */
  class WriteToStreamCoroutine : public oatpp::async::Coroutine<WriteToStreamCoroutine> {
  private:
    std::shared_ptr<BufferBody> m_body;
    std::shared_ptr<OutputStream> m_stream;
    oatpp::data::stream::AsyncInlineWriteData m_inlineData;
  public:

    /**
     * Constructor.
     * @param body - &l:BufferBody;.
     * @param stream - &id:oatpp::data::stream::OutputStream;.
     */
    WriteToStreamCoroutine(const std::shared_ptr<BufferBody>& body,
                           const std::shared_ptr<OutputStream>& stream);
    
    Action act() override;
    
  };
  
public:

  /**
   * Start &l:BufferBody::WriteToStreamCoroutine; to write buffer data to stream.
   * @param stream - &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) override;
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_BufferBody_hpp */
