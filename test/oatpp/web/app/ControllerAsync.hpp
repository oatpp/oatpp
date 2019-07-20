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

#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/data/stream/Stream.hpp"
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

  ENDPOINT_ASYNC("GET", "chunked", Chunked) {

    ENDPOINT_ASYNC_INIT(Chunked)

    class ReadCallback : public oatpp::data::stream::AsyncReadCallback {
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

      oatpp::async::Action readAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                           void*& currBufferPtr,
                                           data::v_io_size& bytesLeftToRead,
                                           oatpp::async::Action&& nextAction) override
      {
        if(m_counter < m_iterations) {
          std::memcpy(currBufferPtr, m_text->getData(), m_text->getSize());
          currBufferPtr = &((p_char8) currBufferPtr)[m_text->getSize()];
          bytesLeftToRead -= m_text->getSize();
        }
        m_counter ++;
        return std::forward<oatpp::async::Action>(nextAction);
      }

    };

    Action act() {
      oatpp::String text = "Hello World!!! 0123456789";
      auto body = std::make_shared<oatpp::web::protocol::http::outgoing::ChunkedBody>(nullptr, std::make_shared<ReadCallback>(text, 5), 1024);
      return _return(OutgoingResponse::createShared(Status::CODE_200, body));
    }

  };
  
#include OATPP_CODEGEN_END(ApiController)
  
};
  
}}}}

#endif /* oatpp_test_web_app_ControllerAsync_hpp */
