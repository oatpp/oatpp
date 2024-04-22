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

#ifndef oatpp_test_web_app_ControllerWithInterceptors_hpp
#define oatpp_test_web_app_ControllerWithInterceptors_hpp

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/utils/Conversion.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include <sstream>

namespace oatpp { namespace test { namespace web { namespace app {

namespace multipart = oatpp::web::mime::multipart;

class ControllerWithInterceptors : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::ControllerWithInterceptors";
public:
  ControllerWithInterceptors(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

  static std::shared_ptr<ControllerWithInterceptors> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<ControllerWithInterceptors>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT_INTERCEPTOR(interceptor, inter1) {

    /* assert order of interception */
    OATPP_ASSERT(request->getHeader("header-in-inter2") == "inter2")
    OATPP_ASSERT(request->getHeader("header-in-inter3") == "inter3")
    /********************************/

    request->putHeader("header-in-inter1", "inter1");
    auto response = (this->*intercepted)(request);
    response->putHeader("header-out-inter1", "inter1");
    return response;

  }
  ENDPOINT_INTERCEPTOR(interceptor, inter2) {

    /* assert order of interception */
    OATPP_ASSERT(request->getHeader("header-in-inter3") == "inter3")
    /********************************/

    request->putHeader("header-in-inter2", "inter2");
    auto response = (this->*intercepted)(request);
    response->putHeader("header-out-inter2", "inter2");
    return response;

  }
  ENDPOINT_INTERCEPTOR(interceptor, inter3) {
    request->putHeader("header-in-inter3", "inter3");
    auto response = (this->*intercepted)(request);
    response->putHeader("header-out-inter3", "inter3");
    return response;
  }
  ENDPOINT_INTERCEPTOR(interceptor, replacer) {
    auto response = (this->*intercepted)(request);
    response->putOrReplaceHeader("to-be-replaced", "replaced_value");
    return response;
  }
  ENDPOINT_INTERCEPTOR(interceptor, asserter) {
    auto response = (this->*intercepted)(request);

    OATPP_ASSERT(response->getHeader("header-out-inter1") == "inter1")
    OATPP_ASSERT(response->getHeader("header-out-inter2") == "inter2")
    OATPP_ASSERT(response->getHeader("header-out-inter3") == "inter3")

    return response;
  }
  ENDPOINT("GET", "test/interceptors", interceptor,
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {

    OATPP_ASSERT(request->getHeader("header-in-inter1") == "inter1")
    OATPP_ASSERT(request->getHeader("header-in-inter2") == "inter2")
    OATPP_ASSERT(request->getHeader("header-in-inter3") == "inter3")

    auto response = createResponse(Status::CODE_200, "Hello World!!!");
    response->putHeader("to-be-replaced", "original_value");
    return response;
  }


#include OATPP_CODEGEN_END(ApiController)

};

}}}}

#endif /* oatpp_test_web_app_ControllerWithInterceptors_hpp */
