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

#include "./Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class BufferBody : public oatpp::base::Controllable, public Body {
public:
  OBJECT_POOL(Http_Outgoing_BufferBody_Pool, BufferBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_BufferBody_Pool, BufferBody, 32)
private:
  oatpp::String m_buffer;
public:
  BufferBody(const oatpp::String& buffer)
    : m_buffer(buffer)
  {}
public:
  
  static std::shared_ptr<BufferBody> createShared(const oatpp::String& buffer) {
    return Shared_Http_Outgoing_BufferBody_Pool::allocateShared(buffer);
  }
  
  void declareHeaders(const std::shared_ptr<Headers>& headers) override {
    headers->put(oatpp::web::protocol::http::Header::CONTENT_LENGTH,
                 oatpp::utils::conversion::int32ToStr(m_buffer->getSize()));
  }
  
  void writeToStream(const std::shared_ptr<OutputStream>& stream) override {
    oatpp::os::io::Library::v_size progress = 0;
    while (progress < m_buffer->getSize()) {
      auto res = stream->write(&m_buffer->getData()[progress], m_buffer->getSize() - progress);
      if(res < 0) {
        if(res == oatpp::data::stream::IOStream::ERROR_IO_PIPE ||
           (res != oatpp::data::stream::IOStream::ERROR_IO_RETRY && res != oatpp::data::stream::IOStream::ERROR_IO_WAIT_RETRY)) {
          return;
        }
      }
      progress += res;
    }
  }
  
public:
  
  class WriteToStreamCoroutine : public oatpp::async::Coroutine<WriteToStreamCoroutine> {
  private:
    std::shared_ptr<BufferBody> m_body;
    std::shared_ptr<OutputStream> m_stream;
    const void* m_currData;
    oatpp::os::io::Library::v_size m_currDataSize;
  public:
    
    WriteToStreamCoroutine(const std::shared_ptr<BufferBody>& body,
                           const std::shared_ptr<OutputStream>& stream)
      : m_body(body)
      , m_stream(stream)
      , m_currData(m_body->m_buffer->getData())
      , m_currDataSize(m_body->m_buffer->getSize())
    {}
    
    Action act() override {
      return oatpp::data::stream::IOStream::writeDataAsyncInline(m_stream.get(), m_currData, m_currDataSize, finish());
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
