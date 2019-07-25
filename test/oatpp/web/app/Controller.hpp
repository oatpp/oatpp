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

#ifndef oatpp_test_web_app_Controller_hpp
#define oatpp_test_web_app_Controller_hpp

#include "./DTOs.hpp"

#include "oatpp/web/mime/multipart/InMemoryReader.hpp"

#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"
#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <sstream>

namespace oatpp { namespace test { namespace web { namespace app {

class Controller : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::Controller";
public:
  Controller(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:
  
  static std::shared_ptr<Controller> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<Controller>(objectMapper);
  }
  
#include OATPP_CODEGEN_BEGIN(ApiController)
  
  ENDPOINT("GET", "/", root) {
    //OATPP_LOGV(TAG, "GET '/'");
    return createResponse(Status::CODE_200, "Hello World!!!");
  }
  
  ENDPOINT("GET", "params/{param}", getWithParams,
           PATH(String, param)) {
    //OATPP_LOGV(TAG, "GET params/%s", param->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = param;
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("GET", "queries", getWithQueries,
           QUERY(String, name), QUERY(Int32, age)) {
    auto dto = TestDto::createShared();
    dto->testValue = "name=" + name + "&age=" + oatpp::utils::conversion::int32ToStr(age->getValue());
    return createDtoResponse(Status::CODE_200, dto);
  }

  ENDPOINT("GET", "queries/map", getWithQueriesMap,
           QUERIES(QueryParams, queries)) {
    auto dto = TestDto::createShared();
    dto->testMap = dto->testMap->createShared();
    for(auto& it : queries) {
      dto->testMap->put(it.first.toString(), it.second.toString());
    }
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("GET", "headers", getWithHeaders,
           HEADER(String, param, "X-TEST-HEADER")) {
    //OATPP_LOGV(TAG, "GET headers {X-TEST-HEADER: %s}", param->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = param;
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("POST", "body", postBody,
           BODY_STRING(String, body)) {
    //OATPP_LOGV(TAG, "POST body %s", body->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = body;
    return createDtoResponse(Status::CODE_200, dto);
  }

  ENDPOINT("POST", "body-dto", postBodyDto,
           BODY_DTO(TestDto::ObjectWrapper, body)) {
    //OATPP_LOGV(TAG, "POST body %s", body->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = body->testValue;
    return createDtoResponse(Status::CODE_200, dto);
  }

  ENDPOINT("POST", "echo", echo,
           BODY_STRING(String, body)) {
    //OATPP_LOGV(TAG, "POST body(echo) size=%d", body->getSize());
    return createResponse(Status::CODE_200, body);
  }

  ENDPOINT("GET", "header-value-set", headerValueSet,
           HEADER(String, valueSet, "X-VALUE-SET")) {

    oatpp::web::protocol::http::HeaderValueData valueData;
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, valueSet, ',');

    OATPP_ASSERT_HTTP(valueData.tokens.find("VALUE_1") != valueData.tokens.end(), Status::CODE_400, "No header 'VALUE_1' in value set");
    OATPP_ASSERT_HTTP(valueData.tokens.find("VALUE_2") != valueData.tokens.end(), Status::CODE_400, "No header 'VALUE_2' in value set");
    OATPP_ASSERT_HTTP(valueData.tokens.find("VALUE_3") != valueData.tokens.end(), Status::CODE_400, "No header 'VALUE_3' in value set");
    return createResponse(Status::CODE_200, "");
  }

  class ReadCallback : public oatpp::data::stream::ReadCallback {
  private:
    oatpp::String m_text;
    v_int32 m_counter;
    v_int32 m_iterations;
  public:

    ReadCallback(const oatpp::String& text, v_int32 iterations)
      : m_text(text)
      , m_counter(0)
      , m_iterations(iterations)
    {}

    data::v_io_size read(void *buffer, data::v_io_size count) override {
      if(m_counter < m_iterations) {
        std::memcpy(buffer, m_text->getData(), m_text->getSize());
        m_counter ++;
        return m_text->getSize();
      }
      return 0;
    }

  };

  ENDPOINT("GET", "chunked/{text-value}/{num-iterations}", chunked,
           PATH(String, text, "text-value"),
           PATH(Int32, numIterations, "num-iterations"),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::ChunkedBody>
      (std::make_shared<ReadCallback>(text, numIterations->getValue()), nullptr, 1024);
    return OutgoingResponse::createShared(Status::CODE_200, body);
  }

  ENDPOINT("POST", "test/multipart/{chunk-size}", multipartTest,
           PATH(Int32, chunkSize, "chunk-size"),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {

    auto multipart = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());
    oatpp::web::mime::multipart::InMemoryReader multipartReader(multipart.get());
    request->transferBody(&multipartReader);

    auto responseBody = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(multipart, chunkSize->getValue());

    return OutgoingResponse::createShared(Status::CODE_200, responseBody);

  }

#include OATPP_CODEGEN_END(ApiController)
  
};

}}}}

#endif /* oatpp_test_web_app_Controller_hpp */
