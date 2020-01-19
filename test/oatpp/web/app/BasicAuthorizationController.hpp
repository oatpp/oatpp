/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
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

#ifndef oatpp_test_web_app_AuthorizationController_hpp
#define oatpp_test_web_app_AuthorizationController_hpp

#include "./DTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <sstream>

namespace oatpp { namespace test { namespace web { namespace app {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Default Basic Authorization

class DefaultBasicAuthorizationController : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::BasicAuthorizationController";
public:

  DefaultBasicAuthorizationController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {
    setDefaultAuthorizationHandler(std::make_shared<oatpp::web::server::handler::BasicAuthorizationHandler>("default-test-realm"));
  }

public:

  static std::shared_ptr<DefaultBasicAuthorizationController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<DefaultBasicAuthorizationController>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT("GET", "default-basic-authorization", basicAuthorization,
           AUTHORIZATION(std::shared_ptr<oatpp::web::server::handler::DefaultBasicAuthorizationObject>, authObject)) {

    auto dto = TestDto::createShared();
    dto->testValue = authObject->userId + ":" + authObject->password;

    if(dto->testValue == "foo:bar") {
      return createDtoResponse(Status::CODE_200, dto);
    } else {
      return createDtoResponse(Status::CODE_401, dto);
    }

  }

#include OATPP_CODEGEN_END(ApiController)

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom Basic Authorization

class MyAuthorizationObject : public oatpp::web::server::handler::AuthorizationObject {
public:

  MyAuthorizationObject(v_int64 pId, const oatpp::String& pAuthString)
    : id(pId)
    , authString(pAuthString)
  {}

  v_int64 id;
  oatpp::String authString;

};

class MyBasicAuthorizationHandler : public oatpp::web::server::handler::BasicAuthorizationHandler {
public:

  MyBasicAuthorizationHandler()
    : BasicAuthorizationHandler("custom-test-realm")
  {}

  std::shared_ptr<AuthorizationObject> authorize(const oatpp::String& userId, const oatpp::String& password) override {
    if(userId == "foo" && password == "bar") {
      return std::make_shared<MyAuthorizationObject>(1337, userId + ":" + password);
    }
    return nullptr;
  }

};

class BasicAuthorizationController : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::BasicAuthorizationController";
public:
  std::shared_ptr<AuthorizationHandler> m_authHandler = std::make_shared<MyBasicAuthorizationHandler>();
public:
  BasicAuthorizationController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

  static std::shared_ptr<BasicAuthorizationController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<BasicAuthorizationController>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT("GET", "basic-authorization", basicAuthorization,
           AUTHORIZATION(std::shared_ptr<MyAuthorizationObject>, authObject, m_authHandler)) {

    auto dto = TestDto::createShared();
    dto->testValue = authObject->authString;

    if(dto->testValue == "foo:bar" && authObject->id == 1337) {
      return createDtoResponse(Status::CODE_200, dto);
    } else {
      return createDtoResponse(Status::CODE_401, dto);
    }

  }

#include OATPP_CODEGEN_END(ApiController)

};

}}}}

#endif /* oatpp_test_web_app_Controller_hpp */
