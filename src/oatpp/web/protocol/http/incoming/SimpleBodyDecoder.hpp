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

#ifndef oatpp_web_protocol_http_incoming_SimpleBodyDecoder_hpp
#define oatpp_web_protocol_http_incoming_SimpleBodyDecoder_hpp

#include "BodyDecoder.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

/**
 * Default implementation of &id:oatpp::web::protocol::http::incoming::BodyDecoder;.
 */
class SimpleBodyDecoder : public BodyDecoder {
private:
  static data::v_io_size readLine(const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                          p_char8 buffer,
                                          data::v_io_size maxLineSize);
  static void doChunkedDecoding(const std::shared_ptr<oatpp::data::stream::InputStream>& from,
                                const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream);
  
  static oatpp::async::Action doChunkedDecodingAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                                     oatpp::async::Action&& actionOnReturn,
                                                     const std::shared_ptr<oatpp::data::stream::InputStream>& fromStream,
                                                     const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream);
public:

  /**
   * Decode bodyStream and write decoded data to toStream.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param toStream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  void decode(const Headers& headers,
              const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
              const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const override;

  /**
   * Same as &l:SimpleBodyDecoder::decode (); but Async.
   * @param parentCoroutine - caller coroutine as &id:oatpp::async::AbstractCoroutine;*.
   * @param actionOnReturn - &id:oatpp::async::Action; to do when decoding finished.
   * @param headers - Headers map. &id:oatpp::web::protocol::http::Headers;.
   * @param bodyStream - `std::shared_ptr` to &id:oatpp::data::stream::InputStream;.
   * @param toStream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::Action;.
   */
  oatpp::async::Action decodeAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   oatpp::async::Action&& actionOnReturn,
                                   const Headers& headers,
                                   const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                   const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const override;
  
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_SimpleBodyDecoder_hpp */
