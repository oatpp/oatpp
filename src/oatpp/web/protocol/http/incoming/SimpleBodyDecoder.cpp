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

#include "SimpleBodyDecoder.hpp"

#include "oatpp/web/protocol/http/encoding/Chunked.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

void SimpleBodyDecoder::decode(const Headers& headers,
                               data::stream::InputStream* bodyStream,
                               data::stream::WriteCallback* writeCallback) const {

  auto transferEncoding = headers.getAsMemoryLabel<data::share::StringKeyLabelCI_FAST>(Header::TRANSFER_ENCODING);
  if(transferEncoding && transferEncoding == Header::Value::TRANSFER_ENCODING_CHUNKED) {
    http::encoding::DecoderChunked decoder;
    data::buffer::IOBuffer buffer;
    data::stream::transfer(bodyStream, writeCallback, 0 /* read until error */, buffer.getData(), buffer.getSize(), &decoder);
  } else {

    auto contentLengthStr = headers.getAsMemoryLabel<data::share::StringKeyLabel>(Header::CONTENT_LENGTH);
    if(!contentLengthStr) {

      auto connectionStr = headers.getAsMemoryLabel<data::share::StringKeyLabelCI_FAST>(Header::CONNECTION);
      if(connectionStr && connectionStr == "close") {
        data::buffer::IOBuffer buffer;
        data::stream::transfer(bodyStream, writeCallback, 0 /* read until error */, buffer.getData(), buffer.getSize());
      } // else - do nothing. invalid response.

      return;

    } else {

      bool success;
      auto contentLength = utils::conversion::strToInt64(contentLengthStr.toString(), success);

      if(!success){
        return; // it is an invalid request/response
      }

      if(contentLength > 0) {
        data::buffer::IOBuffer buffer;
        data::stream::transfer(bodyStream, writeCallback, contentLength, buffer.getData(), buffer.getSize());
      }

    }
  }
  
}

async::CoroutineStarter SimpleBodyDecoder::decodeAsync(const Headers& headers,
                                                       const std::shared_ptr<data::stream::InputStream>& bodyStream,
                                                       const std::shared_ptr<data::stream::WriteCallback>& writeCallback) const {
  auto transferEncoding = headers.getAsMemoryLabel<data::share::StringKeyLabelCI_FAST>(Header::TRANSFER_ENCODING);
  if(transferEncoding && transferEncoding == Header::Value::TRANSFER_ENCODING_CHUNKED) {
    auto decoder = std::make_shared<http::encoding::DecoderChunked>();
    return data::stream::transferAsync(bodyStream, writeCallback, 0 /* read until error */, data::buffer::IOBuffer::createShared(), decoder);
  } else {

    auto contentLengthStr = headers.getAsMemoryLabel<data::share::StringKeyLabel>(Header::CONTENT_LENGTH);
    if(!contentLengthStr) {

      auto connectionStr = headers.getAsMemoryLabel<data::share::StringKeyLabelCI_FAST>(Header::CONNECTION);
      if(connectionStr && connectionStr == "close") {
        return data::stream::transferAsync(bodyStream, writeCallback, 0 /* read until error */, data::buffer::IOBuffer::createShared());
      }

      return nullptr; // else - do nothing. invalid response.

    } else {

      bool success;
      auto contentLength = utils::conversion::strToInt64(contentLengthStr.toString(), success);

      if(!success){
        throw HttpError(http::Status::CODE_400, "Invalid 'Content-Length' Header");
      }

      if(contentLength > 0) {
        return data::stream::transferAsync(bodyStream, writeCallback, contentLength, data::buffer::IOBuffer::createShared());
      } else {
        return nullptr;
      }

    }
  }
}
  
}}}}}
