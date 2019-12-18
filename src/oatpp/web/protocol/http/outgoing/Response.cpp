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

#include "./Response.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

Response::Response(const Status& status,
                   const std::shared_ptr<Body>& body)
  : m_status(status)
  , m_body(body)
{}

std::shared_ptr<Response> Response::createShared(const Status& status,
                                                 const std::shared_ptr<Body>& body) {
  return Shared_Outgoing_Response_Pool::allocateShared(status, body);
}

const Status& Response::getStatus() const {
  return m_status;
}

protocol::http::Headers& Response::getHeaders() {
  return m_headers;
}

void Response::putHeader(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  m_headers.put(key, value);
}

bool Response::putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  return m_headers.putIfNotExists(key, value);
}

oatpp::String Response::getHeader(const oatpp::data::share::StringKeyLabelCI_FAST& headerName) const {
  return m_headers.get(headerName);
}

void Response::setConnectionUpgradeHandler(const std::shared_ptr<oatpp::network::server::ConnectionHandler>& handler) {
  m_connectionUpgradeHandler = handler;
}

std::shared_ptr<oatpp::network::server::ConnectionHandler> Response::getConnectionUpgradeHandler() {
  return m_connectionUpgradeHandler;
}

void Response::setConnectionUpgradeParameters(const std::shared_ptr<const ConnectionHandler::ParameterMap>& parameters) {
  m_connectionUpgradeParameters = parameters;
}

std::shared_ptr<const Response::ConnectionHandler::ParameterMap> Response::getConnectionUpgradeParameters() {
  return m_connectionUpgradeParameters;
}

void Response::send(data::stream::OutputStream* stream, oatpp::data::stream::BufferOutputStream* headersWriteBuffer) {

  if(m_body){
    m_body->declareHeaders(m_headers);
  } else {
    m_headers.put_LockFree(Header::CONTENT_LENGTH, "0");
  }

  headersWriteBuffer->setCurrentPosition(0);

  headersWriteBuffer->write("HTTP/1.1 ", 9);
  headersWriteBuffer->writeAsString(m_status.code);
  headersWriteBuffer->write(" ", 1);
  headersWriteBuffer->OutputStream::write(m_status.description);
  headersWriteBuffer->write("\r\n", 2);

  http::Utils::writeHeaders(m_headers, headersWriteBuffer);

  headersWriteBuffer->write("\r\n", 2);

  if(m_body) {

    auto bodySize = m_body->getKnownSize();

    if(bodySize >= 0 && bodySize + headersWriteBuffer->getCurrentPosition() < headersWriteBuffer->getCapacity()) {
      m_body->writeToStream(headersWriteBuffer);
      headersWriteBuffer->flushToStream(stream);
    } else {
      headersWriteBuffer->flushToStream(stream);
      m_body->writeToStream(stream);
    }

  } else {
    headersWriteBuffer->flushToStream(stream);
  }
  
}

oatpp::async::CoroutineStarter Response::sendAsync(const std::shared_ptr<Response>& _this,
                                                   const std::shared_ptr<data::stream::OutputStream>& stream,
                                                   const std::shared_ptr<oatpp::data::stream::BufferOutputStream>& headersWriteBuffer)
{
  
  class SendAsyncCoroutine : public oatpp::async::Coroutine<SendAsyncCoroutine> {
  private:
    std::shared_ptr<Response> m_this;
    std::shared_ptr<data::stream::OutputStream> m_stream;
    std::shared_ptr<oatpp::data::stream::BufferOutputStream> m_headersWriteBuffer;
  public:
    
    SendAsyncCoroutine(const std::shared_ptr<Response>& _this,
                       const std::shared_ptr<data::stream::OutputStream>& stream,
                       const std::shared_ptr<oatpp::data::stream::BufferOutputStream>& headersWriteBuffer)
      : m_this(_this)
      , m_stream(stream)
      , m_headersWriteBuffer(headersWriteBuffer)
    {}
    
    Action act() override {
    
      if(m_this->m_body){
        m_this->m_body->declareHeaders(m_this->m_headers);
      } else {
        m_this->m_headers.put_LockFree(Header::CONTENT_LENGTH, "0");
      }

      m_headersWriteBuffer->setCurrentPosition(0);

      m_headersWriteBuffer->write("HTTP/1.1 ", 9);
      m_headersWriteBuffer->writeAsString(m_this->m_status.code);
      m_headersWriteBuffer->write(" ", 1);
      m_headersWriteBuffer->OutputStream::write(m_this->m_status.description);
      m_headersWriteBuffer->write("\r\n", 2);

      http::Utils::writeHeaders(m_this->m_headers, m_headersWriteBuffer.get());
      
      m_headersWriteBuffer->write("\r\n", 2);

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
  
  return SendAsyncCoroutine::start(_this, stream, headersWriteBuffer);
  
}
  
}}}}}
