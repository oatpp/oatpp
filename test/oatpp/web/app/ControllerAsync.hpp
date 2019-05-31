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

#ifndef oatpp_test_web_app_ControllerAsync_hpp
#define oatpp_test_web_app_ControllerAsync_hpp

#include "./DTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

namespace oatpp { namespace test { namespace web { namespace app {
  
class ControllerAsync : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::ControllerAsync";
public:
  ControllerAsync(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:
  
  static std::shared_ptr<ControllerAsync> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<ControllerAsync>(objectMapper);
  }
  
#include OATPP_CODEGEN_BEGIN(ApiController)
  
  ENDPOINT_ASYNC("GET", "/", Root) {
    
    ENDPOINT_ASYNC_INIT(Root)
    
    Action act() {
      //OATPP_LOGV(TAG, "GET '/'");
      return _return(controller->createResponse(Status::CODE_200, "Hello World Async!!!"));
    }

  };

  ENDPOINT_ASYNC("GET", "params/{param}", GetWithParams) {

    ENDPOINT_ASYNC_INIT(GetWithParams)

    Action act() {
      auto param = request->getPathVariable("param");
      //OATPP_LOGV(TAG, "GET params/%s", param->c_str());
      auto dto = TestDto::createShared();
      dto->testValue = param;
      return _return(controller->createDtoResponse(Status::CODE_200, dto));
    }

  };

  ENDPOINT_ASYNC("GET", "headers", GetWithHeaders) {

    ENDPOINT_ASYNC_INIT(GetWithHeaders)

    Action act() {
      auto param = request->getHeader("X-TEST-HEADER");
      //OATPP_LOGV(TAG, "GET headers {X-TEST-HEADER: %s}", param->c_str());
      auto dto = TestDto::createShared();
      dto->testValue = param;
      return _return(controller->createDtoResponse(Status::CODE_200, dto));
    }

  };

  ENDPOINT_ASYNC("POST", "body", PostBody) {

    ENDPOINT_ASYNC_INIT(PostBody)

    Action act() {
      //OATPP_LOGV(TAG, "POST body. Reading body...");
      return request->readBodyToStringAsync().callbackTo(&PostBody::onBodyRead);
    }

    Action onBodyRead(const String& body) {
      //OATPP_LOGV(TAG, "POST body %s", body->c_str());
      auto dto = TestDto::createShared();
      dto->testValue = body;
      return _return(controller->createDtoResponse(Status::CODE_200, dto));
    }

  };

  ENDPOINT_ASYNC("POST", "echo", Echo) {

    ENDPOINT_ASYNC_INIT(Echo)

    Action act() {
      //OATPP_LOGV(TAG, "POST body(echo). Reading body...");
      return request->readBodyToStringAsync().callbackTo(&Echo::onBodyRead);
    }

    Action onBodyRead(const String& body) {
      //OATPP_LOGV(TAG, "POST echo size=%d", body->getSize());
      return _return(controller->createResponse(Status::CODE_200, body));
    }

  };
  
#include OATPP_CODEGEN_END(ApiController)
  
};
  
}}}}

#endif /* oatpp_test_web_app_ControllerAsync_hpp */
