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
#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

/**
 * Abstract BodyDecoder.
 * Body Decoder is responsible to decode body of incoming request.
 * For example if you receive chunked-encoded body, you have to decode it in order to get "plain" body.
 * You may extend this class in order to customize body-decoding process.
 */
class BodyDecoder {
private:
  
  template<class Wrapper>
  class ToDtoDecoder : public oatpp::async::CoroutineWithResult<ToDtoDecoder<Wrapper>, const Wrapper&> {
  private:
    const BodyDecoder* m_decoder;
    Headers m_headers;
    std::shared_ptr<data::stream::InputStream> m_bodyStream;
    std::shared_ptr<data::stream::IOStream> m_connection;
    std::shared_ptr<data::mapping::ObjectMapper> m_objectMapper;
    std::shared_ptr<data::stream::BufferOutputStream> m_outputStream;
  public:
    
    ToDtoDecoder(const BodyDecoder* decoder,
                 Headers& headers,
                 const std::shared_ptr<data::stream::InputStream>& bodyStream,
                 const std::shared_ptr<data::stream::IOStream>& connection,
                 const std::shared_ptr<data::mapping::ObjectMapper>& objectMapper)
      : m_decoder(decoder)
      , m_headers(headers)
      , m_bodyStream(bodyStream)
      , m_connection(connection)
      , m_objectMapper(objectMapper)
      , m_outputStream(std::make_shared<data::stream::BufferOutputStream>())
    {}
    
    oatpp::async::Action act() override {
      return m_decoder->decodeAsync(m_headers, m_bodyStream, m_outputStream, m_connection)
        .next(this->yieldTo(&ToDtoDecoder::onDecoded));
    }
    
    oatpp::async::Action onDecoded() {
      auto body = m_outputStream->toString();
      oatpp::utils::parser::Caret caret(body);
      auto dto = m_objectMapper->readFromCaret<Wrapper>(caret);
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
   * @param bodyStream - pointer to &id:oatpp::data::stream::InputStream;.
   * @param writeCallback - &id:oatpp::data::stream::WriteCallback;.
   * @param connection
   */
  virtual void decode(const Headers& headers,
                      data::stream::InputStream* bodyStream,
                      data::stream::WriteCallback* writeCallback,
                      data::stream::IOStream* connection) const = 0;

  /**
   * Implement this method! Same as &l:BodyDecoder::decode (); but Async.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param writeCallback - `std::shared_ptr` to &id:oatpp::data::stream::WriteCallback;.
   * @param connection
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  virtual oatpp::async::CoroutineStarter decodeAsync(const Headers& headers,
                                                     const std::shared_ptr<data::stream::InputStream>& bodyStream,
                                                     const std::shared_ptr<data::stream::WriteCallback>& writeCallback,
                                                     const std::shared_ptr<data::stream::IOStream>& connection) const = 0;

  /**
   * Read body stream and decode it to string.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - pointer to &id:oatpp::data::stream::InputStream;.
   * @param connection
   * @return - &oatpp::String;.
   */
  oatpp::String decodeToString(const Headers& headers,
                               data::stream::InputStream* bodyStream,
                               data::stream::IOStream* connection) const
  {
    oatpp::data::stream::BufferOutputStream stream;
    decode(headers, bodyStream, &stream, connection);
    return stream.toString();
  }

  /**
   * Read body stream, decode, and deserialize it as DTO Object (see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/)).
   * @tparam Wrapper - ObjectWrapper type.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - pointer to &id:oatpp::data::stream::InputStream;.
   * @param connection
   * @param objectMapper - pointer to &id:oatpp::data::mapping::ObjectMapper;.
   * @return - deserialized DTO object.
   */
  template<class Wrapper>
  Wrapper decodeToDto(const Headers& headers,
                      data::stream::InputStream* bodyStream,
                      data::stream::IOStream* connection,
                      data::mapping::ObjectMapper* objectMapper) const
  {
    return objectMapper->readFromString<Wrapper>(decodeToString(headers, bodyStream, connection));
  }

  /**
   * Same as &l:BodyDecoder::decodeToString (); but Async.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param connection
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const oatpp::String&>
  decodeToStringAsync(const Headers& headers,
                      const std::shared_ptr<data::stream::InputStream>& bodyStream,
                      const std::shared_ptr<data::stream::IOStream>& connection) const;

  /**
   * Same as &l:BodyDecoder::decodeToDto (); but Async.
   * @tparam Wrapper - ObjectWrapper type.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param connection
   * @param objectMapper - `std::shared_ptr` to &id:oatpp::data::mapping::ObjectMapper;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  template<class Wrapper>
  oatpp::async::CoroutineStarterForResult<const Wrapper&>
  decodeToDtoAsync(const Headers& headers,
                   const std::shared_ptr<data::stream::InputStream>& bodyStream,
                   const std::shared_ptr<data::stream::IOStream>& connection,
                   const std::shared_ptr<data::mapping::ObjectMapper>& objectMapper) const
  {
    return ToDtoDecoder<Wrapper>::startForResult(this, headers, bodyStream, connection, objectMapper);
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_BodyDecoder_hpp */
