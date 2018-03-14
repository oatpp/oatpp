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

#ifndef oatpp_web_protocol_http_outgoing_DtoBody_hpp
#define oatpp_web_protocol_http_outgoing_DtoBody_hpp

#include "../Http.hpp"
#include "./ChunkedBufferBody.hpp"

#include "../../../../../../oatpp-lib/core/src/data/mapping/ObjectMapper.hpp"
#include "../../../../../../oatpp-lib/core/src/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class DtoBody : public ChunkedBufferBody {
public:
  OBJECT_POOL(Http_Outgoing_DtoBody_Pool, DtoBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_DtoBody_Pool, DtoBody, 32)
private:
  oatpp::data::mapping::type::AbstractSharedWrapper m_dto;
  oatpp::data::mapping::ObjectMapper* m_objectMapper;
public:
  DtoBody(const oatpp::data::mapping::type::AbstractSharedWrapper& dto,
          oatpp::data::mapping::ObjectMapper* objectMapper,
          bool chunked)
    : ChunkedBufferBody(oatpp::data::stream::ChunkedBuffer::createShared(), chunked)
    , m_dto(dto)
    , m_objectMapper(objectMapper)
  {}
public:
  
  static std::shared_ptr<DtoBody> createShared(const oatpp::data::mapping::type::AbstractSharedWrapper& dto,
                                         oatpp::data::mapping::ObjectMapper* objectMapper) {
    return Shared_Http_Outgoing_DtoBody_Pool::allocateShared(dto, objectMapper, false);
  }
  
  static std::shared_ptr<DtoBody> createShared(const oatpp::data::mapping::type::AbstractSharedWrapper& dto,
                                         oatpp::data::mapping::ObjectMapper* objectMapper,
                                         bool chunked) {
    return Shared_Http_Outgoing_DtoBody_Pool::allocateShared(dto, objectMapper, chunked);
  }
  
  void declareHeaders(const std::shared_ptr<Headers>& headers) override {
    m_objectMapper->write(m_buffer, m_dto);
    ChunkedBufferBody::declareHeaders(headers);
    headers->putIfNotExists(Header::CONTENT_TYPE, m_objectMapper->getInfo().http_content_type);
  }
  
};

}}}}}
  
#endif /* oatpp_web_protocol_http_outgoing_DtoBody_hpp */
