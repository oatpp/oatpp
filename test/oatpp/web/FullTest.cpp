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

#include "oatpp/web/app/ControllerWithInterceptors.hpp"
#include "oatpp/web/app/ControllerWithErrorHandler.hpp"
#include "oatpp/web/app/Controller.hpp"
#include "oatpp/web/app/BasicAuthorizationController.hpp"
#include "oatpp/web/app/BearerAuthorizationController.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/json/ObjectMapper.hpp"

#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/data/resource/InMemoryData.hpp"
#include "oatpp/macro/component.hpp"

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

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, virtualInterface)([] {
    return oatpp::network::virtual_::Interface::obtainShared("virtualhost");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {

    if(m_port == 0) { // Use oatpp virtual interface
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
    return oatpp::web::server::HttpConnectionHandler::createShared(router);
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([] {
    return std::make_shared<oatpp::json::ObjectMapper>();
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
  
void FullTest::onRun() {

  TestComponent component(m_port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::Controller::createShared());
  runner.addController(app::ControllerWithInterceptors::createShared());
  runner.addController(app::ControllerWithErrorHandler::createShared());
  runner.addController(app::DefaultBasicAuthorizationController::createShared());
  runner.addController(app::BasicAuthorizationController::createShared());
  runner.addController(app::BearerAuthorizationController::createShared());

  runner.run([this] {

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);
    auto client = app::Client::createShared(requestExecutor, objectMapper);

    auto connection = client->getConnection();
    OATPP_ASSERT(connection)

    v_int32 iterationsStep = m_iterationsPerStep;

    auto lastTick = oatpp::Environment::getMicroTickCount();

    for(v_int32 i = 0; i < iterationsStep * 10; i ++) {

      { // test simple GET
        auto response = client->getRoot(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Hello World!!!")
      }

      { // test simple GET with CORS
        auto response = client->getCors(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Ping")
        auto header = response->getHeader(oatpp::web::protocol::http::Header::CORS_ORIGIN);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "*")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_METHODS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "GET, POST, OPTIONS")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_HEADERS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization")
      }

      { // test simple OPTIONS with CORS
        auto response = client->optionsCors(connection);
        OATPP_ASSERT(response->getStatusCode() == 204)
        auto header = response->getHeader(oatpp::web::protocol::http::Header::CORS_ORIGIN);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "*")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_METHODS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "GET, POST, OPTIONS")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_HEADERS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization")
      }

      { // test simple GET with CORS
        auto response = client->getCorsOrigin(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Pong")
        auto header = response->getHeader(oatpp::web::protocol::http::Header::CORS_ORIGIN);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "127.0.0.1")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_METHODS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "GET, POST, OPTIONS")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_HEADERS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization")
      }

      { // test simple GET with CORS
        auto response = client->getCorsOriginMethods(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Ping")
        auto header = response->getHeader(oatpp::web::protocol::http::Header::CORS_ORIGIN);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "127.0.0.1")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_METHODS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "GET, OPTIONS")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_HEADERS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "DNT, User-Agent, X-Requested-With, If-Modified-Since, Cache-Control, Content-Type, Range, Authorization")
      }

      { // test simple GET with CORS
        auto response = client->getCorsOriginMethodsHeader(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Pong")
        auto header = response->getHeader(oatpp::web::protocol::http::Header::CORS_ORIGIN);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "127.0.0.1")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_METHODS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "GET, OPTIONS")
        header = response->getHeader(oatpp::web::protocol::http::Header::CORS_HEADERS);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "X-PWNT")
      }

      { // test GET with path parameter
        auto response = client->getWithParams("my_test_param", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testValue == "my_test_param")
      }

      { // test GET with query parameters
        auto response = client->getWithQueries("oatpp", 1, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testValue == "name=oatpp&age=1")
      }

      { // test GET with optional query parameters
        auto response = client->getWithOptQueries("oatpp", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testValue == "name=oatpp&age=101")
      }

      { // test GET with query parameters
        auto response = client->getWithQueriesMap("value1", 32, 0.32f, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testMap)
        OATPP_ASSERT(dto->testMap->size() == 3)
        OATPP_ASSERT(dto->testMap["key1"] == "value1")
        OATPP_ASSERT(dto->testMap["key2"] == "32")
        OATPP_ASSERT(dto->testMap["key3"] == oatpp::utils::Conversion::float32ToStr(0.32f))
      }

      { // test GET with header parameter
        auto response = client->getWithHeaders("my_test_header", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testValue == "my_test_header")
      }

      { // test POST with body
        auto response = client->postBody("my_test_body", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testValue == "my_test_body")
      }

      { // test POST with dto body
        auto dtoIn = app::TestDto::createShared();
        dtoIn->testValueInt = i;
        auto response = client->postBodyDto(dtoIn, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dtoOut = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dtoOut)
        OATPP_ASSERT(dtoOut->testValueInt == i)
      }

      { // test Enum as String

        OATPP_ASSERT(oatpp::Enum<app::AllowedPathParams>::getEntries().size() == 2)

        oatpp::Enum<app::AllowedPathParams> v = app::AllowedPathParams::HELLO;
        auto response = client->getHeaderEnumAsString(v, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      { // test Enum as String
        oatpp::Enum<app::AllowedPathParams> v = app::AllowedPathParams::HELLO;
        auto response = client->getHeaderEnumAsNumber(v, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      { // test Big Echo with body
        oatpp::data::stream::BufferOutputStream stream;
        for(v_int32 j = 0; j < oatpp::data::buffer::IOBuffer::BUFFER_SIZE; j++) {
          stream.writeSimple("0123456789", 10);
        }
        auto data = stream.toString();
        auto response = client->echoBody(data, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData)
        OATPP_ASSERT(returnedData == data)
      }

      {
        String bodyIn = "null";
        auto response = client->testBodyIsNull1(bodyIn, connection);
        OATPP_ASSERT(response->getStatusCode() == 400)
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData)
//        OATPP_ASSERT(returnedData == "server=oatpp/" OATPP_VERSION "\n"
//                                     "code=400\n"
//                                     "description=Bad Request\n"
//                                     "message=Missing valid body parameter 'body'\n")
        connection = client->getConnection();
      }

      {
        String bodyIn = "\"null\"";
        auto response = client->testBodyIsNull1(bodyIn, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData)
        OATPP_ASSERT(returnedData == "OK---null")
      }

      {
        String bodyIn = "null";
        auto response = client->testBodyIsNull2(bodyIn, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData)
        OATPP_ASSERT(returnedData == "OK---null")
      }

      {
        String bodyIn = "\"null\"";
        auto response = client->testBodyIsNull2(bodyIn, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData)
        OATPP_ASSERT(returnedData == "OK---\"null\"")
      }

      {
        auto response = client->headerValueSet("   VALUE_1, VALUE_2,  VALUE_3", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      {
        auto response = client->getDefaultHeaders1(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      {
        auto response = client->getDefaultHeaders2("some param", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      { // test custom authorization handler with custom authorization object
        auto response = client->defaultBasicAuthorization("foo:bar", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      { // test call of an endpoint that requiers authorization headers, but we don't send one
        auto response = client->defaultBasicAuthorizationWithoutHeader();
        OATPP_ASSERT(response->getStatusCode() == 401)
        oatpp::String body = response->readBodyToString();
//        OATPP_ASSERT(body == "server=oatpp/" OATPP_VERSION "\n"
//                             "code=401\n"
//                             "description=Unauthorized\n"
//                             "message=Authorization Required\n")
        // should also add the WWW-Authenticate header when Authorization is missing
        auto header = response->getHeader(oatpp::web::protocol::http::Header::WWW_AUTHENTICATE);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "Basic realm=\"default-test-realm\"")
      }

      { // test custom authorization handler with custom authorization object
        auto response = client->customBasicAuthorization("foo:bar", connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      { // test call of an endpoint that requiers authorization headers, but we don't send one
        auto response = client->customBasicAuthorizationWithoutHeader();
        OATPP_ASSERT(response->getStatusCode() == 401)
        oatpp::String body = response->readBodyToString();
//        OATPP_ASSERT(body == "server=oatpp/" OATPP_VERSION "\n"
//                             "code=401\n"
//                             "description=Unauthorized\n"
//                             "message=Authorization Required\n")
        // should also add the WWW-Authenticate header when Authorization is missing
        auto header = response->getHeader(oatpp::web::protocol::http::Header::WWW_AUTHENTICATE);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "Basic realm=\"custom-test-realm\"")
      }

      { // test custom authorization handler with custom authorization object with unknown credentials where the
        // handler returns nullptr
        auto response = client->customBasicAuthorization("john:doe");
        oatpp::String body = response->readBodyToString();
        OATPP_ASSERT(response->getStatusCode() == 401)
//        OATPP_ASSERT(body == "server=oatpp/" OATPP_VERSION "\n"
//                             "code=401\n"
//                             "description=Unauthorized\n"
//                             "message=Unauthorized\n")
        // should also add the WWW-Authenticate header when Authorization is missing or wrong
        auto header = response->getHeader(oatpp::web::protocol::http::Header::WWW_AUTHENTICATE);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "Basic realm=\"custom-test-realm\"")
      }

      { // test custom authorization handler with custom authorization method
        oatpp::String token = "4e99e8c12de7e01535248d2bac85e732";
        auto response = client->bearerAuthorization(token);
        oatpp::String body = response->readBodyToString();
        OATPP_ASSERT(response->getStatusCode() == 200)
      }

      { // test custom authorization handler with custom authorization object with unknown credentials where the
        // handler returns nullptr
        oatpp::String token = "900150983cd24fb0d6963f7d28e17f72";
        auto response = client->bearerAuthorization(token);
        oatpp::String body = response->readBodyToString();
        OATPP_ASSERT(response->getStatusCode() == 401)
//        OATPP_ASSERT(body == "server=oatpp/" OATPP_VERSION "\n"
//                             "code=401\n"
//                             "description=Unauthorized\n"
//                             "message=Unauthorized\n")
        // should also add the WWW-Authenticate header when Authorization is missing or wrong
        auto header = response->getHeader(oatpp::web::protocol::http::Header::WWW_AUTHENTICATE);
        OATPP_ASSERT(header)
        OATPP_ASSERT(header == "Bearer realm=\"custom-bearer-realm\"")
      }

      { // test Chunked body
        oatpp::String sample = "__abcdefghijklmnopqrstuvwxyz-0123456789";
        v_int32 numIterations = 10;
        oatpp::data::stream::BufferOutputStream stream;
        for(v_int32 j = 0; j < numIterations; j++) {
          stream.writeSimple(sample->data(), static_cast<v_buff_size>(sample->size()));
        }
        auto data = stream.toString();
        auto response = client->getChunked(sample, numIterations, connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto returnedData = response->readBodyToString();
        OATPP_ASSERT(returnedData)
        OATPP_ASSERT(returnedData == data)
      }

      { // Multipart body

        std::unordered_map<oatpp::String, oatpp::String> map;
        map["value1"] = "Hello";
        map["value2"] = "World";
        auto multipart = createMultipart(map);

        auto body = std::make_shared<MultipartBody>(multipart);

        auto response = client->multipartTest(i + 1, body);
        OATPP_ASSERT(response->getStatusCode() == 200)

        multipart = std::make_shared<oatpp::web::mime::multipart::PartList>(response->getHeaders());

        oatpp::web::mime::multipart::Reader multipartReader(multipart.get());
        multipartReader.setPartReader("value1", oatpp::web::mime::multipart::createInMemoryPartReader(10));
        multipartReader.setPartReader("value2", oatpp::web::mime::multipart::createInMemoryPartReader(10));

        response->transferBody(&multipartReader);

        OATPP_ASSERT(multipart->getAllParts().size() == 2)
        auto part1 = multipart->getNamedPart("value1");
        auto part2 = multipart->getNamedPart("value2");

        OATPP_ASSERT(part1)
        OATPP_ASSERT(part1->getPayload())

        OATPP_ASSERT(part2)
        OATPP_ASSERT(part2->getPayload())

        OATPP_ASSERT(part1->getPayload()->getInMemoryData() == "Hello")
        OATPP_ASSERT(part2->getPayload()->getInMemoryData() == "World")

      }

      { // test interceptors
        auto response = client->getInterceptors(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Hello World!!!")
      }

      { // test controller's error handler catches
        auto response = client->getCaughtError(connection);
        OATPP_ASSERT(response->getStatusCode() == 418)
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Controller With Errors!")
      }

      { // test header replacement
        auto response = client->getInterceptors(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        OATPP_ASSERT(response->getHeader("to-be-replaced") == "replaced_value")
      }

      if((i + 1) % iterationsStep == 0) {
        auto ticks = oatpp::Environment::getMicroTickCount() - lastTick;
        lastTick = oatpp::Environment::getMicroTickCount();
        OATPP_LOGv("i", "{}, tick={}", i + 1, ticks)
      }

      { // test bundle
        auto response = client->getBundle(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto dto = response->readBodyToDto<oatpp::Object<app::TestDto>>(objectMapper.get());
        OATPP_ASSERT(dto)
        OATPP_ASSERT(dto->testValue == "str-param")
        OATPP_ASSERT(dto->testValueInt == 32000)
      }

      { // test host header
        auto response = client->getHostHeader(connection);
        OATPP_ASSERT(response->getStatusCode() == 200)
        auto value = response->readBodyToString();
        auto host = clientConnectionProvider->getProperty("host");
        OATPP_ASSERT(host)
        OATPP_ASSERT(value == host.toString() + ":" + oatpp::utils::Conversion::int32ToStr(m_port))
      }

    }

  }, std::chrono::minutes(10));

  std::this_thread::sleep_for(std::chrono::seconds(1));

}
  
}}}
