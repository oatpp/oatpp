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

#include "oatpp/web/mime/multipart/FileStreamProvider.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"

#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"
#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <sstream>

namespace oatpp { namespace test { namespace web { namespace app {

class MyAuthorizationObject : public oatpp::web::server::handler::AuthorizationObject {
public:
  oatpp::Int64 id;
};

class MyAuthorizationHandler : public oatpp::web::server::handler::AuthorizationHandler {
public:
  std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> handleAuthorization(const oatpp::String &header) override {
    auto def = oatpp::web::server::handler::DefaultAuthorizationHandler::defaultAuthorizationObject(header);
    auto my = std::make_shared<MyAuthorizationObject>();
    my->user = def->user;
    my->password = def->password;
    if(my->user != "foo" || my->password != "bar") {
      return nullptr;
    }
    my->id = 1337;
    return my;
  }
};

class AuthorizationController : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::AuthorizationController";

public:
  AuthorizationController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {
    m_authorizationHandler = std::make_shared<MyAuthorizationHandler>();
  }
public:

  static std::shared_ptr<AuthorizationController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<AuthorizationController>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT("GET", "mydefauthorization", authorization,
           AUTHORIZATION(std::shared_ptr<oatpp::web::server::handler::AuthorizationObject>, authorizationHeader)) {
    auto dto = TestDto::createShared();
    dto->testValue = authorizationHeader->user + ":" + authorizationHeader->password;
    if(dto->testValue == "foo:bar") {
      return createDtoResponse(Status::CODE_200, dto);
    } else {
      return createDtoResponse(Status::CODE_401, dto);
    }
  }

  ENDPOINT("GET", "myauthorization", myauthorization,
           AUTHORIZATION(std::shared_ptr<MyAuthorizationObject>, authorizationHeader)) {
    auto dto = TestDto::createShared();
    dto->testValue = authorizationHeader->user + ":" + authorizationHeader->password;
    if(dto->testValue == "foo:bar" && authorizationHeader->id == oatpp::Int64(1337)) {
      return createDtoResponse(Status::CODE_200, dto);
    } else {
      return createDtoResponse(Status::CODE_401, dto);
    }
  }

  ENDPOINT("GET", "myauthorizationrealm", myauthorizationrealm,
           AUTHORIZATION(std::shared_ptr<MyAuthorizationObject>, authorizationHeader, "Test Realm")) {
    auto dto = TestDto::createShared();
    dto->testValue = authorizationHeader->user + ":" + authorizationHeader->password;
    if(dto->testValue == "foo:bar" && authorizationHeader->id == oatpp::Int64(1337)) {
      return createDtoResponse(Status::CODE_200, dto);
    } else {
      return createDtoResponse(Status::CODE_401, dto);
    }
  }
#include OATPP_CODEGEN_END(ApiController)

};

}}}}

#endif /* oatpp_test_web_app_Controller_hpp */
