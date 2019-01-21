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

#ifndef oatpp_test_web_app_Controller_hpp
#define oatpp_test_web_app_Controller_hpp

#include "./DTOs.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

namespace oatpp { namespace test { namespace web { namespace app {

class Controller : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::Controller";
public:
  Controller(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:
  
  static std::shared_ptr<Controller> createShared(const std::shared_ptr<ObjectMapper>& objectMapper){
    return std::make_shared<Controller>(objectMapper);
  }
  
#include OATPP_CODEGEN_BEGIN(ApiController)
  
  ENDPOINT("GET", "/", root) {
    OATPP_LOGD(TAG, "GET '/'");
    return createResponse(Status::CODE_200, "Hello World!!!");
  }
  
  ENDPOINT("GET", "params/{param}", getWithParams,
           PATH(String, param)) {
    OATPP_LOGD(TAG, "GET params/%s", param->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = param;
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("GET", "headers", getWithHeaders,
           HEADER(String, param, "X-TEST-HEADER")) {
    OATPP_LOGD(TAG, "GET headers {X-TEST-HEADER: %s}", param->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = param;
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("POST", "body", postBody,
           BODY_STRING(String, body)) {
    OATPP_LOGD(TAG, "POST body %s", body->c_str());
    auto dto = TestDto::createShared();
    dto->testValue = body;
    return createDtoResponse(Status::CODE_200, dto);
  }

#include OATPP_CODEGEN_END(ApiController)
  
};

}}}}

#endif /* oatpp_test_web_app_Controller_hpp */
