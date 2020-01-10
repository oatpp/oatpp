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
#include "oatpp/core/data/stream/BufferStream.hpp"

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
  m_headers.put(key, value);
}

bool Request::putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  return m_headers.putIfNotExists(key, value);
}

std::shared_ptr<Body> Request::getBody() {
  return m_body;
}

void Request::send(data::stream::OutputStream* stream){
  
  if(m_body){
    m_body->declareHeaders(m_headers);
  } else {
    m_headers.put_LockFree(Header::CONTENT_LENGTH, "0");
  }

  oatpp::data::stream::BufferOutputStream buffer;

  buffer.writeSimple(m_method.getData(), m_method.getSize());
  buffer.writeSimple(" /", 2);
  buffer.writeSimple(m_path.getData(), m_path.getSize());
  buffer.writeSimple(" ", 1);
  buffer.writeSimple("HTTP/1.1", 8);
  buffer.writeSimple("\r\n", 2);

  http::Utils::writeHeaders(m_headers, &buffer);

  buffer.writeSimple("\r\n", 2);

  if(m_body) {

    auto bodySize = m_body->getKnownSize();

    if(bodySize >= 0 && bodySize + buffer.getCurrentPosition() < buffer.getCapacity()) {
      m_body->writeToStream(&buffer);
      buffer.flushToStream(stream);
    } else {
      buffer.flushToStream(stream);
      m_body->writeToStream(stream);
    }

  } else {
    buffer.flushToStream(stream);
  }
  
}

oatpp::async::CoroutineStarter Request::sendAsync(std::shared_ptr<Request> _this,
                                                  const std::shared_ptr<data::stream::OutputStream>& stream)
{
  
  class SendAsyncCoroutine : public oatpp::async::Coroutine<SendAsyncCoroutine> {
  private:
    std::shared_ptr<Request> m_this;
    std::shared_ptr<data::stream::OutputStream> m_stream;
    std::shared_ptr<oatpp::data::stream::BufferOutputStream> m_headersWriteBuffer;
  public:
    
    SendAsyncCoroutine(const std::shared_ptr<Request>& request,
                       const std::shared_ptr<data::stream::OutputStream>& stream)
      : m_this(request)
      , m_stream(stream)
      , m_headersWriteBuffer(std::make_shared<oatpp::data::stream::BufferOutputStream>())
    {}
    
    Action act() {
      
      if(m_this->m_body){
        m_this->m_body->declareHeaders(m_this->m_headers);
      } else {
        m_this->m_headers.put_LockFree(Header::CONTENT_LENGTH, "0");
      }
      
      m_headersWriteBuffer->writeSimple(m_this->m_method.getData(), m_this->m_method.getSize());
      m_headersWriteBuffer->writeSimple(" /", 2);
      m_headersWriteBuffer->writeSimple(m_this->m_path.getData(), m_this->m_path.getSize());
      m_headersWriteBuffer->writeSimple(" ", 1);
      m_headersWriteBuffer->writeSimple("HTTP/1.1", 8);
      m_headersWriteBuffer->writeSimple("\r\n", 2);

      http::Utils::writeHeaders(m_this->m_headers, m_headersWriteBuffer.get());
      
      m_headersWriteBuffer->writeSimple("\r\n", 2);

      const auto& body = m_this->m_body;

      if(body) {

        auto bodySize = body->getKnownSize();

        if(bodySize >= 0 && bodySize + m_headersWriteBuffer->getCurrentPosition() < m_headersWriteBuffer->getCapacity()) {

          return body->writeToStreamAsync(m_headersWriteBuffer)
            .next(oatpp::data::stream::BufferOutputStream::flushToStreamAsync(m_headersWriteBuffer, m_stream))
            .next(finish());

        } else {
          return yieldTo(&SendAsyncCoroutine::writeHeaders);
        }

      } else {
        return yieldTo(&SendAsyncCoroutine::writeHeaders);
      }
      
    }
    
    Action writeHeaders() {
      return oatpp::data::stream::BufferOutputStream::flushToStreamAsync(m_headersWriteBuffer, m_stream).next(yieldTo(&SendAsyncCoroutine::writeBody));
    }
    
    Action writeBody() {
      if(m_this->m_body) {
        return m_this->m_body->writeToStreamAsync(m_stream).next(finish());
      }
      return finish();
    }
    
  };
  
  return SendAsyncCoroutine::start(_this, stream);
  
}
  
}}}}}
