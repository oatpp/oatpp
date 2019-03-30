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

#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/network/client/SimpleTCPConnectionProvider.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/core/macro/component.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

namespace oatpp { namespace test { namespace web {

namespace {

typedef oatpp::web::protocol::http::incoming::Response IncomingResponse;

//#define OATPP_TEST_USE_PORT 8000

class TestComponent {
public:

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
    return std::make_shared<oatpp::async::Executor>(1);
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, virtualInterface)([] {
    return oatpp::network::virtual_::Interface::createShared("virtualhost");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {
#ifdef OATPP_TEST_USE_PORT
    return oatpp::network::server::SimpleTCPConnectionProvider::createShared(OATPP_TEST_USE_PORT, true /* nonBlocking */);
#else
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, interface);
    return oatpp::network::virtual_::server::ConnectionProvider::createShared(interface, true /* nonBlocking */);
#endif
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
    return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([] {
    return oatpp::parser::json::mapping::ObjectMapper::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([this] {
#ifdef OATPP_TEST_USE_PORT
    return oatpp::network::client::SimpleTCPConnectionProvider::createShared("127.0.0.1", OATPP_TEST_USE_PORT);
#else
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, interface);
    return oatpp::network::virtual_::client::ConnectionProvider::createShared(interface);
#endif
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<app::Client>, appClient)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);
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
    auto callback = static_cast<oatpp::web::client::RequestExecutor::AsyncCallback>(&ClientCoroutine_getRootAsync::onResponse);
    return appClient->getRootAsync(this, callback);
  }

  Action onResponse(const std::shared_ptr<IncomingResponse>& response) {
    OATPP_ASSERT(response->getStatusCode() == 200 && "ClientCoroutine_getRootAsync");
    return response->readBodyToStringAsync(this, &ClientCoroutine_getRootAsync::onBodyRead);
  }

  Action onBodyRead(const oatpp::String& body) {
    OATPP_ASSERT(body == "Hello World Async!!!");
    ++ SUCCESS_COUNTER;
    return finish();
  }

  Action handleError(const std::shared_ptr<const Error>& error) override {
    if(error) {
      OATPP_LOGD("[FullAsyncClientTest::ClientCoroutine_getRootAsync::handleError()]", "Error. %s", error->what());
    }
    return Action::TYPE_ERROR;
  }

};

std::atomic<v_int32> ClientCoroutine_getRootAsync::SUCCESS_COUNTER(0);

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
    oatpp::data::stream::ChunkedBuffer stream;
    for(v_int32 i = 0; i < oatpp::data::buffer::IOBuffer::BUFFER_SIZE; i++) {
      stream.write("0123456789", 10);
    }
    m_data = stream.toString();
    auto callback = static_cast<oatpp::web::client::RequestExecutor::AsyncCallback>(&ClientCoroutine_echoBodyAsync::onResponse);
    return appClient->echoBodyAsync(this, callback, m_data);
  }

  Action onResponse(const std::shared_ptr<IncomingResponse>& response) {
    OATPP_ASSERT(response->getStatusCode() == 200 && "ClientCoroutine_echoBodyAsync");
    return response->readBodyToStringAsync(this, &ClientCoroutine_echoBodyAsync::onBodyRead);
  }

  Action onBodyRead(const oatpp::String& body) {
    OATPP_ASSERT(body == m_data);
    ++ SUCCESS_COUNTER;
    return finish();
  }

  Action handleError(const std::shared_ptr<const Error>& error) override {
    if(error) {
      OATPP_LOGD("[FullAsyncClientTest::ClientCoroutine_echoBodyAsync::handleError()]", "Error. %s", error->what());
    }
    return Action::TYPE_ERROR;
  }

};

std::atomic<v_int32> ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER(0);

}

void FullAsyncClientTest::onRun() {

  TestComponent component;

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::ControllerAsync::createShared());

  OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
  executor->detach();

  runner.run([] {

    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

    ClientCoroutine_getRootAsync::SUCCESS_COUNTER = 0;
    ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER = 0;

    v_int32 iterations = 1;

    for(v_int32 i = 0; i < iterations; i++) {
      executor->execute<ClientCoroutine_getRootAsync>();
      executor->execute<ClientCoroutine_echoBodyAsync>();
    }

    while(
      ClientCoroutine_getRootAsync::SUCCESS_COUNTER != -1 ||
      ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER != -1
    ) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if(ClientCoroutine_getRootAsync::SUCCESS_COUNTER == iterations){
        ClientCoroutine_getRootAsync::SUCCESS_COUNTER = -1;
        OATPP_LOGD("Client", "getRootAsync - DONE!");
      }
      if(ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER == iterations){
        ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER = -1;
        OATPP_LOGD("Client", "echoBodyAsync - DONE!");
      }
    }

    OATPP_ASSERT(ClientCoroutine_getRootAsync::SUCCESS_COUNTER == -1); // -1 is success
    OATPP_ASSERT(ClientCoroutine_echoBodyAsync::SUCCESS_COUNTER == -1); // -1 is success

  }, std::chrono::minutes(10));

}

}}}
