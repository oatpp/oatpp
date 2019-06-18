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

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

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
  m_headers[key] = value;
}

bool Response::putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value) {
  auto it = m_headers.find(key);
  if(it == m_headers.end()) {
    m_headers.insert({key, value});
    return true;
  }
  return false;
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

void Response::send(const std::shared_ptr<data::stream::OutputStream>& stream) {
  
  if(m_body){
    m_body->declareHeaders(m_headers);
  } else {
    m_headers[Header::CONTENT_LENGTH] = "0";
  }

  oatpp::data::stream::ChunkedBuffer buffer;

  buffer.write("HTTP/1.1 ", 9);
  buffer.writeAsString(m_status.code);
  buffer.write(" ", 1);
  buffer.OutputStream::write(m_status.description);
  buffer.write("\r\n", 2);
  
  auto it = m_headers.begin();
  while(it != m_headers.end()) {
    buffer.write(it->first.getData(), it->first.getSize());
    buffer.write(": ", 2);
    buffer.write(it->second.getData(), it->second.getSize());
    buffer.write("\r\n", 2);
    it ++;
  }

  buffer.write("\r\n", 2);
  buffer.flushToStream(stream);
  if(m_body) {
    m_body->writeToStream(stream);
  }
  
}

oatpp::async::CoroutineStarter Response::sendAsync(const std::shared_ptr<data::stream::OutputStream>& stream){
  
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
    
    Action act() override {
    
      if(m_response->m_body){
        m_response->m_body->declareHeaders(m_response->m_headers);
      } else {
        m_response->m_headers[Header::CONTENT_LENGTH] = "0";
      }
      
      m_buffer->write("HTTP/1.1 ", 9);
      m_buffer->writeAsString(m_response->m_status.code);
      m_buffer->write(" ", 1);
      m_buffer->OutputStream::write(m_response->m_status.description);
      m_buffer->write("\r\n", 2);
      
      auto it = m_response->m_headers.begin();
      while(it != m_response->m_headers.end()) {
        m_buffer->write(it->first.getData(), it->first.getSize());
        m_buffer->write(": ", 2);
        m_buffer->write(it->second.getData(), it->second.getSize());
        m_buffer->write("\r\n", 2);
        it ++;
      }
      
      m_buffer->write("\r\n", 2);
      
      return yieldTo(&SendAsyncCoroutine::writeHeaders);
    
    }
    
    Action writeHeaders() {
      return m_buffer->flushToStreamAsync(m_stream).next(yieldTo(&SendAsyncCoroutine::writeBody));
    }
    
    Action writeBody() {
      if(m_response->m_body) {
        return m_response->m_body->writeToStreamAsync(m_stream).next(finish());
      }
      return finish();
    }
    
  };
  
  return SendAsyncCoroutine::start(shared_from_this(), stream);
  
}
  
}}}}}
