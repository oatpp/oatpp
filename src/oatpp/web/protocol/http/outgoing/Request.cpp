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

#include "Request.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

Request::Request(const oatpp::data::share::StringKeyLabel& method,
                 const oatpp::data::share::StringKeyLabel& path,
                 const Headers& headers,
                 const std::shared_ptr<Body>& body)
  : m_method(method)
  , m_path(path)
  , m_headers(headers)
  , m_body(body)
{}

std::shared_ptr<Request> Request::createShared(const oatpp::data::share::StringKeyLabel& method,
                                               const oatpp::data::share::StringKeyLabel& path,
                                               const Headers& headers,
                                               const std::shared_ptr<Body>& body) {
  return Shared_Outgoing_Request_Pool::allocateShared(method, path, headers, body);
}

const oatpp::data::share::StringKeyLabel& Request::getMethod() const {
  return m_method;
}

const oatpp::data::share::StringKeyLabel& Request::getPath() const {
  return m_path;
}

protocol::http::Headers& Request::getHeaders() {
  return m_headers;
}

void Request::putHeader(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  m_headers[key] = value;
}

bool Request::putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  auto it = m_headers.find(key);
  if(it == m_headers.end()) {
    m_headers.insert({key, value});
    return true;
  }
  return false;
}

std::shared_ptr<Body> Request::getBody() {
  return m_body;
}

void Request::send(data::stream::OutputStream* stream){
  
  if(m_body){
    m_body->declareHeaders(m_headers);
  } else {
    m_headers[Header::CONTENT_LENGTH] = "0";
  }
  
  stream->write(m_method.getData(), m_method.getSize());
  stream->write(" /", 2);
  stream->write(m_path.getData(), m_path.getSize());
  stream->write(" ", 1);
  stream->write("HTTP/1.1", 8);
  stream->write("\r\n", 2);
  
  auto it = m_headers.begin();
  while(it != m_headers.end()){
    stream->write(it->first.getData(), it->first.getSize());
    stream->write(": ", 2);
    stream->write(it->second.getData(), it->second.getSize());
    stream->write("\r\n", 2);
    it ++;
  }
  
  stream->write("\r\n", 2);
  if(m_body) {
    m_body->writeToStream(stream);
  }
  
}

oatpp::async::CoroutineStarter Request::sendAsync(const std::shared_ptr<data::stream::OutputStream>& stream){
  
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
      
      if(m_request->m_body){
        m_request->m_body->declareHeaders(m_request->m_headers);
      } else {
        m_request->m_headers[Header::CONTENT_LENGTH] = "0";
      }
      
      m_buffer->write(m_request->m_method.getData(), m_request->m_method.getSize());
      m_buffer->write(" /", 2);
      m_buffer->write(m_request->m_path.getData(), m_request->m_path.getSize());
      m_buffer->write(" ", 1);
      m_buffer->write("HTTP/1.1", 8);
      m_buffer->write("\r\n", 2);

      http::Utils::writeHeaders(m_request->m_headers, m_buffer.get());
      
      m_buffer->write("\r\n", 2);
      
      return yieldTo(&SendAsyncCoroutine::writeHeaders);
      
    }
    
    Action writeHeaders() {
      return m_buffer->flushToStreamAsync(m_stream).next(yieldTo(&SendAsyncCoroutine::writeBody));
    }
    
    Action writeBody() {
      if(m_request->m_body) {
        return m_request->m_body->writeToStreamAsync(m_stream).next(finish());
      }
      return finish();
    }
    
  };
  
  return SendAsyncCoroutine::start(shared_from_this(), stream);
  
}
  
}}}}}
