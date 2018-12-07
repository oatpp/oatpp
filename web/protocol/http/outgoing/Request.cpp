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

#include "Request.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
void Request::send(const std::shared_ptr<data::stream::OutputStream>& stream){
  
  if(body){
    body->declareHeaders(headers);
  } else {
    headers[Header::CONTENT_LENGTH] = "0";
  }
  
  stream->write(method.getData(), method.getSize());
  stream->write(" /", 2);
  stream->write(path.getData(), path.getSize());
  stream->write(" ", 1);
  stream->write("HTTP/1.1", 8);
  stream->write("\r\n", 2);
  
  auto it = headers.begin();
  while(it != headers.end()){
    stream->write(it->first.getData(), it->first.getSize());
    stream->write(": ", 2);
    stream->write(it->second.getData(), it->second.getSize());
    stream->write("\r\n", 2);
    it ++;
  }
  
  stream->write("\r\n", 2);
  if(body) {
    body->writeToStream(stream);
  }
  
}
  
oatpp::async::Action Request::sendAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                        const oatpp::async::Action& actionOnFinish,
                                        const std::shared_ptr<data::stream::OutputStream>& stream){
  
  class SendAsyncCoroutine : public oatpp::async::Coroutine<SendAsyncCoroutine> {
  private:
    std::shared_ptr<Request> m_request;
    std::shared_ptr<data::stream::OutputStream> m_stream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_buffer;
  public:
    
    SendAsyncCoroutine(const std::shared_ptr<Request>& request,
                       const std::shared_ptr<data::stream::OutputStream>& stream)
      : m_request(request)
      , m_stream(stream)
      , m_buffer(oatpp::data::stream::ChunkedBuffer::createShared())
    {}
    
    Action act() {
      
      if(m_request->body){
        m_request->body->declareHeaders(m_request->headers);
      } else {
        m_request->headers[Header::CONTENT_LENGTH] = "0";
      }
      
      m_buffer->data::stream::OutputStream::write(m_request->method.getData(), m_request->method.getSize());
      m_buffer->write(" /", 2);
      m_buffer->data::stream::OutputStream::write(m_request->path.getData(), m_request->path.getSize());
      m_buffer->write(" ", 1);
      m_buffer->write("HTTP/1.1", 8);
      m_buffer->write("\r\n", 2);
      
      auto it = m_request->headers.begin();
      while(it != m_request->headers.end()){
        m_buffer->write(it->first.getData(), it->first.getSize());
        m_buffer->write(": ", 2);
        m_buffer->write(it->second.getData(), it->second.getSize());
        m_buffer->write("\r\n", 2);
        it++;
      }
      
      m_buffer->write("\r\n", 2);
      
      return yieldTo(&SendAsyncCoroutine::writeHeaders);
      
    }
    
    Action writeHeaders() {
      return m_buffer->flushToStreamAsync(this, yieldTo(&SendAsyncCoroutine::writeBody), m_stream);
    }
    
    Action writeBody() {
      if(m_request->body) {
        return m_request->body->writeToStreamAsync(this, finish(), m_stream);
      }
      return finish();
    }
    
  };
  
  return parentCoroutine->startCoroutine<SendAsyncCoroutine>(actionOnFinish, getSharedPtr<Request>(), stream);
  
}
  
}}}}}
