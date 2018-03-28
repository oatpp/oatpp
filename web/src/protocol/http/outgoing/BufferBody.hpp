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

#ifndef oatpp_web_protocol_http_outgoing_BufferBody_hpp
#define oatpp_web_protocol_http_outgoing_BufferBody_hpp

#include "../Http.hpp"
#include "./Body.hpp"

#include "../../../../../../oatpp/core/src/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class BufferBody : public oatpp::base::Controllable, public Body {
public:
  OBJECT_POOL(Http_Outgoing_BufferBody_Pool, BufferBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_BufferBody_Pool, BufferBody, 32)
private:
  oatpp::base::String::PtrWrapper m_buffer;
public:
  BufferBody(const oatpp::base::String::PtrWrapper& buffer)
    : m_buffer(buffer)
  {}
public:
  
  static std::shared_ptr<BufferBody> createShared(const oatpp::base::String::PtrWrapper& buffer) {
    return Shared_Http_Outgoing_BufferBody_Pool::allocateShared(buffer);
  }
  
  void declareHeaders(const std::shared_ptr<Headers>& headers) override {
    headers->put(oatpp::web::protocol::http::Header::CONTENT_LENGTH,
                 oatpp::utils::conversion::int32ToStr(m_buffer->getSize()));
  }
  
  void writeToStream(const std::shared_ptr<OutputStream>& stream) override {
    stream->write(m_buffer);
  }
  
public:
  
  class WriteToStreamCoroutine : public oatpp::async::Coroutine<WriteToStreamCoroutine> {
  private:
    std::shared_ptr<BufferBody> m_body;
    std::shared_ptr<OutputStream> m_stream;
  public:
    
    WriteToStreamCoroutine(const std::shared_ptr<BufferBody>& body,
                           const std::shared_ptr<OutputStream>& stream)
      : m_body(body)
      , m_stream(stream)
    {}
    
    Action act() override {
      auto readCount = m_stream->write(m_body->m_buffer);
      if(readCount > 0) {
        return finish();
      } else if(readCount == oatpp::data::stream::IOStream::ERROR_TRY_AGAIN){
        return waitRetry();
      }
      return abort();
    }
    
  };
  
public:
  
  Action writeToStreamAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                             const Action& actionOnReturn,
                             const std::shared_ptr<OutputStream>& stream) override {
    return parentCoroutine->startCoroutine<WriteToStreamCoroutine>(actionOnReturn, getSharedPtr<BufferBody>(), stream);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_BufferBody_hpp */
