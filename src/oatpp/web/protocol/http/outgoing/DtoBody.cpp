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

#include "./DtoBody.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

DtoBody::DtoBody(const oatpp::data::mapping::type::AbstractObjectWrapper& dto,
                 oatpp::data::mapping::ObjectMapper* objectMapper,
                 bool chunked)
  : ChunkedBufferBody(oatpp::data::stream::ChunkedBuffer::createShared(), chunked)
  , m_dto(dto)
  , m_objectMapper(objectMapper)
{}

std::shared_ptr<DtoBody> DtoBody::createShared(const oatpp::data::mapping::type::AbstractObjectWrapper& dto,
                                               oatpp::data::mapping::ObjectMapper* objectMapper,
                                               bool chunked) {
  return Shared_Http_Outgoing_DtoBody_Pool::allocateShared(dto, objectMapper, chunked);
}

void DtoBody::declareHeaders(Headers& headers) noexcept {
  if(m_dto) {
    m_objectMapper->write(m_buffer, m_dto);
  }
  ChunkedBufferBody::declareHeaders(headers);

  auto it = headers.find(Header::CONTENT_TYPE);
  if(it == headers.end()) {
    headers[Header::CONTENT_TYPE] = m_objectMapper->getInfo().http_content_type;
  }
}

}}}}}
