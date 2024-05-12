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

#include "ContentMappers.hpp"

namespace oatpp { namespace web { namespace mime {

void ContentMappers::putMapper(const std::shared_ptr<data::mapping::ObjectMapper>& mapper) {
  std::unique_lock lock(m_mutex);
  if(m_defaultMapper == nullptr) {
    m_defaultMapper = mapper;
  }
  m_mappers[mapper->getInfo().httpContentType] = mapper;
}

void ContentMappers::setDefaultMapper(const oatpp::String& contentType) {
  std::unique_lock lock(m_mutex);
  if(m_defaultMapper == nullptr) {
    m_defaultMapper = m_mappers.at(contentType);
  }
}

void ContentMappers::setDefaultMapper(const std::shared_ptr<data::mapping::ObjectMapper>& mapper) {
  std::unique_lock lock(m_mutex);
  m_defaultMapper = mapper;
  if(m_defaultMapper) {
    m_mappers[m_defaultMapper->getInfo().httpContentType] = m_defaultMapper;
  }
}

std::shared_ptr<data::mapping::ObjectMapper> ContentMappers::getMapper(const oatpp::String& contentType) const {
  std::shared_lock lock(m_mutex);
  auto it = m_mappers.find(contentType);
  if(it == m_mappers.end()) {
    return nullptr;
  }
  return it->second;
}

std::shared_ptr<data::mapping::ObjectMapper> ContentMappers::getDefaultMapper() const {
  std::shared_lock lock(m_mutex);
  return m_defaultMapper;
}

std::shared_ptr<data::mapping::ObjectMapper> ContentMappers::selectMapper(const protocol::http::HeaderValueData& values) const {
  // TODO select
  return m_defaultMapper;
}

std::shared_ptr<data::mapping::ObjectMapper> ContentMappers::selectMapper(const oatpp::String& contentType) const {

  std::shared_lock lock(m_mutex);

  if(!contentType || contentType->empty()) {
    return m_defaultMapper;
  }

  protocol::http::HeaderValueData values;
  protocol::http::Parser::parseHeaderValueData(values, contentType, ',');

  return selectMapper(values);

}

std::shared_ptr<data::mapping::ObjectMapper> ContentMappers::selectMapper(const std::vector<oatpp::String>& acceptableContentTypes) const {

  std::shared_lock lock(m_mutex);

  if(acceptableContentTypes.empty()) {
    return m_defaultMapper;
  }

  protocol::http::HeaderValueData values;
  for(auto& ct : acceptableContentTypes) {
    protocol::http::Parser::parseHeaderValueData(values, ct, ',');
  }

  return selectMapper(values);

}

void ContentMappers::clear() {
  std::unique_lock lock(m_mutex);
  m_defaultMapper = nullptr;
  m_mappers.clear();
}

}}}
