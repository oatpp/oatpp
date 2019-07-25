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

#include "BodyDecoder.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BodyDecoder

void BodyDecoder::decodeToStream(const Headers& headers,
                                 data::stream::InputStream* bodyStream,
                                 data::stream::OutputStream* toStream) const
{
  data::stream::DefaultWriteCallback callback(toStream);
  decode(headers, bodyStream, &callback);
}

async::CoroutineStarter BodyDecoder::decodeToStreamAsync(const Headers& headers,
                                                         const std::shared_ptr<data::stream::InputStream>& bodyStream,
                                                         const std::shared_ptr<data::stream::OutputStream>& toStream) const
{
  auto callback = std::make_shared<data::stream::DefaultAsyncWriteCallback>(toStream);
  return decodeAsync(headers, bodyStream, callback);
}

oatpp::async::CoroutineStarterForResult<const oatpp::String&>
BodyDecoder::decodeToStringAsync(const Headers& headers, const std::shared_ptr<data::stream::InputStream>& bodyStream) const {

  class ToStringDecoder : public oatpp::async::CoroutineWithResult<ToStringDecoder, const oatpp::String&> {
  private:
    const BodyDecoder* m_decoder;
    Headers m_headers;
    std::shared_ptr<oatpp::data::stream::InputStream> m_bodyStream;
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> m_chunkedBuffer;
  public:

    ToStringDecoder(const BodyDecoder* decoder,
                    const Headers& headers,
                    const std::shared_ptr<data::stream::InputStream>& bodyStream)
      : m_decoder(decoder)
      , m_headers(headers)
      , m_bodyStream(bodyStream)
      , m_chunkedBuffer(data::stream::ChunkedBuffer::createShared())
    {}

    Action act() override {
      return m_decoder->decodeToStreamAsync(m_headers, m_bodyStream, m_chunkedBuffer).next(yieldTo(&ToStringDecoder::onDecoded));
    }

    Action onDecoded() {
      return _return(m_chunkedBuffer->toString());
    }

  };

  return ToStringDecoder::startForResult(this, headers, bodyStream);

}

}}}}}
