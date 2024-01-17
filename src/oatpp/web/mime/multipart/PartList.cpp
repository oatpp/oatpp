/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
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

#include "PartList.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

PartList::PartList(const oatpp::String& boundary)
  : Multipart(boundary)
  , m_readIteratorInitialized(false)
{}

PartList::PartList(const Headers& requestHeaders)
  : Multipart(parseBoundaryFromHeaders(requestHeaders))
  , m_readIteratorInitialized(false)
{
  if(!getBoundary()) {
    throw std::runtime_error("[oatpp::web::mime::multipart::PartList::PartList]: Error. No 'boundary' value found in headers.");
  }
}

std::shared_ptr<PartList> PartList::createSharedWithRandomBoundary(v_int32 boundarySize) {
  auto boundary = generateRandomBoundary(boundarySize);
  return std::make_shared<PartList>(boundary);
}

std::shared_ptr<Part> PartList::readNextPart(async::Action& action) {
  if(!m_readIteratorInitialized) {
    m_readIteratorInitialized = true;
    m_iterator = m_parts.begin();
  }
  if(m_iterator == m_parts.end()) {
    return nullptr;
  }
  return *m_iterator ++;
}

void PartList::writeNextPart(const std::shared_ptr<Part>& part, async::Action& action) {

  if(part->getName()) {
    m_namedParts[part->getName()].push_back(part);
  }

  m_parts.push_back(part);

}

std::shared_ptr<Part> PartList::getNamedPart(const oatpp::String& name) {

  auto it = m_namedParts.find(name);
  if(it != m_namedParts.end()) {
    return it->second.front();
  }

  return nullptr;

}

std::list<std::shared_ptr<Part>> PartList::getNamedParts(const oatpp::String& name) {

  auto it = m_namedParts.find(name);
  if(it != m_namedParts.end()) {
    return it->second;
  }

  return std::list<std::shared_ptr<Part>>{};

}

const std::list<std::shared_ptr<Part>>& PartList::getAllParts() {
  return m_parts;
}

v_int64 PartList::count() {
  return static_cast<v_int64>(m_parts.size());
}

}}}}
