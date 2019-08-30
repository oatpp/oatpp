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

#ifndef oatpp_test_web_app_BearerAuthorizationController_hpp
#define oatpp_test_web_app_BearerAuthorizationController_hpp

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

class BearerAuthorizationObject : public oatpp::web::server::handler::AuthorizationObject {
public:
  oatpp::String user;
  oatpp::String password;
  oatpp::String token;
};

class BearerAuthorizationHandler : public oatpp::web::server::handler::AuthorizationHandler {
public:

  std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> handleAuthorization(const oatpp::String &header) override {
    if(!header->startsWith("Bearer ")) {
      return nullptr;
    }

    oatpp::String token = (const char*)(header->c_str() + 7);
    // foo:bar = 4e99e8c12de7e01535248d2bac85e732

    if(token == "4e99e8c12de7e01535248d2bac85e732") {
      auto obj = std::make_shared<BearerAuthorizationObject>();
      obj->user = "foo";
      obj->password = "bar";
      obj->token = token;
      return obj;
    }

    return nullptr;
  }

};

class BearerAuthorizationController : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::BearerAuthorizationController";

public:
  BearerAuthorizationController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {
    m_authorizationHandler = std::make_shared<BearerAuthorizationHandler>();
  }
public:

  static std::shared_ptr<BearerAuthorizationController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<BearerAuthorizationController>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT("GET", "bearerauthorization", authorization,
           AUTHORIZATION(std::shared_ptr<BearerAuthorizationObject>, authorizatioBearer, "Bearer Realm", "Bearer")) {
    auto dto = TestDto::createShared();
    dto->testValue = authorizatioBearer->user + ":" + authorizatioBearer->password;
    if(dto->testValue == "foo:bar" && authorizatioBearer->token == "4e99e8c12de7e01535248d2bac85e732") {
      return createDtoResponse(Status::CODE_200, dto);
    } else {
      return createDtoResponse(Status::CODE_401, dto);
    }
  }

#include OATPP_CODEGEN_END(ApiController)

};

}}}}

#endif /* oatpp_test_web_app_Controller_hpp */
