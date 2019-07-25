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

#include "FullTest.hpp"

#include "oatpp/web/app/Client.hpp"

#include "oatpp/web/app/Controller.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/network/client/SimpleTCPConnectionProvider.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/core/macro/component.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

namespace oatpp { namespace test { namespace web {

namespace {

typedef oatpp::web::mime::multipart::Multipart Multipart;
typedef oatpp::web::protocol::http::outgoing::MultipartBody MultipartBody;

class TestComponent {
private:
  v_int32 m_port;
public:

  TestComponent(v_int32 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, virtualInterface)([] {
    return oatpp::network::virtual_::Interface::createShared("virtualhost");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {

    if(m_port == 0) { // Use oatpp virtual interface
      OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, interface);
      return std::static_pointer_cast<oatpp::network::ServerConnectionProvider>(
        oatpp::network::virtual_::server::ConnectionProvider::createShared(interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ServerConnectionProvider>(
      oatpp::network::server::SimpleTCPConnectionProvider::createShared(m_port)
    );

  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    return oatpp::web::server::HttpConnectionHandler::createShared(router);
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([] {
    return oatpp::parser::json::mapping::ObjectMapper::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([this] {

    if(m_port == 0) {
      OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, interface);
      return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
        oatpp::network::virtual_::client::ConnectionProvider::createShared(interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
      oatpp::network::client::SimpleTCPConnectionProvider::createShared("127.0.0.1", m_port)
    );

  }());

};

std::shared_ptr<Multipart> createMultipart(const std::unordered_map<oatpp::String, oatpp::String>& map) {

  auto multipart = std::make_shared<oatpp::web::mime::multipart::Multipart>("0--qwerty1234--0");

  for(auto& pair : map) {

    oatpp::web::mime::multipart::Headers partHeaders;
    auto part = std::make_shared<oatpp::web::mime::multipart::Part>(partHeaders);
    multipart->addPart(part);
    part->putHeader("Content-Disposition", "form-data; name=\"" + pair.first + "\"");
    part->setDataInfo(std::make_shared<oatpp::data::stream::BufferInputStream>(pair.second));

  }

  return multipart;

}

}
  
void FullTest::onRun() {

  TestComponent component(m_port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::Controller::createShared());

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

      { // test simple GET
        auto response = client->getRoot(connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Hello World!!!");
      }

      { // test GET with path parameter
        auto response = client->getWithParams("my_test_param", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_param");
      }

      { // test GET with query parameters
        auto response = client->getWithQueries("oatpp", 1, connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "name=oatpp&age=1");
      }

      { // test GET with query parameters
        auto response = client->getWithQueriesMap("value1", 32, 0.32, connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testMap);
        OATPP_ASSERT(dto->testMap->count() == 3);
        OATPP_ASSERT(dto->testMap->get("key1", "") == "value1");
        OATPP_ASSERT(dto->testMap->get("key2", "") == "32");
        OATPP_ASSERT(dto->testMap->get("key3", "") == oatpp::utils::conversion::float32ToStr(0.32));
      }

      { // test GET with header parameter
        auto response = client->getWithHeaders("my_test_header", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_header");
      }

      { // test POST with body
        auto response = client->postBody("my_test_body", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper.get());
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_body");
      }

      { // test Big Echo with body
        oatpp::data::stream::ChunkedBuffer stream;
        for(v_int32 i = 0; i < oatpp::data::buffer::IOBuffer::BUFFER_SIZE; i++) {
          stream.write("0123456789", 10);
        }
        auto data = stream.toString();
        auto response = client->echoBody(data, connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData);
        OATPP_ASSERT(returnedData == data);
      }

      {
        auto response = client->headerValueSet("   VALUE_1, VALUE_2,  VALUE_3", connection);
        OATPP_ASSERT(response->getStatusCode() == 200);
      }

      { // test Chunked body
        oatpp::String sample = "__abcdefghijklmnopqrstuvwxyz-0123456789";
        v_int32 numIterations = 10;
        oatpp::data::stream::ChunkedBuffer stream;
        for(v_int32 i = 0; i < numIterations; i++) {
          stream.write(sample->getData(), sample->getSize());
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

        auto body = std::make_shared<MultipartBody>(multipart, i + 1);

        auto response = client->multipartTest(i + 1, body);
        OATPP_ASSERT(response->getStatusCode() == 200);

        multipart = std::make_shared<oatpp::web::mime::multipart::Multipart>(response->getHeaders());
        oatpp::web::mime::multipart::InMemoryReader multipartReader(multipart.get());
        response->transferBody(&multipartReader);

        OATPP_ASSERT(multipart->getAllParts().size() == 2);
        auto part1 = multipart->getNamedPart("value1");
        auto part2 = multipart->getNamedPart("value2");

        OATPP_ASSERT(part1);
        OATPP_ASSERT(part2);

        OATPP_ASSERT(part1->getInMemoryData() == "Hello");
        OATPP_ASSERT(part2->getInMemoryData() == "World");

      }

      if((i + 1) % iterationsStep == 0) {
        auto ticks = oatpp::base::Environment::getMicroTickCount() - lastTick;
        lastTick = oatpp::base::Environment::getMicroTickCount();
        OATPP_LOGV("i", "%d, tick=%d", i + 1, ticks);
      }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop server and unblock accepting thread

    runner.getServer()->stop();
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
    connectionProvider->getConnection();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

  }, std::chrono::minutes(10));

  std::this_thread::sleep_for(std::chrono::seconds(1));

}
  
}}}
