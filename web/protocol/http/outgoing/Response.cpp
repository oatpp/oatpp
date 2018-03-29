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

#include "./Response.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
void Response::send(const std::shared_ptr<data::stream::OutputStream>& stream){
  
  if(body){
    body->declareHeaders(headers);
  } else {
    headers->put(Header::CONTENT_LENGTH, "0");
  }
  
  stream->write("HTTP/1.1 ", 9);
  stream->writeAsString(status.code);
  stream->write(" ", 1);
  stream->OutputStream::write(status.description);
  stream->write("\r\n", 2);
  
  auto curr = headers->getFirstEntry();
  while(curr != nullptr){
    stream->write(curr->getKey()->getData(), curr->getKey()->getSize());
    stream->write(": ", 2);
    stream->write(curr->getValue()->getData(), curr->getValue()->getSize());
    stream->write("\r\n", 2);
    curr = curr->getNext();
  }
  
  stream->write("\r\n", 2);
  if(body) {
    body->writeToStream(stream);
  }
  
}
  
oatpp::async::Action Response::sendAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                          const oatpp::async::Action& actionOnFinish,
                                          const std::shared_ptr<data::stream::OutputStream>& stream){
  
  class SendAsyncCoroutine : public oatpp::async::Coroutine<SendAsyncCoroutine> {
  private:
    std::shared_ptr<Response> m_response;
    std::shared_ptr<data::stream::OutputStream> m_stream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_buffer;
  public:
    
    SendAsyncCoroutine(const std::shared_ptr<Response>& response,
                       const std::shared_ptr<data::stream::OutputStream>& stream)
      : m_response(response)
      , m_stream(stream)
      , m_buffer(oatpp::data::stream::ChunkedBuffer::createShared())
    {}
    
    Action act() {
    
      if(m_response->body){
        m_response->body->declareHeaders(m_response->headers);
      } else {
        m_response->headers->put(Header::CONTENT_LENGTH, "0");
      }
      
      m_buffer->write("HTTP/1.1 ", 9);
      m_buffer->writeAsString(m_response->status.code);
      m_buffer->write(" ", 1);
      m_buffer->OutputStream::write(m_response->status.description);
      m_buffer->write("\r\n", 2);
      
      auto curr = m_response->headers->getFirstEntry();
      while(curr != nullptr){
        m_buffer->write(curr->getKey()->getData(), curr->getKey()->getSize());
        m_buffer->write(": ", 2);
        m_buffer->write(curr->getValue()->getData(), curr->getValue()->getSize());
        m_buffer->write("\r\n", 2);
        curr = curr->getNext();
      }
      
      m_buffer->write("\r\n", 2);
      
      return yieldTo(&SendAsyncCoroutine::writeHeaders);
    
    }
    
    Action writeHeaders() {
      return m_buffer->flushToStreamAsync(this, yieldTo(&SendAsyncCoroutine::writeBody), m_stream);
    }
    
    Action writeBody() {
      if(m_response->body) {
        return m_response->body->writeToStreamAsync(this, finish(), m_stream);
      }
      return finish();
    }
    
  };
  
  return parentCoroutine->startCoroutine<SendAsyncCoroutine>(actionOnFinish, getSharedPtr<Response>(), stream);
  
}
  
}}}}}
