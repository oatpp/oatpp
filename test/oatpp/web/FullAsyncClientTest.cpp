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

#include "FullAsyncClientTest.hpp"

#include "oatpp/web/app/Client.hpp"

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

#include "oatpp/core/macro/component.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

namespace oatpp { namespace test { namespace web {

namespace {

typedef oatpp::web::protocol::http::incoming::Response IncomingResponse;

class TestComponent {
private:
  v_uint16 m_port;
public:

  TestComponent(v_uint16 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
    return std::make_shared<oatpp::async::Executor>(4, 1, 1);
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

  OATPP_CREATE_COMPONENT(std::shared_ptr<app::Client>, appClient)([] {

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

    auto retryPolicy = std::make_shared<oatpp::web::client::SimpleRetryPolicy>(5, std::chrono::seconds(1));

    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider, retryPolicy);
    return app::Client::createShared(requestExecutor, objectMapper);

  }());

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ClientCoroutine_getRootAsync

class ClientCoroutine_getRootAsync : public oatpp::async::Coroutine<ClientCoroutine_getRootAsync> {
public:
  static std::atomic<v_int32> SUCCESS_COUNTER;
private:
  OATPP_COMPONENT(std::shared_ptr<app::Client>, appClient);
public:

  Action act() override {
    return appClient->getRootAsync().callbackTo(&ClientCoroutine_getRootAsync::onResponse);
  }

  Action onResponse(const std::shared_ptr<IncomingResponse>& response) {
    OATPP_ASSERT(response->getStatusCode() == 200 && "ClientCoroutine_getRootAsync");
    return response->readBodyToStringAsync().callbackTo(&ClientCoroutine_getRootAsync::onBodyRead);
  }

  Action onBodyRead(const oatpp::String& body) {
    OATPP_ASSERT(body == "Hello World Async!!!");
    ++ SUCCESS_COUNTER;
    return finish();
  }

  Action handleError(Error* error) override {
    if(error->is<oatpp::AsyncIOError>()) {
      auto e = static_cast<oatpp::AsyncIOError*>(error);
      OATPP_LOGE("[FullAsyncClientTest::ClientCoroutine_getRootAsync::handleError()]", "AsyncIOError. %s, %d", e->what(), e->getCode());
    } else {
      OATPP_LOGE("[FullAsyncClientTest::ClientCoroutine_getRootAsync::handleError()]", "Error. %s", error->what());
    }
    OATPP_ASSERT(!"Error");
    return error;
  }

};

std::atomic<v_int32> ClientCoroutine_getRootAsync::SUCCESS_COUNTER(0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ClientCoroutine_postBodyAsync

class ClientCoroutine_postBodyAsync : public oatpp::async::Coroutine<ClientCoroutine_postBodyAsync> {
public:
  static std::atomic<v_int32> SUCCESS_COUNTER;
private:
  OATPP_COMPONENT(std::shared_ptr<app::Client>, appClient);
  OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
public:

  Action act() override {
    return appClient->postBodyAsync("my_test_body").callbackTo(&ClientCoroutine_postBodyAsync::onResponse);
  }

  Action onResponse(const std::shared_ptr<IncomingResponse>& response) {
    OATPP_ASSERT(response->getStatusCode() == 200 && "ClientCoroutine_postBodyAsync");
    return response->readBodyToDtoAsync<oatpp::Object<app::TestDto>>(objectMapper).callbackTo(&ClientCoroutine_postBodyAsync::onBodyRead);
  }

  Action onBodyRead(const oatpp::Object<app::TestDto>& body) {
    OATPP_ASSERT(body);
    OATPP_ASSERT(body->testValue == "my_test_body");
    ++ SUCCESS_COUNTER;
    return finish();
  }

  Action handleError(Error* error) override {
    if(error->is<oatpp::AsyncIOError>()) {
      auto e = static_cast<oatpp::AsyncIOError*>(error);
      OATPP_LOGE("[FullAsyncClientTest::ClientCoroutine_postBodyAsync::handleError()]", "AsyncIOError. %s, %d", e->what(), e->getCode());
    } else {
      OATPP_LOGE("[FullAsyncClientTest::ClientCoroutine_postBodyAsync::handleError()]", "Error. %s", error->what());
    }
    OATPP_ASSERT(!"Error");
    return error;
  }

};

std::atomic<v_int32> ClientCoroutine_postBodyAsync::SUCCESS_COUNTER(0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ClientCoroutine_echoBodyAsync

class ClientCoroutine_echoBodyAsync : public oatpp::async::Coroutine<ClientCoroutine_echoBodyAsync> {
public:
  static std::atomic<v_int32> SUCCESS_COUNTER;
private:
  OATPP_COMPONENT(std::shared_ptr<app::Client>, appClient);
  oatpp::String m_data;
public:

  Action act() override {
    oatpp::data::stream::BufferOutputStream stream;
    for(v_int32 i = 0; i < oatpp::data::buffer::IOBuffer::BUFFER_SIZE; i++) {
      stream.writeSimple("0123456789", 10);
    }
    m_data = stream.toString();
    return appClient->echoBodyAsync(m_data).callbackTo(&ClientCoroutine_echoBodyAsync::onResponse);
  }

  Action onResponse(const std::shared_ptr<IncomingResponse>& response) {
    OATPP_ASSERT(response->getStatusCode() == 200 && "ClientCoroutine_echoBodyAsync");
    return response->readBodyToStringAsync().callbackTo(&ClientCoroutine_echoBodyAsync::onBodyRead);
  }

  Action onBodyRead(const oatpp::String& body) {
    OATPP_ASSERT(body == m_data);
    ++ SUCCESS_COUNTER;
    return finish();
  }

  Action handleError(Error* error) override {
    if(error) {
      if(error->is<oatpp::AsyncIOError>()) {
        auto e = static_cast<oatpp::AsyncIOError*>(error);
        OATPP_LOGE("[FullAsyncClientTest::ClientCoroutine_echoBodyAsync::handleError()]", "AsyncIOError. %s, %d", e->what(), e->getCode());
      } else {
        OATPP_LOGE("[FullAsyncClientTest::ClientCoroutine_echoBodyAsync::handleError()]", "Error. %s", error->what());
      }
    }
    OATPP_ASSERT(!"Error");
    return error;
  }

};

std::atomic<v_int32> ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER(0);

}

void FullAsyncClientTest::onRun() {

  TestComponent component(m_port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::ControllerAsync::createShared());

  runner.run([this, &runner] {

    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

    ClientCoroutine_getRootAsync::SUCCESS_COUNTER = 0;
    ClientCoroutine_postBodyAsync::SUCCESS_COUNTER = 0;
    ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER = 0;

    v_int32 iterations = m_connectionsPerEndpoint;

    for(v_int32 i = 0; i < iterations; i++) {
      executor->execute<ClientCoroutine_getRootAsync>();
      executor->execute<ClientCoroutine_postBodyAsync>();
      executor->execute<ClientCoroutine_echoBodyAsync>();
    }

    while(
      ClientCoroutine_getRootAsync::SUCCESS_COUNTER != -1 ||
      ClientCoroutine_postBodyAsync::SUCCESS_COUNTER != -1 ||
      ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER != -1
    ) {

      OATPP_LOGV("Client", "Root=%d, postBody=%d, echoBody=%d",
        ClientCoroutine_getRootAsync::SUCCESS_COUNTER.load(),
        ClientCoroutine_postBodyAsync::SUCCESS_COUNTER.load(),
        ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER.load()
      );

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      if(ClientCoroutine_getRootAsync::SUCCESS_COUNTER == iterations){
        ClientCoroutine_getRootAsync::SUCCESS_COUNTER = -1;
        OATPP_LOGV("Client", "getRootAsync - DONE!");
      }
      if(ClientCoroutine_postBodyAsync::SUCCESS_COUNTER == iterations){
        ClientCoroutine_postBodyAsync::SUCCESS_COUNTER = -1;
        OATPP_LOGV("Client", "postBodyAsync - DONE!");
      }
      if(ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER == iterations){
        ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER = -1;
        OATPP_LOGV("Client", "echoBodyAsync - DONE!");
      }
    }

    OATPP_ASSERT(ClientCoroutine_getRootAsync::SUCCESS_COUNTER == -1); // -1 is success
    OATPP_ASSERT(ClientCoroutine_postBodyAsync::SUCCESS_COUNTER == -1); // -1 is success
    OATPP_ASSERT(ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER == -1); // -1 is success

    executor->waitTasksFinished(); // Wait executor tasks before quit.
    executor->stop();

  }, std::chrono::minutes(10));

  OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
  executor->join();

}

}}}
