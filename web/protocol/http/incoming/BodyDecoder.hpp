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

#ifndef oatpp_web_protocol_http_incoming_BodyDecoder_hpp
#define oatpp_web_protocol_http_incoming_BodyDecoder_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {
  
class BodyDecoder {
private:
  
  class ToStringDecoder : public oatpp::async::CoroutineWithResult<ToStringDecoder, oatpp::String> {
  private:
    std::shared_ptr<Protocol::Headers> m_headers;
    std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_chunkedBuffer = oatpp::data::stream::ChunkedBuffer::createShared();
  public:
    
    ToStringDecoder(const std::shared_ptr<Protocol::Headers>& headers,
                    const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream)
      : m_headers(headers)
      , m_bodyStream(bodyStream)
    {}
    
    Action act() override {
      return decodeAsync(this, yieldTo(&ToStringDecoder::onDecoded), m_headers, m_bodyStream, m_chunkedBuffer);
    }
    
    Action onDecoded() {
      return _return(m_chunkedBuffer->toString());
    }
    
  };
  
  template<class Type>
  class ToDtoDecoder : public oatpp::async::CoroutineWithResult<ToDtoDecoder<Type>, typename Type::ObjectWrapper> {
  private:
    std::shared_ptr<Protocol::Headers> m_headers;
    std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_chunkedBuffer = oatpp::data::stream::ChunkedBuffer::createShared();
  public:
    
    ToDtoDecoder(const std::shared_ptr<Protocol::Headers>& headers,
                 const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                 const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
      : m_headers(headers)
      , m_bodyStream(bodyStream)
      , m_objectMapper(objectMapper)
    {}
    
    oatpp::async::Action act() override {
      return decodeAsync(this, this->yieldTo(&ToDtoDecoder::onDecoded), m_headers, m_bodyStream, m_chunkedBuffer);
    }
    
    oatpp::async::Action onDecoded() {
      auto body = m_chunkedBuffer->toString();
      oatpp::parser::ParsingCaret caret(body);
      auto dto = m_objectMapper->readFromCaret<Type>(caret);
      if(caret.hasError()) {
        return this->error(caret.getError());
      }
      return this->_return(dto);
    }
    
  };
  
private:
  static os::io::Library::v_size readLine(const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                          p_char8 buffer,
                                          os::io::Library::v_size maxLineSize);
  static void doChunkedDecoding(const std::shared_ptr<oatpp::data::stream::InputStream>& from,
                                const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream);
  
  static oatpp::async::Action doChunkedDecodingAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                     const oatpp::async::Action& actionOnReturn,
                                                     const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                                     const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream);
public:
  
  // read chunk by chunk from 'bodyStream' and write to 'toStream' using buffer::IOBuffer
  
  static void decode(const std::shared_ptr<Protocol::Headers>& headers,
                     const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                     const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream);
  
  static oatpp::String
  decodeToString(const std::shared_ptr<Protocol::Headers>& headers,
                 const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) {
    auto chunkedBuffer = oatpp::data::stream::ChunkedBuffer::createShared();
    decode(headers, bodyStream, chunkedBuffer);
    return chunkedBuffer->toString();
  }
  
  template<class Type>
  static typename Type::ObjectWrapper decodeToDto(const std::shared_ptr<Protocol::Headers>& headers,
                                               const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                               const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper){
    return objectMapper->readFromString<Type>(decodeToString(headers, bodyStream));
  }
  
  // Async
  
  static oatpp::async::Action decodeAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                          const oatpp::async::Action& actionOnReturn,
                                          const std::shared_ptr<Protocol::Headers>& headers,
                                          const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                          const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream);
  
  template<typename ParentCoroutineType>
  static oatpp::async::Action
  decodeToStringAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                      oatpp::async::Action (ParentCoroutineType::*callback)(const oatpp::String&),
                      const std::shared_ptr<Protocol::Headers>& headers,
                      const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) {
    return parentCoroutine->startCoroutineForResult<ToStringDecoder>(callback, headers, bodyStream);
  }
  
  template<class DtoType, typename ParentCoroutineType>
  static oatpp::async::Action
  decodeToDtoAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                   oatpp::async::Action (ParentCoroutineType::*callback)(const typename DtoType::ObjectWrapper&),
                   const std::shared_ptr<Protocol::Headers>& headers,
                   const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                   const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) {
    return parentCoroutine->startCoroutineForResult<ToDtoDecoder<DtoType>>(callback, headers, bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_BodyDecoder_hpp */
