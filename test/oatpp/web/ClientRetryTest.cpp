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

#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/network/client/SimpleTCPConnectionProvider.hpp"

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
  v_int32 m_port;
public:

  TestServerComponent(v_int32 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {

    if(m_port == 0) { // Use oatpp virtual interface
      auto interface = oatpp::network::virtual_::Interface::obtainShared("virtualhost");
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

};

class TestClientComponent {
private:
  v_int32 m_port;
public:

  TestClientComponent(v_int32 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([this] {

    if(m_port == 0) {
      auto interface = oatpp::network::virtual_::Interface::obtainShared("virtualhost");
      return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
        oatpp::network::virtual_::client::ConnectionProvider::createShared(interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
      oatpp::network::client::SimpleTCPConnectionProvider::createShared("127.0.0.1", m_port)
    );

  }());

};

void runServer(v_int32 port, const std::shared_ptr<ApiController>& controller) {

  TestServerComponent component(port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::Controller::createShared());

  runner.run([&runner] {

    std::this_thread::sleep_for(std::chrono::seconds(1));

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop server and unblock accepting thread

    runner.getServer()->stop();
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
    connectionProvider->getConnection();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

  }, std::chrono::minutes(10));

  std::this_thread::sleep_for(std::chrono::seconds(1));

}

}

void ClientRetryTest::onRun() {

  TestClientComponent component(m_port);

  auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  auto controller = app::Controller::createShared(objectMapper);


  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);

  //auto connectionPool = std::make_shared<oatpp::network::ClientConnectionPool>(connectionProvider, 10, std::chrono::seconds(5));

  {

    oatpp::test::PerformanceChecker checker("no server available");

    auto retryPolicy = std::make_shared<oatpp::web::client::SimpleRetryPolicy>(2, std::chrono::seconds(1));
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider, retryPolicy);
    auto client = app::Client::createShared(requestExecutor, objectMapper);

    auto response = client->getRoot();
    auto ticks = checker.getElapsedTicks();

    OATPP_ASSERT(response.get() == nullptr);
    OATPP_ASSERT(ticks >= 2 * 1000 * 1000 /* 2s */);
    OATPP_ASSERT(ticks < 3 * 1000 * 1000 /* 2s */);

  }

  //runServer(m_port, controller);
  //runServer(m_port, controller);

}

}}}
