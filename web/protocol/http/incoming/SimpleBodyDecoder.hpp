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

#ifndef oatpp_web_protocol_http_incoming_SimpleBodyDecoder_hpp
#define oatpp_web_protocol_http_incoming_SimpleBodyDecoder_hpp

#include "BodyDecoder.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

class SimpleBodyDecoder : public BodyDecoder {
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
  
  void decode(const std::shared_ptr<Protocol::Headers>& headers,
              const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
              const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const override;
  
  oatpp::async::Action decodeAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   const oatpp::async::Action& actionOnReturn,
                                   const std::shared_ptr<Protocol::Headers>& headers,
                                   const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                   const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const override;
  
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_incoming_SimpleBodyDecoder_hpp */
