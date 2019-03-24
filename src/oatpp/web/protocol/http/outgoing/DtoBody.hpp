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

#ifndef oatpp_web_protocol_http_outgoing_DtoBody_hpp
#define oatpp_web_protocol_http_outgoing_DtoBody_hpp

#include "./ChunkedBufferBody.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Implementation of &id:oatpp::web::protocol::http::outgoing::Body; class. <br>
 * Extends of &id:oatpp::web::protocol::http::outgoing::ChunkedBufferBody; class. <br>
 * Implements functionality to use DTO Object (see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/)) as data source for http body.
 */
class DtoBody : public ChunkedBufferBody {
public:
  OBJECT_POOL(Http_Outgoing_DtoBody_Pool, DtoBody, 32)
  SHARED_OBJECT_POOL(Shared_Http_Outgoing_DtoBody_Pool, DtoBody, 32)
private:
  oatpp::data::mapping::type::AbstractObjectWrapper m_dto;
  oatpp::data::mapping::ObjectMapper* m_objectMapper;
public:
  /**
   * Constructor.
   * @param dto - &id:oatpp::data::mapping::type::AbstractObjectWrapper;.
   * @param objectMapper - &id:oatpp::data::mapping::ObjectMapper;.
   * @param chunked - set `true` to send using HTTP chunked transfer encoding. Set `false` to send as body with specified `Content-Length` header.
   */
  DtoBody(const oatpp::data::mapping::type::AbstractObjectWrapper& dto,
          oatpp::data::mapping::ObjectMapper* objectMapper,
          bool chunked = false);
public:

  /**
   * Create shared DtoBody.
   * @param dto - &id:oatpp::data::mapping::type::AbstractObjectWrapper;.
   * @param objectMapper - &id:oatpp::data::mapping::ObjectMapper;.
   * @param chunked - set `true` to send using HTTP chunked transfer encoding. Set `false` to send as body with specified `Content-Length` header.
   * @return - `std::shared_ptr` to DtoBody.
   */
  static std::shared_ptr<DtoBody> createShared(const oatpp::data::mapping::type::AbstractObjectWrapper& dto,
                                               oatpp::data::mapping::ObjectMapper* objectMapper,
                                               bool chunked = false);

  /**
   * Add `Transfer-Encoding: chunked` header if `chunked` option was set to `true`.<br>
   * Else, add `Content-Length` header if `chunked` option was set to `false`.<br>
   * <br>
   * Add `Content-Type` header depending on &id:oatpp::data::mapping::ObjectMapper; used. See &id:oatpp::data::mapping::ObjectMapper::Info;.
   *
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  void declareHeaders(Headers& headers) noexcept override;
  
};

}}}}}
  
#endif /* oatpp_web_protocol_http_outgoing_DtoBody_hpp */
