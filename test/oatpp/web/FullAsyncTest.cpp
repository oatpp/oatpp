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

#include "FullAsyncTest.hpp"

#include "oatpp/web/app/Client.hpp"

#include "oatpp/web/app/ControllerWithInterceptorsAsync.hpp"
#include "oatpp/web/app/ControllerAsync.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/core/data/resource/InMemoryData.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

namespace oatpp { namespace test { namespace web {

namespace {

typedef oatpp::web::mime::multipart::PartList PartList;
typedef oatpp::web::protocol::http::outgoing::MultipartBody MultipartBody;

class TestComponent {
private:
  v_uint16 m_port;
public:

  TestComponent(v_uint16 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
    return std::make_shared<oatpp::async::Executor>(1, 1, 1);
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, virtualInterface)([] {
    return oatpp::network::virtual_::Interface::obtainShared("virtualhost");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {

    if(m_port == 0) {
      OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, _interface);
      return std::static_pointer_cast<oatpp::network::ServerConnectionProvider>(
        oatpp::network::virtual_::server::ConnectionProvider::createShared(_interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ServerConnectionProvider>(
      oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", m_port})
    );

  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
    return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([] {
    return oatpp::parser::json::mapping::ObjectMapper::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([this] {

    if(m_port == 0) {
      OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, _interface);
      return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
        oatpp::network::virtual_::client::ConnectionProvider::createShared(_interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
      oatpp::network::tcp::client::ConnectionProvider::createShared({"localhost", m_port})
    );

  }());

};

std::shared_ptr<PartList> createMultipart(const std::unordered_map<oatpp::String, oatpp::String>& map) {

  auto multipart = oatpp::web::mime::multipart::PartList::createSharedWithRandomBoundary();

  for(auto& pair : map) {

    oatpp::web::mime::multipart::Headers partHeaders;
    auto part = std::make_shared<oatpp::web::mime::multipart::Part>(partHeaders);
    multipart->writeNextPartSimple(part);
    part->putHeader("Content-Disposition", "form-data; name=\"" + pair.first + "\"");
    part->setPayload(std::make_shared<oatpp::data::resource::InMemoryData>(pair.second));

  }

  return multipart;

}

}
  
void FullAsyncTest::onRun() {

  TestComponent component(m_port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::ControllerAsync::createShared());
  runner.addController(app::ControllerWithInterceptorsAsync::createShared());

  runner.run([this, &runner] {

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);
    auto client = app::Client::createShared(requestExecutor, objectMapper);

    auto connection = client->getConnection();
    OATPP_ASSERT(connection);

    v_int32 iterationsStep = m_iterationsPerStep;

    auto lastTick = oatpp::base::Environment::getMicroTickCount();

    for(v_int32 i = 0; i < iterationsStep * 10; i ++) {

      //OATPP_LOGV("i", "%d", i);

      { // test simple GET
        auto response = client->getRoot(connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Hello World Async!!!");
      }
      
      { // test GET with path parameter
        auto response = client->getWithParams("my_test_param-Async", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_param-Async");
      }
      
      { // test GET with header parameter
        auto response = client->getWithHeaders("my_test_header-Async", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_header-Async");
      }
      
      { // test POST with body
        auto response = client->postBody("my_test_body-Async", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_body-Async");
      }

      { // test Big Echo with body
        oatpp::data::stream::BufferOutputStream stream;
        for(v_int32 i = 0; i < oatpp::data::buffer::IOBuffer::BUFFER_SIZE; i++) {
          stream.writeSimple("0123456789", 10);
        }
        auto data = stream.toString();
        auto response = client->echoBody(data, connection);
        OATPP_ASSERT(response->getStatusCode() == 200);

        auto returnedData = response->readBodyToString();

        OATPP_ASSERT(returnedData);
        OATPP_ASSERT(returnedData == data);
      }

      { // test Chunked body
        oatpp::String sample = "__abcdefghijklmnopqrstuvwxyz-0123456789";
        v_int32 numIterations = 10;
        oatpp::data::stream::BufferOutputStream stream;
        for(v_int32 i = 0; i < numIterations; i++) {
          stream.writeSimple(sample->data(), sample->size());
        }
        auto data = stream.toString();
        auto response = client->getChunked(sample, numIterations, connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData);
        OATPP_ASSERT(returnedData == data);
      }

      { // Multipart body

        std::unordered_map<oatpp::String, oatpp::String> map;
        map["value1"] = "Hello";
        map["value2"] = "World";
        auto multipart = createMultipart(map);

        auto body = std::make_shared<MultipartBody>(multipart);

        auto response = client->multipartTest(i + 1, body);
        OATPP_ASSERT(response->getStatusCode() == 200);

        multipart = std::make_shared<oatpp::web::mime::multipart::PartList>(response->getHeaders());

        oatpp::web::mime::multipart::Reader multipartReader(multipart.get());
        multipartReader.setPartReader("value1", oatpp::web::mime::multipart::createInMemoryPartReader(10));
        multipartReader.setPartReader("value2", oatpp::web::mime::multipart::createInMemoryPartReader(10));

        response->transferBody(&multipartReader);

        OATPP_ASSERT(multipart->getAllParts().size() == 2);
        auto part1 = multipart->getNamedPart("value1");
        auto part2 = multipart->getNamedPart("value2");

        OATPP_ASSERT(part1);
        OATPP_ASSERT(part1->getPayload());

        OATPP_ASSERT(part2);
        OATPP_ASSERT(part2->getPayload());

        OATPP_ASSERT(part1->getPayload()->getInMemoryData() == "Hello");
        OATPP_ASSERT(part2->getPayload()->getInMemoryData() == "World");

      }

      { // test interceptor GET
        auto response = client->getInterceptors(connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Hello World Async!!!");
      }

      { // test host header
        auto response = client->getHostHeader(connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto value = response->readBodyToString();
        auto host = clientConnectionProvider->getProperty("host");
        OATPP_ASSERT(host);
        OATPP_ASSERT(value == host.toString() + ":" + oatpp::utils::conversion::int32ToStr(m_port));
      }

      if((i + 1) % iterationsStep == 0) {
        auto ticks = oatpp::base::Environment::getMicroTickCount() - lastTick;
        lastTick = oatpp::base::Environment::getMicroTickCount();
        OATPP_LOGV("i", "%d, tick=%d", i + 1, ticks);
      }
      
    }

    connection.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

  }, std::chrono::minutes(10));


  OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
  executor->waitTasksFinished();
  executor->stop();
  executor->join();

}
  
}}}
