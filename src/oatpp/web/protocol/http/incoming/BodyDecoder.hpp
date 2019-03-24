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

#ifndef oatpp_web_protocol_http_incoming_BodyDecoder_hpp
#define oatpp_web_protocol_http_incoming_BodyDecoder_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

/**
 * Abstract BodyDecoder.
 * Body Decoder is responsible to decode body of incoming request.
 * For example if you receive chunked-encoded body, you have to decode it in order to get "plain" body.
 * You may extend this class in order to customize body-decoding process.
 */
class BodyDecoder {
private:

  class ToStringDecoder : public oatpp::async::CoroutineWithResult<ToStringDecoder, oatpp::String> {
  private:
    const BodyDecoder* m_decoder;
    Headers m_headers;
    std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_chunkedBuffer;
  public:
    
    ToStringDecoder(const BodyDecoder* decoder,
                    const Headers& headers,
                    const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream);
    
    Action act() override;
    Action onDecoded();
    
  };
  
  template<class Type>
  class ToDtoDecoder : public oatpp::async::CoroutineWithResult<ToDtoDecoder<Type>, typename Type::ObjectWrapper> {
  private:
    const BodyDecoder* m_decoder;
    Headers m_headers;
    std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_chunkedBuffer = oatpp::data::stream::ChunkedBuffer::createShared();
  public:
    
    ToDtoDecoder(const BodyDecoder* decoder,
                 Headers& headers,
                 const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                 const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
      : m_decoder(decoder)
      , m_headers(headers)
      , m_bodyStream(bodyStream)
      , m_objectMapper(objectMapper)
    {}
    
    oatpp::async::Action act() override {
      return m_decoder->decodeAsync(this, this->yieldTo(&ToDtoDecoder::onDecoded), m_headers, m_bodyStream, m_chunkedBuffer);
    }
    
    oatpp::async::Action onDecoded() {
      auto body = m_chunkedBuffer->toString();
      oatpp::parser::Caret caret(body);
      auto dto = m_objectMapper->readFromCaret<Type>(caret);
      if(caret.hasError()) {
        return this->error(caret.getErrorMessage());
      }
      return this->_return(dto);
    }
    
  };
  
public:

  /**
   * Implement this method! Decode bodyStream and write decoded data to toStream.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param toStream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  virtual void decode(const Headers& headers,
                      const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                      const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const = 0;

  /**
   * Implement this method! Same as &l:BodyDecoder::decode (); but Async.
   * @param parentCoroutine - caller coroutine as &id:oatpp::async::AbstractCoroutine;*.
   * @param actionOnReturn - &id:oatpp::async::Action; to do when decoding finished.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param toStream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::Action;.
   */
  virtual oatpp::async::Action decodeAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                           const oatpp::async::Action& actionOnReturn,
                                           const Headers& headers,
                                           const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                           const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const = 0;

  /**
   * Read body stream and decode it to string.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @return - &oatpp::String;.
   */
  oatpp::String decodeToString(const Headers& headers,
                               const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) const {
    auto chunkedBuffer = oatpp::data::stream::ChunkedBuffer::createShared();
    decode(headers, bodyStream, chunkedBuffer);
    return chunkedBuffer->toString();
  }

  /**
   * Read body stream, decode, and deserialize it as DTO Object (see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/)).
   * @tparam Type - DTO object type.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param objectMapper - `std::shared_ptr` to &id:oatpp::data::mapping::ObjectMapper;.
   * @return - deserialized DTO object.
   */
  template<class Type>
  typename Type::ObjectWrapper decodeToDto(const Headers& headers,
                                           const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                           const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return objectMapper->readFromString<Type>(decodeToString(headers, bodyStream));
  }

  /**
   * Same as &l:BodyDecoder::decodeToString (); but Async.
   * @tparam ParentCoroutineType - caller coroutine type.
   * @param parentCoroutine - caller coroutine as &id:oatpp::async::AbstractCoroutine;*.
   * @param callback - pointer to callback function.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @return - &id:oatpp::async::Action;.
   */
  template<typename ParentCoroutineType>
  oatpp::async::Action decodeToStringAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                           oatpp::async::Action (ParentCoroutineType::*callback)(const oatpp::String&),
                                           const Headers& headers,
                                           const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) const {
    return parentCoroutine->startCoroutineForResult<ToStringDecoder>(callback, this, headers, bodyStream);
  }

  /**
   * Same as &l:BodyDecoder::decodeToDto (); but Async.
   * @tparam DtoType - DTO object type.
   * @tparam ParentCoroutineType - caller coroutine type.
   * @param parentCoroutine - caller coroutine as &id:oatpp::async::AbstractCoroutine;*.
   * @param callback - pointer to callback function.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param objectMapper - `std::shared_ptr` to &id:oatpp::data::mapping::ObjectMapper;.
   * @return -  &id:oatpp::async::Action;.
   */
  template<class DtoType, typename ParentCoroutineType>
  oatpp::async::Action decodeToDtoAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                        oatpp::async::Action (ParentCoroutineType::*callback)(const typename DtoType::ObjectWrapper&),
                                        const Headers& headers,
                                        const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                        const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return parentCoroutine->startCoroutineForResult<ToDtoDecoder<DtoType>>(callback, this, headers, bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_BodyDecoder_hpp */
