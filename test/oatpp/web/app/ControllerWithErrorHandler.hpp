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

#ifndef oatpp_test_web_app_ControllerWithErrorHandler_hpp
#define oatpp_test_web_app_ControllerWithErrorHandler_hpp

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <sstream>

namespace oatpp { namespace test { namespace web { namespace app {

namespace http = oatpp::web::protocol::http;

/**
 * Custom Error Handler.
 */
class CustomErrorHandler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler {
public:
  /**
   * Constructor.
   */
  CustomErrorHandler() = default;
public:

  /**
   * Create shared DefaultErrorHandler.
   * @return - `std::shared_ptr` to DefaultErrorHandler.
   */
  static std::shared_ptr<CustomErrorHandler> createShared() {
    return std::make_shared<CustomErrorHandler>();
  }

  std::shared_ptr<http::outgoing::Response> handleError(const std::exception_ptr& error) override {
    try {
      std::rethrow_exception(error);
    } catch(const std::runtime_error& e) {
      return oatpp::web::protocol::http::outgoing::ResponseFactory::createResponse(http::Status::CODE_418, e.what());
    } catch(...) {
      throw;
    }
  }

  std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> handleError(const http::Status& status, const oatpp::String& message, const Headers& headers) override {
    throw std::logic_error("Function not implemented");
  }

};

class ControllerWithErrorHandler : public oatpp::web::server::api::ApiController {
private:
  static constexpr const char* TAG = "test::web::app::ControllerWithErrorHandler";
public:
  explicit ControllerWithErrorHandler(const std::shared_ptr<ObjectMapper>& objectMapper)
      : oatpp::web::server::api::ApiController(objectMapper)
  {
    setErrorHandler(CustomErrorHandler::createShared());
  }
public:

  static std::shared_ptr<ControllerWithErrorHandler> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = OATPP_GET_COMPONENT(std::shared_ptr<ObjectMapper>)){
    return std::make_shared<ControllerWithErrorHandler>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT("GET", "test/errorhandling", errorCaught,
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    throw std::runtime_error("Controller With Errors!");
  }


#include OATPP_CODEGEN_END(ApiController)

};

}}}}

#endif /* oatpp_test_web_app_ControllerWithErrorHandler_hpp */
