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

#include "ClientRetryTest.hpp"

#include "oatpp/web/app/Client.hpp"

#include "oatpp/web/app/ControllerWithInterceptors.hpp"
#include "oatpp/web/app/Controller.hpp"
#include "oatpp/web/app/BasicAuthorizationController.hpp"
#include "oatpp/web/app/BearerAuthorizationController.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/network/ConnectionPool.hpp"

#include "oatpp/core/macro/component.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"
#include "oatpp-test/Checker.hpp"

namespace oatpp { namespace test { namespace web {

namespace {

typedef oatpp::web::server::api::ApiController ApiController;

class TestServerComponent {
private:
  v_uint16 m_port;
public:

  TestServerComponent(v_uint16 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {

    if(m_port == 0) { // Use oatpp virtual interface
      auto _interface = oatpp::network::virtual_::Interface::obtainShared("virtualhost");
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
    return oatpp::parser::json::mapping::ObjectMapper::createShared();
  }());

};

class TestClientComponent {
private:
  v_uint16 m_port;
public:

  TestClientComponent(v_uint16 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([this] {

    if(m_port == 0) {
      auto _interface = oatpp::network::virtual_::Interface::obtainShared("virtualhost");
      return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
        oatpp::network::virtual_::client::ConnectionProvider::createShared(_interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
      oatpp::network::tcp::client::ConnectionProvider::createShared({"localhost", m_port})
    );

  }());

};

void runServer(v_uint16 port, v_int32 delaySeconds, v_int32 iterations, bool stable, const std::shared_ptr<app::Controller>& controller) {

  TestServerComponent component(port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(controller);

  runner.run([&runner, delaySeconds, iterations, stable, controller] {

    for(v_int32 i = 0; i < iterations; i ++) {
      std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
      if(!stable) {
        controller->available = !controller->available;
        OATPP_LOGI("Server", "Available=%d", (v_int32)controller->available.load());
      }
    }

  }, std::chrono::minutes(10));

  std::this_thread::sleep_for(std::chrono::seconds(1));

}

}

void ClientRetryTest::onRun() {

  TestClientComponent component(m_port);

  auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  auto controller = app::Controller::createShared(objectMapper);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);

  {

    OATPP_LOGI(TAG, "Test: no server available");
    oatpp::test::PerformanceChecker checker("test: no server available");

    auto retryPolicy = std::make_shared<oatpp::web::client::SimpleRetryPolicy>(2, std::chrono::seconds(1));
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider, retryPolicy);
    auto client = app::Client::createShared(requestExecutor, objectMapper);

    auto response = client->getRoot();
    auto ticks = checker.getElapsedTicks();

    OATPP_LOGD(TAG, "ticks=%d", ticks);

    if(m_port == 0) {

      OATPP_ASSERT(response.get() == nullptr);
      OATPP_ASSERT(ticks >= 2 * 1000 * 1000 /* 2s */);
      OATPP_ASSERT(ticks < 3 * 1000 * 1000 /* 3s */);

    } else {

// TODO - investigate why it takes more than 2 seconds on windows to try to connect to unavailable host
#if !defined(WIN32) && !defined(_WIN32)

      OATPP_ASSERT(response.get() == nullptr);
      OATPP_ASSERT(ticks >= 2 * 1000 * 1000 /* 2s */);
      OATPP_ASSERT(ticks < 3 * 1000 * 1000 /* 3s */);

#endif

    }

  }

  {

    OATPP_LOGI(TAG, "Test: server pops up");
    oatpp::test::PerformanceChecker checker("test: server pops up");

    auto retryPolicy = std::make_shared<oatpp::web::client::SimpleRetryPolicy>(10 * 10, std::chrono::milliseconds(100));
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider, retryPolicy);
    auto client = app::Client::createShared(requestExecutor, objectMapper);

    std::list<std::thread> threads;

    for(v_int32 i = 0; i < 100; i ++) {
      threads.push_back(std::thread([client]{
        auto response = client->getRoot();
        OATPP_ASSERT(response && "Test: server pops up");
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto data = response->readBodyToString();
        OATPP_ASSERT(data == "Hello World!!!");
      }));
    }

    OATPP_LOGD(TAG, "Waiting for server to start...");
    std::this_thread::sleep_for(std::chrono::seconds(3));

    runServer(m_port, 2, 2, true, controller);

    for(std::thread& thread : threads) {
      thread.join();
    }

    auto ticks = checker.getElapsedTicks();
    OATPP_ASSERT(ticks < 10 * 1000 * 1000 /* 10s */);

  }

  {

    OATPP_LOGI(TAG, "Test: unstable server!");

    auto retryPolicy = std::make_shared<oatpp::web::client::SimpleRetryPolicy>(-1, std::chrono::seconds(1));
    auto connectionPool = oatpp::network::ClientConnectionPool::createShared(connectionProvider, 10, std::chrono::seconds(1));
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionPool, retryPolicy);
    auto client = app::Client::createShared(requestExecutor, objectMapper);

    std::list<std::thread> threads;

    std::thread clientThread([client]{

      v_int64 counter = 0;

      v_int64 tick0 = oatpp::base::Environment::getMicroTickCount();

      while(oatpp::base::Environment::getMicroTickCount() - tick0 < 10 * 1000 * 1000) {

        auto response = client->getAvailability();
        OATPP_ASSERT(response && "Test: unstable server!");
        OATPP_ASSERT(response->getStatusCode() == 200);
        auto data = response->readBodyToString();
        OATPP_ASSERT(data == "Hello World!!!");
        counter ++;

        if(counter % 1000 == 0) {
          OATPP_LOGD("client", "requests=%d", counter);
        }

      }

    });

    runServer(m_port, 2, 6, false, controller);

    clientThread.join();
    connectionPool->stop();

  }

  std::this_thread::sleep_for(std::chrono::seconds(2)); // wait connection pool.

}

}}}
