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

#include "oatpp/web/mime/multipart/FileStreamProvider.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"

#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"

#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

namespace oatpp { namespace test { namespace web { namespace app {

namespace multipart = oatpp::web::mime::multipart;

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

  ENDPOINT_ASYNC("GET", "chunked/{text-value}/{num-iterations}", Chunked) {

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
                                           oatpp::data::stream::AsyncInlineReadData& inlineData,
                                           oatpp::async::Action&& nextAction) override
      {
        if(m_counter < m_iterations) {
          std::memcpy(inlineData.currBufferPtr, m_text->getData(), m_text->getSize());
          inlineData.inc(m_text->getSize());
        }
        m_counter ++;
        return std::forward<oatpp::async::Action>(nextAction);
      }

    };

    Action act() {
      oatpp::String text = request->getPathVariable("text-value");
      auto numIterations = oatpp::utils::conversion::strToInt32(request->getPathVariable("num-iterations")->c_str());

      auto body = std::make_shared<oatpp::web::protocol::http::outgoing::ChunkedBody>
        (nullptr, std::make_shared<ReadCallback>(text, numIterations), 1024);

      return _return(OutgoingResponse::createShared(Status::CODE_200, body));
    }

  };

  ENDPOINT_ASYNC("POST", "test/multipart/{chunk-size}", MultipartTest) {

    ENDPOINT_ASYNC_INIT(MultipartTest)

    v_int32 m_chunkSize;
    std::shared_ptr<oatpp::web::mime::multipart::Multipart> m_multipart;

    Action act() override {

      m_chunkSize = oatpp::utils::conversion::strToInt32(request->getPathVariable("chunk-size")->c_str());

      m_multipart = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());
      auto multipartReader = std::make_shared<oatpp::web::mime::multipart::AsyncReader>(m_multipart);

      multipartReader->setDefaultPartReader(std::make_shared<oatpp::web::mime::multipart::AsyncInMemoryPartReader>(10));

      return request->transferBodyAsync(multipartReader).next(yieldTo(&MultipartTest::respond));

    }

    Action respond() {

      auto responseBody = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(m_multipart, m_chunkSize);
      return _return(OutgoingResponse::createShared(Status::CODE_200, responseBody));

    }

  };


  ENDPOINT_ASYNC("POST", "test/multipart-all", MultipartUpload) {

    ENDPOINT_ASYNC_INIT(MultipartUpload)

    /* Coroutine State */
    std::shared_ptr<multipart::Multipart> m_multipart;

    Action act() override {

      m_multipart = std::make_shared<multipart::Multipart>(request->getHeaders());
      auto multipartReader = std::make_shared<multipart::AsyncReader>(m_multipart);

      /* Configure to read part with name "part1" into memory */
      multipartReader->setPartReader("part1", multipart::createAsyncInMemoryPartReader(256 /* max-data-size */));

      /* Configure to stream part with name "part2" to file */
      multipartReader->setPartReader("part2", multipart::createAsyncFilePartReader("/Users/leonid/Documents/test/my-text-file.tf"));

      /* Configure to read all other parts into memory */
      multipartReader->setDefaultPartReader(multipart::createAsyncInMemoryPartReader(16 * 1024 /* max-data-size */));

      /* Read multipart body */
      return request->transferBodyAsync(multipartReader).next(yieldTo(&MultipartUpload::onUploaded));

    }

    Action onUploaded() {

      /* Print number of uploaded parts */
      OATPP_LOGD("Multipart", "parts_count=%d", m_multipart->count());

      /* Get multipart by name */
      auto part1 = m_multipart->getNamedPart("part1");

      /* Asser part not-null */
      OATPP_ASSERT_HTTP(part1, Status::CODE_400, "part1 is empty");

      /* Print value of "part1" */
      OATPP_LOGD("Multipart", "part1='%s'", part1->getInMemoryData()->c_str());

      /* Get multipart by name */
      auto filePart = m_multipart->getNamedPart("part2");

      /* Asser part not-null */
      OATPP_ASSERT_HTTP(filePart, Status::CODE_400, "part2 is empty");

      auto inputStream = filePart->getInputStream();

      // TODO - process file stream.

      return _return(controller->createResponse(Status::CODE_200, "OK"));

    }

  };

#include OATPP_CODEGEN_END(ApiController)
  
};
  
}}}}

#endif /* oatpp_test_web_app_ControllerAsync_hpp */
