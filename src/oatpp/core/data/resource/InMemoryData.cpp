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

#include "InMemoryData.hpp"

namespace oatpp { namespace data { namespace resource {

InMemoryData::OutputDataHandle::~OutputDataHandle() {
  dataHandle->data = data + stream->toString();
}

InMemoryData::InMemoryData(const oatpp::String& data)
  : m_handle(std::make_shared<DataHandle>(data))
{}

std::shared_ptr<data::stream::OutputStream> InMemoryData::openOutputStream() {
  auto outputDataHandle = std::make_shared<OutputDataHandle>();
  if(!m_handle) {
    m_handle = std::make_shared<DataHandle>("");
  }
  if(!m_handle->data){
    m_handle->data = "";
  }
  outputDataHandle->dataHandle = m_handle;
  outputDataHandle->data = m_handle->data;

  auto stream = std::make_shared<data::stream::BufferOutputStream>(1024, outputDataHandle);
  outputDataHandle->stream = stream.get();
  return stream;
}

std::shared_ptr<data::stream::InputStream> InMemoryData::openInputStream() {
  if(!m_handle) {
    m_handle = std::make_shared<DataHandle>("");
  }
  if(!m_handle->data){
    m_handle->data = "";
  }
  return std::make_shared<data::stream::BufferInputStream>(m_handle->data, m_handle);
}

oatpp::String InMemoryData::getInMemoryData() {
  if(m_handle && m_handle->data) {
    return m_handle->data;
  }
  return nullptr;
}

v_int64 InMemoryData::getKnownSize() {
  if(m_handle && m_handle->data) {
    return m_handle->data->size();
  }
  return 0;
}

oatpp::String InMemoryData::getLocation() {
  return nullptr;
}

}}}
