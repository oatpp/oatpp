/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__/__)(__)  |_|    |_|
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

#include "RequestTimeoutTest.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"

#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"

namespace oatpp { namespace test { namespace web { namespace server {

namespace {

// Handler that sleeps for specified time
class SlowHandler : public oatpp::web::server::HttpRequestHandler {
private:
  v_int64 m_sleepMs;
public:
  SlowHandler(v_int64 sleepMs) : m_sleepMs(sleepMs) {}

  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepMs));
    return ResponseFactory::createResponse(Status::CODE_200, "OK");
  }
};

// Async handler that sleeps for specified time
class AsyncSlowHandler : public oatpp::web::server::HttpRequestHandler {
private:
  v_int64 m_sleepMs;
public:
  AsyncSlowHandler(v_int64 sleepMs) : m_sleepMs(sleepMs) {}

  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
  handleAsync(const std::shared_ptr<IncomingRequest>& request) override {

    class SleepCoroutine : public oatpp::async::CoroutineWithResult<SleepCoroutine, const std::shared_ptr<OutgoingResponse>&> {
    private:
      v_int64 m_sleepMs;
      bool m_slept;
    public:
      SleepCoroutine(v_int64 sleepMs) : m_sleepMs(sleepMs), m_slept(false) {}

      Action act() override {
        if (!m_slept) {
          m_slept = true;
          return oatpp::async::Action::createWaitRepeatAction(
            oatpp::Environment::getMicroTickCount() + m_sleepMs * 1000
          );
        }
        auto response = ResponseFactory::createResponse(Status::CODE_200, "OK");
        return _return(response);
      }
    };

    return SleepCoroutine::startForResult(m_sleepMs);
  }
};

std::shared_ptr<oatpp::network::Server>
runServer(const std::shared_ptr<oatpp::network::ServerConnectionProvider>& connectionProvider, v_int64 timeoutMs) {

  auto router = oatpp::web::server::HttpRouter::createShared();

  router->route("GET", "/fast", std::make_shared<SlowHandler>(100)); // 100ms - should succeed
  router->route("GET", "/slow", std::make_shared<SlowHandler>(5000)); // 5s - should timeout

  auto config = std::make_shared<oatpp::web::server::HttpProcessor::Config>();
  config->requestTimeout = timeoutMs;

  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router, config);

  auto server = std::make_shared<oatpp::network::Server>(connectionProvider, connectionHandler);

  std::thread t([server, connectionHandler] {
    server->run();
    OATPP_LOGd("TEST", "server stopped")
    connectionHandler->stop();
    OATPP_LOGd("TEST", "connectionHandler stopped")
  });
  t.detach();

  return server;
}

std::shared_ptr<oatpp::network::Server>
runAsyncServer(const std::shared_ptr<oatpp::network::ServerConnectionProvider>& connectionProvider, v_int64 timeoutMs) {

  auto router = oatpp::web::server::HttpRouter::createShared();

  router->route("GET", "/fast", std::make_shared<AsyncSlowHandler>(100)); // 100ms - should succeed
  router->route("GET", "/slow", std::make_shared<AsyncSlowHandler>(5000)); // 5s - should timeout

  auto executor = std::make_shared<oatpp::async::Executor>();

  auto config = std::make_shared<oatpp::web::server::HttpProcessor::Config>();
  config->requestTimeout = timeoutMs;

  auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor, config);

  auto server = std::make_shared<oatpp::network::Server>(connectionProvider, connectionHandler);

  std::thread t([server, connectionHandler, executor] {
    server->run();
    OATPP_LOGd("TEST_ASYNC", "server stopped")
    connectionHandler->stop();
    OATPP_LOGd("TEST_ASYNC", "connectionHandler stopped")
    executor->waitTasksFinished();
    executor->stop();
    executor->join();
    OATPP_LOGd("TEST_ASYNC", "executor stopped")
  });
  t.detach();

  return server;
}

void runClient(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider, const oatpp::String& path) {

  oatpp::web::client::HttpRequestExecutor executor(connectionProvider);

  auto response = executor.execute("GET", path, oatpp::web::protocol::http::Headers(), nullptr, nullptr);

  OATPP_ASSERT(response)
  OATPP_ASSERT(response->getStatusCode() == 200 || response->getStatusCode() == 504)
  OATPP_LOGd("TEST", "Path={}, Status={}", path->c_str(), response->getStatusCode())

}

}

void RequestTimeoutTest::onRun() {

  std::shared_ptr<oatpp::network::ServerConnectionProvider> serverConnectionProvider;
  std::shared_ptr<oatpp::network::ClientConnectionProvider> clientConnectionProvider;

  if(m_port == 0) {
    auto _interface = oatpp::network::virtual_::Interface::obtainShared("virtualhost");
    serverConnectionProvider = oatpp::network::virtual_::server::ConnectionProvider::createShared(_interface);
    clientConnectionProvider = oatpp::network::virtual_::client::ConnectionProvider::createShared(_interface);
  } else {
    serverConnectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", m_port});
    clientConnectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({"localhost", m_port});
  }

  v_int64 timeoutMs = 1000; // 1 second timeout

  {
    OATPP_LOGd(TAG, "Run synchronous timeout test on host={}, port={}, timeout={}ms",
               serverConnectionProvider->getProperty("host").toString(),
               serverConnectionProvider->getProperty("port").toString(),
               timeoutMs)

    auto server = runServer(serverConnectionProvider, timeoutMs);

    // Test fast request - should succeed
    runClient(clientConnectionProvider, "/fast");

    // Test slow request - should timeout with 504
    runClient(clientConnectionProvider, "/slow");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server->stop();

    /* wait connection handler to stop */
    std::this_thread::sleep_for(std::chrono::seconds(1));
    OATPP_LOGd(TAG, "Synchronous test DONE")
  }

  {
    OATPP_LOGd(TAG, "Run asynchronous timeout test on host={}, port={}, timeout={}ms",
               serverConnectionProvider->getProperty("host").toString(),
               serverConnectionProvider->getProperty("port").toString(),
               timeoutMs)

    auto server = runAsyncServer(serverConnectionProvider, timeoutMs);

    // Test fast request - should succeed
    runClient(clientConnectionProvider, "/fast");

    // Test slow request - should timeout with 504
    runClient(clientConnectionProvider, "/slow");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server->stop();

    /* wait connection handler to stop */
    std::this_thread::sleep_for(std::chrono::seconds(1));
    OATPP_LOGd(TAG, "Asynchronous test DONE")
  }

}

}}}}
