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

#include "../../../../../core/src/data/stream/ChunkedBuffer.hpp"

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
  
oatpp::async::Action Response::sendAsync(const std::shared_ptr<data::stream::OutputStream>& stream){
  
  auto buffer = oatpp::data::stream::ChunkedBuffer::createShared();
  
  auto _this = this->getSharedPtr<Response>();
  
  return oatpp::async::Routine::_do({
    
    [_this, stream, buffer] {
      
      if(_this->body){
        _this->body->declareHeaders(_this->headers);
      } else {
        _this->headers->put(Header::CONTENT_LENGTH, "0");
      }
      
      buffer->write("HTTP/1.1 ", 9);
      buffer->writeAsString(_this->status.code);
      buffer->write(" ", 1);
      buffer->OutputStream::write(_this->status.description);
      buffer->write("\r\n", 2);
      
      auto curr = _this->headers->getFirstEntry();
      while(curr != nullptr){
        buffer->write(curr->getKey()->getData(), curr->getKey()->getSize());
        buffer->write(": ", 2);
        buffer->write(curr->getValue()->getData(), curr->getValue()->getSize());
        buffer->write("\r\n", 2);
        curr = curr->getNext();
      }
      
      buffer->write("\r\n", 2);
      
      return oatpp::async::Action::_continue();
      
    }, nullptr
    
  })._then({
    
    [stream, buffer] {
      return buffer->flushToStreamAsync(stream);
    }, nullptr
    
  })._then({
    
    [_this, stream] {
      if(_this->body) {
        return _this->body->writeToStreamAsync(stream);
      } else {
        return oatpp::async::Action::_return();
      }
    }, nullptr
    
  });
  
}
  
}}}}}
