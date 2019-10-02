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

#ifndef oatpp_test_web_app_ControllerWithInterceptorsAsync_hpp
#define oatpp_test_web_app_ControllerWithInterceptorsAsync_hpp

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <sstream>

namespace oatpp { namespace test { namespace web { namespace app {

namespace multipart = oatpp::web::mime::multipart;

class ControllerWithInterceptorsAsync : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::ControllerWithInterceptorsAsync";
public:
  ControllerWithInterceptorsAsync(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

  static std::shared_ptr<ControllerWithInterceptorsAsync> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<ControllerWithInterceptorsAsync>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT_INTERCEPTOR_ASYNC(Interceptor, inter1) {

    /* assert order of interception */
    OATPP_ASSERT(request->getHeader("header-in-inter2") == "inter2");
    OATPP_ASSERT(request->getHeader("header-in-inter3") == "inter3");
    /********************************/

    request->putHeader("header-in-inter1", "inter1");
    return (this->*intercepted)(request);

  }
  ENDPOINT_INTERCEPTOR_ASYNC(Interceptor, inter2) {

    /* assert order of interception */
    OATPP_ASSERT(request->getHeader("header-in-inter3") == "inter3");
    /********************************/

    request->putHeader("header-in-inter2", "inter2");
    return (this->*intercepted)(request);

  }
  ENDPOINT_INTERCEPTOR_ASYNC(Interceptor, inter3) {

    class IterceptorCoroutine : public oatpp::async::CoroutineWithResult<IterceptorCoroutine, const std::shared_ptr<OutgoingResponse>&> {
    private:
      ControllerWithInterceptorsAsync* m_this;
      Handler<ControllerWithInterceptorsAsync>::MethodAsync m_intercepted;
      std::shared_ptr<IncomingRequest> m_request;
    public:

      IterceptorCoroutine(ControllerWithInterceptorsAsync* _this,
                          const Handler<ControllerWithInterceptorsAsync>::MethodAsync& intercepted,
                          const std::shared_ptr<IncomingRequest>& request)
        : m_this(_this)
        , m_intercepted(intercepted)
        , m_request(request)
      {}

      oatpp::async::Action act() override {
        m_request->putHeader("header-in-inter3", "inter3");
        return (m_this->*m_intercepted)(m_request).callbackTo(&IterceptorCoroutine::onResponse);
      }

      oatpp::async::Action onResponse(const std::shared_ptr<OutgoingResponse>& response) {
        response->putHeader("header-out-inter3", "inter3");
        return this->_return(response);
      }

    };

    return IterceptorCoroutine::startForResult(this, intercepted, request);

  }
  ENDPOINT_INTERCEPTOR_ASYNC(Interceptor, asserter) {

    class IterceptorCoroutine : public oatpp::async::CoroutineWithResult<IterceptorCoroutine, const std::shared_ptr<OutgoingResponse>&> {
    private:
      ControllerWithInterceptorsAsync* m_this;
      Handler<ControllerWithInterceptorsAsync>::MethodAsync m_intercepted;
      std::shared_ptr<IncomingRequest> m_request;
    public:

      IterceptorCoroutine(ControllerWithInterceptorsAsync* _this,
                          const Handler<ControllerWithInterceptorsAsync>::MethodAsync& intercepted,
                          const std::shared_ptr<IncomingRequest>& request)
        : m_this(_this)
        , m_intercepted(intercepted)
        , m_request(request)
      {}

      oatpp::async::Action act() override {
        return (m_this->*m_intercepted)(m_request).callbackTo(&IterceptorCoroutine::onResponse);
      }

      oatpp::async::Action onResponse(const std::shared_ptr<OutgoingResponse>& response) {
        OATPP_ASSERT(response->getHeader("header-out-inter3") == "inter3");
        return this->_return(response);
      }

    };

    return IterceptorCoroutine::startForResult(this, intercepted, request);

  }
  ENDPOINT_ASYNC("GET", "test/interceptors", Interceptor) {

    ENDPOINT_ASYNC_INIT(Interceptor)

    Action act() {

      OATPP_ASSERT(request->getHeader("header-in-inter1") == "inter1");
      OATPP_ASSERT(request->getHeader("header-in-inter2") == "inter2");
      OATPP_ASSERT(request->getHeader("header-in-inter3") == "inter3");

      return _return(controller->createResponse(Status::CODE_200, "Hello World Async!!!"));
    }

  };

#include OATPP_CODEGEN_END(ApiController)

};

}}}}

#endif /* oatpp_test_web_app_ControllerWithInterceptorsAsync_hpp */
