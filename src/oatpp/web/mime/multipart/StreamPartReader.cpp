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

#include "StreamPartReader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

const char* const StreamPartReader::TAG_NAME = "[oatpp::web::mime::multipart::StreamPartReader::TAG]";

StreamPartReader::StreamPartReader(const std::shared_ptr<PartReaderStreamProvider>& streamProvider,
                                   v_io_size maxDataSize)
  : m_streamProvider(streamProvider)
  , m_maxDataSize(maxDataSize)
{}

void StreamPartReader::onNewPart(const std::shared_ptr<Part>& part) {

  if(!m_streamProvider) {
    throw std::runtime_error("[oatpp::web::mime::multipart::StreamPartReader::onNewPart()]: Error. Stream provider is nullptr.");
  }

  auto tag = part->getTagObject();

  if(tag) {
    throw std::runtime_error("[oatpp::web::mime::multipart::StreamPartReader::onNewPart()]: Error. "
                             "Part tag object is not nullptr. Seems like this part is already being processed by another part reader.");
  }

  auto tagObject = std::make_shared<TagObject>();
  tagObject->outputStream = m_streamProvider->getOutputStream(part);
  part->setTag(TAG_NAME, tagObject);

}

void StreamPartReader::onPartData(const std::shared_ptr<Part>& part, p_char8 data, oatpp::v_io_size size) {

  auto tag = part->getTagObject();
  if(!tag) {
    throw std::runtime_error("[oatpp::web::mime::multipart::StreamPartReader::onPartData()]: Error. "
                             "Part tag object is nullptr.");

  }

  if(part->getTagName() != TAG_NAME) {
    throw std::runtime_error("[oatpp::web::mime::multipart::StreamPartReader::onPartData()]: Error. "
                             "Wrong tag name. Seems like this part is already being processed by another part reader.");

  }

  auto tagObject = std::static_pointer_cast<TagObject>(tag);

  if(size > 0) {
    if(m_maxDataSize > 0 && tagObject->size + size > m_maxDataSize) {
      OATPP_LOGE("[oatpp::web::mime::multipart::StreamPartReader::onPartData()]", "Error. Part size exceeds specified maxDataSize=%d", m_maxDataSize);
      throw std::runtime_error("[oatpp::web::mime::multipart::StreamPartReader::onPartData()]: Error. Part size exceeds specified maxDataSize");
    }
    auto res = tagObject->outputStream->writeExactSizeDataSimple(data, size);
    if(res != size) {
      OATPP_LOGE("[oatpp::web::mime::multipart::StreamPartReader::onPartData()]", "Error. Failed to stream all data. Streamed %d/%d", res, size);
      throw std::runtime_error("[oatpp::web::mime::multipart::StreamPartReader::onPartData()]: Error. Failed to stream all data.");
    }
    tagObject->size += res;
  } else {
    part->clearTag();
    part->setDataInfo(m_streamProvider->getInputStream(part));
  }

}


const char* const AsyncStreamPartReader::TAG_NAME = "[oatpp::web::mime::multipart::AsyncStreamPartReader::TAG]";

AsyncStreamPartReader::AsyncStreamPartReader(const std::shared_ptr<AsyncPartReaderStreamProvider>& streamProvider,
                                             v_io_size maxDataSize)
  : m_streamProvider(streamProvider)
  , m_maxDataSize(maxDataSize)
{}

async::CoroutineStarter AsyncStreamPartReader::onNewPartAsync(const std::shared_ptr<Part>& part) {

  class OnNewPartCoroutine : public async::Coroutine<OnNewPartCoroutine> {
  private:
    std::shared_ptr<Part> m_part;
    std::shared_ptr<AsyncPartReaderStreamProvider> m_streamProvider;
    std::shared_ptr<data::stream::OutputStream> m_obtainedStream;
  public:

    OnNewPartCoroutine(const std::shared_ptr<Part>& part,
                       const std::shared_ptr<AsyncPartReaderStreamProvider>& streamProvider)
      : m_part(part)
      , m_streamProvider(streamProvider)
    {}

    Action act() override {

      if(!m_streamProvider) {
        throw std::runtime_error("[oatpp::web::mime::multipart::AsyncStreamPartReader::onNewPartAsync(){OnNewPartCoroutine}]: Error. Stream provider is nullptr.");
      }

      auto tag = m_part->getTagObject();

      if(tag) {
        throw std::runtime_error("[oatpp::web::mime::multipart::AsyncStreamPartReader::onNewPartAsync(){OnNewPartCoroutine}]: Error. "
                                 "Part tag object is not nullptr. Seems like this part is already being processed by another part reader.");
      }

      return m_streamProvider->getOutputStreamAsync(m_part, m_obtainedStream).next(yieldTo(&OnNewPartCoroutine::onStreamObtained));

    }

    Action onStreamObtained() {
      auto tagObject = std::make_shared<TagObject>();
      tagObject->outputStream = m_obtainedStream;
      m_part->setTag(TAG_NAME, tagObject);
      return finish();
    }

  };

  return OnNewPartCoroutine::start(part, m_streamProvider);

}

async::CoroutineStarter AsyncStreamPartReader::onPartDataAsync(const std::shared_ptr<Part>& part, p_char8 data, oatpp::v_io_size size) {

  auto tag = part->getTagObject();
  if(!tag) {
    throw std::runtime_error("[oatpp::web::mime::multipart::AsyncStreamPartReader::onPartDataAsync()]: Error. "
                             "Part tag object is nullptr.");

  }

  if(part->getTagName() != TAG_NAME) {
    throw std::runtime_error("[oatpp::web::mime::multipart::AsyncStreamPartReader::onPartDataAsync()]: Error. "
                             "Wrong tag name. Seems like this part is already being processed by another part reader.");

  }

  auto tagObject = std::static_pointer_cast<TagObject>(tag);

  if(size > 0) {
    if(m_maxDataSize > 0 && tagObject->size + size > m_maxDataSize) {
      OATPP_LOGE("[oatpp::web::mime::multipart::AsyncStreamPartReader::onPartDataAsync()]", "Error. Part size exceeds specified maxDataSize=%d", m_maxDataSize);
      throw std::runtime_error("[oatpp::web::mime::multipart::AsyncStreamPartReader::onPartDataAsync()]: Error. Part size exceeds specified maxDataSize");
    }
    return tagObject->outputStream->writeExactSizeDataAsync(data, size);
  } else {
    return onPartDone(part);
  }

}

async::CoroutineStarter AsyncStreamPartReader::onPartDone(const std::shared_ptr<Part>& part) {

  class OnPartDoneCoroutine : public async::Coroutine<OnPartDoneCoroutine> {
  public:
    std::shared_ptr<Part> m_part;
    std::shared_ptr<AsyncPartReaderStreamProvider> m_streamProvider;
    std::shared_ptr<data::stream::InputStream> m_obtainedStream;
  public:

    OnPartDoneCoroutine(const std::shared_ptr<Part>& part,
                        const std::shared_ptr<AsyncPartReaderStreamProvider>& streamProvider)
      : m_part(part)
      , m_streamProvider(streamProvider)
    {}

    Action act() override {
      return m_streamProvider->getInputStreamAsync(m_part, m_obtainedStream).next(yieldTo(&OnPartDoneCoroutine::onStreamObtained));
    }

    Action onStreamObtained() {
      m_part->setDataInfo(m_obtainedStream);
      m_part->clearTag();
      return finish();
    }

  };

  return OnPartDoneCoroutine::start(part, m_streamProvider);

}

}}}}
