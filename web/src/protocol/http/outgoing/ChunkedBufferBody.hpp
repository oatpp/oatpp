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

#ifndef oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp
#define oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp

#include "../Http.hpp"
#include "./Body.hpp"

#include "../../../../../../oatpp-lib/core/src/data/stream/ChunkedBuffer.hpp"
#include "../../../../../../oatpp-lib/core/src/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class ChunkedBufferBody : public oatpp::base::Controllable, public Body {
public:
  OBJECT_POOL(Http_Outgoing_ChunkedBufferBody_Pool, ChunkedBufferBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_ChunkedBufferBody_Pool, ChunkedBufferBody, 32)
protected:
  std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_buffer;
  bool m_chunked;
public:
  ChunkedBufferBody(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer,
                    bool chunked)
    : m_buffer(buffer)
    , m_chunked(chunked)
  {}
public:
  
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer) {
    return Shared_Http_Outgoing_ChunkedBufferBody_Pool::allocateShared(buffer, false);
  }
  
  static std::shared_ptr<ChunkedBufferBody> createShared(const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& buffer,
                                                   bool chunked) {
    return Shared_Http_Outgoing_ChunkedBufferBody_Pool::allocateShared(buffer, chunked);
  }
  
  void declareHeaders(const std::shared_ptr<Headers>& headers) override {
    if(m_chunked){
      headers->put(oatpp::web::protocol::http::Header::TRANSFER_ENCODING,
                   oatpp::web::protocol::http::Header::Value::TRANSFER_ENCODING_CHUNKED);
    } else {
      headers->put(oatpp::web::protocol::http::Header::CONTENT_LENGTH,
                   oatpp::utils::conversion::int64ToStr(m_buffer->getSize()));
    }
  }
  
  void writeToStream(const std::shared_ptr<OutputStream>& stream) override {
    if(m_chunked){
      auto chunks = m_buffer->getChunks();
      auto curr = chunks->getFirstNode();
      while (curr != nullptr) {
        stream->write(oatpp::utils::conversion::primitiveToStr(curr->getData()->size, "%X"));
        stream->write("\r\n", 2);
        stream->write(curr->getData()->data, curr->getData()->size);
        stream->write("\r\n", 2);
        curr = curr->getNext();
      }
      stream->write("0\r\n\r\n", 5);
    } else {
      m_buffer->flushToStream(stream);
    }
  }
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_ChunkedBufferBody_hpp */
