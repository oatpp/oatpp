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

#include "InMemoryPartReader.hpp"

#include "oatpp/core/data/stream/BufferInputStream.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

InMemoryPartReader::InMemoryPartReader(data::v_io_size maxDataSize)
  : m_maxDataSize(maxDataSize)
{}

void InMemoryPartReader::onNewPart(const std::shared_ptr<Part>& part) {
  // DO NOTHING
}

void InMemoryPartReader::onPartData(const std::shared_ptr<Part>& part, p_char8 data, oatpp::data::v_io_size size) {
  if(size > 0) {
    if(m_buffer.getSize() + size > m_maxDataSize) {
      OATPP_LOGE("[oatpp::web::mime::multipart::InMemoryPartReader::onPartData()]", "Error. Part size exceeds specified maxDataSize=%d", m_maxDataSize);
      throw std::runtime_error("[oatpp::web::mime::multipart::InMemoryPartReader::onPartData()]: Error. Part size exceeds specified maxDataSize");
    }
    m_buffer.write(data, size);
  } else {
    auto fullData = m_buffer.toString();
    m_buffer.clear();
    auto stream = std::make_shared<data::stream::BufferInputStream>(fullData.getPtr(), fullData->getData(), fullData->getSize());
    part->setDataInfo(stream, fullData, fullData->getSize());
  }
}


AsyncInMemoryPartReader::AsyncInMemoryPartReader(data::v_io_size maxDataSize)
  : m_maxDataSize(maxDataSize)
{}

async::CoroutineStarter AsyncInMemoryPartReader::onNewPartAsync(const std::shared_ptr<Part>& part) {
  return nullptr; // DO NOTHING
}

async::CoroutineStarter AsyncInMemoryPartReader::onPartDataAsync(const std::shared_ptr<Part>& part, p_char8 data, oatpp::data::v_io_size size) {
  if(size > 0) {
    if(m_buffer.getSize() + size > m_maxDataSize) {
      OATPP_LOGE("[oatpp::web::mime::multipart::AsyncInMemoryPartReader::onPartDataAsync()]", "Error. Part size exceeds specified maxDataSize=%d", m_maxDataSize);
      throw std::runtime_error("[oatpp::web::mime::multipart::AsyncInMemoryPartReader::onPartDataAsync()]: Error. Part size exceeds specified maxDataSize");
    }
    m_buffer.write(data, size);
  } else {
    auto fullData = m_buffer.toString();
    m_buffer.clear();
    auto stream = std::make_shared<data::stream::BufferInputStream>(fullData.getPtr(), fullData->getData(), fullData->getSize());
    part->setDataInfo(stream, fullData, fullData->getSize());
  }
  return nullptr;
}

}}}}
