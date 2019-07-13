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

  class ToStringDecoder : public oatpp::async::CoroutineWithResult<ToStringDecoder, const oatpp::String&> {
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
  class ToDtoDecoder : public oatpp::async::CoroutineWithResult<ToDtoDecoder<Type>, const typename Type::ObjectWrapper&> {
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
      return m_decoder->decodeAsync(m_headers, m_bodyStream, m_chunkedBuffer).next(this->yieldTo(&ToDtoDecoder::onDecoded));
    }
    
    oatpp::async::Action onDecoded() {
      auto body = m_chunkedBuffer->toString();
      oatpp::parser::Caret caret(body);
      auto dto = m_objectMapper->readFromCaret<Type>(caret);
      if(caret.hasError()) {
        return this->template error<oatpp::async::Error>(caret.getErrorMessage());
      }
      return this->_return(dto);
    }
    
  };
  
public:

  /**
   * Virtual destructor.
   */
  virtual ~BodyDecoder() = default;

  /**
   * Implement this method! Decode bodyStream and write decoded data to toStream.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param writeCallback - &id:oatpp::data::stream::WriteCallback;.
   */
  virtual void decode(const Headers& headers,
                      const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                      oatpp::data::stream::WriteCallback* writeCallback) const = 0;

  /**
   * Decode using &id:oatpp::data::stream::DefaultWriteCallback;.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param toStream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  void decode(const Headers& headers,
              const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
              const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const;

  /**
   * Implement this method! Same as &l:BodyDecoder::decode (); but Async.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param writeCallback - `std::shared_ptr` to &id:oatpp::data::stream::AsyncWriteCallback;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  virtual oatpp::async::CoroutineStarter decodeAsync(const Headers& headers,
                                                     const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                                     const std::shared_ptr<oatpp::data::stream::AsyncWriteCallback>& writeCallback) const = 0;

  /**
   * Decode in asynchronous manner using &id:oatpp::data::stream::DefaultAsyncWriteCallback;.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param toStream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter decodeAsync(const Headers& headers,
                                             const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                             const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const;

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
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const oatpp::String&>
  decodeToStringAsync(const Headers& headers, const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream) const {
    return ToStringDecoder::startForResult(this, headers, bodyStream);
  }

  /**
   * Same as &l:BodyDecoder::decodeToDto (); but Async.
   * @tparam DtoType - DTO object type.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param objectMapper - `std::shared_ptr` to &id:oatpp::data::mapping::ObjectMapper;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  template<class DtoType>
  oatpp::async::CoroutineStarterForResult<const typename DtoType::ObjectWrapper&>
  decodeToDtoAsync(const Headers& headers,
                   const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                   const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return ToDtoDecoder<DtoType>::startForResult(this, headers, bodyStream, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_BodyDecoder_hpp */
