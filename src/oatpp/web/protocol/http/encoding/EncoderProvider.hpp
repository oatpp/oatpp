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

#ifndef oatpp_web_protocol_http_encoding_EncoderProvider_hpp
#define oatpp_web_protocol_http_encoding_EncoderProvider_hpp

#include "oatpp/core/data/buffer/Processor.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace encoding {

/**
 * Provider of encoding or decoding &id:oatpp::data::buffer::Processor;.
 */
class EncoderProvider {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~EncoderProvider() = default;

  /**
   * Get name of the encoding. This name is used in HTTP headers.
   * @return - name of the encoding. Ex.: "gzip" or "deflate" or "chunked".
   */
  virtual oatpp::String getEncodingName() = 0;

  /**
   * Get &id:oatpp::data::buffer::Processor; for decoding/encoding.
   * @return - &id:oatpp::data::buffer::Processor;
   */
  virtual std::shared_ptr<data::buffer::Processor> getProcessor() = 0;

};

}}}}}

#endif // oatpp_web_protocol_http_encoding_EncoderProvider_hpp
