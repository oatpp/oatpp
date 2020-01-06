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

#include "ChunkedBody.hpp"

#include "oatpp/web/protocol/http/encoding/Chunked.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

ChunkedBody::ChunkedBody(const std::shared_ptr<ReadCallback>& readCallback)
  : m_readCallback(readCallback)
  , m_buffer(std::make_shared<data::buffer::IOBuffer>())
{}

void ChunkedBody::declareHeaders(Headers& headers) {
  headers.put_LockFree(oatpp::web::protocol::http::Header::TRANSFER_ENCODING, oatpp::web::protocol::http::Header::Value::TRANSFER_ENCODING_CHUNKED);
}

void ChunkedBody::writeToStream(OutputStream* stream) {
  http::encoding::EncoderChunked encoder;
  data::stream::transfer(m_readCallback.get(), stream, 0, m_buffer->getData(), m_buffer->getSize(), &encoder);
}

oatpp::async::CoroutineStarter ChunkedBody::writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) {
  auto encoder = std::make_shared<http::encoding::EncoderChunked>();
  return data::stream::transferAsync(m_readCallback, stream, 0, m_buffer, encoder);
}

v_buff_size ChunkedBody::getKnownSize() {
  return -1;
}


}}}}}