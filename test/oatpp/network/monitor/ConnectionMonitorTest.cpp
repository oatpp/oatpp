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

#include "ConnectionMonitorTest.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"

#include "oatpp/network/monitor/ConnectionMonitor.hpp"
#include "oatpp/network/monitor/ConnectionMaxAgeChecker.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include <thread>

namespace oatpp { namespace test { namespace network { namespace monitor {

namespace {

class ReadCallback : public oatpp::data::stream::ReadCallback {
public:

  v_io_size read(void *buffer, v_buff_size count, async::Action &action) override {
    OATPP_LOGi("TEST", "read(...)")
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    char* data = reinterpret_cast<char*>(buffer);
    data[0] = 'A';
    return 1;
  }

};

class StreamingHandler : public oatpp::web::server::HttpRequestHandler {
public:

  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>
      (std::make_shared<ReadCallback>());
    return OutgoingResponse::createShared(Status::CODE_200, body);
  }

};

class AsyncStreamingHandler : public oatpp::web::server::HttpRequestHandler {
public:

  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
  handleAsync(const std::shared_ptr<IncomingRequest>& request) override {

    class StreamCoroutine : public oatpp::async::CoroutineWithResult<StreamCoroutine, const std::shared_ptr<OutgoingResponse>&> {
    public:

      Action act() override {
        auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>
          (std::make_shared<ReadCallback>());
        return _return(OutgoingResponse::createShared(Status::CODE_200, body));
      }

    };

    return StreamCoroutine::startForResult();

  }

};

std::shared_ptr<oatpp::network::Server> runServer(const std::shared_ptr<oatpp::network::monitor::ConnectionMonitor>& monitor) {

  auto router = oatpp::web::server::HttpRouter::createShared();
  router->route("GET", "/stream", std::make_shared<StreamingHandler>());

  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

  auto server = std::make_shared<oatpp::network::Server>(monitor, connectionHandler);

  std::thread t([server, connectionHandler]{
    server->run();
    OATPP_LOGd("TEST", "server stopped")
    connectionHandler->stop();
    OATPP_LOGd("TEST", "connectionHandler stopped")
  });
  t.detach();

  return server;

}

std::shared_ptr<oatpp::network::Server> runAsyncServer(const std::shared_ptr<oatpp::network::monitor::ConnectionMonitor>& monitor) {

  auto router = oatpp::web::server::HttpRouter::createShared();
  router->route("GET", "/stream", std::make_shared<AsyncStreamingHandler>());

  auto executor = std::make_shared<oatpp::async::Executor>();

  auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);

  auto server = std::make_shared<oatpp::network::Server>(monitor, connectionHandler);

  std::thread t([server, connectionHandler, executor]{
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

void runClient() {

  auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared(
    {"localhost", 8000});
  oatpp::web::client::HttpRequestExecutor executor(connectionProvider);

  auto response = executor.execute("GET", "/stream", oatpp::web::protocol::http::Headers({}), nullptr, nullptr);

  OATPP_ASSERT(response->getStatusCode() == 200)
  auto data = response->readBodyToString();

  OATPP_ASSERT(data)
  OATPP_LOGd("TEST", "data->size() == {}", data->size())
  OATPP_ASSERT(data->size() < 110) // it should be less than 100. But we put 110 for redundancy

}

void runAsyncClient() {

  class ClientCoroutine : public oatpp::async::Coroutine<ClientCoroutine> {
  private:
    std::shared_ptr<oatpp::web::client::HttpRequestExecutor> m_executor;
    std::shared_ptr<oatpp::network::monitor::ConnectionMonitor> m_monitor;
  public:

    ClientCoroutine() {
      auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared(
        {"localhost", 8000});

      m_monitor = std::make_shared<oatpp::network::monitor::ConnectionMonitor>(connectionProvider);

      m_monitor->addMetricsChecker(
        std::make_shared<oatpp::network::monitor::ConnectionMaxAgeChecker>(
          std::chrono::seconds(5)
        )
      );

      m_executor = oatpp::web::client::HttpRequestExecutor::createShared(m_monitor);
    }

    Action act() override {
      return m_executor->executeAsync("GET", "/stream", oatpp::web::protocol::http::Headers({}), nullptr, nullptr)
        .callbackTo(&ClientCoroutine::onResponse);
    }

    Action onResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& response) {
      OATPP_ASSERT(response->getStatusCode() == 200)
      return response->readBodyToStringAsync().callbackTo(&ClientCoroutine::onBody);
    }

    Action onBody(const oatpp::String& data) {
      OATPP_ASSERT(data)
      OATPP_LOGd("TEST", "data->size() == {}", data->size())
      OATPP_ASSERT(data->size() < 60) // it should be less than 50. But we put 60 for redundancy
      m_monitor->stop();
      return finish();
    }

  };

  auto executor = std::make_shared<oatpp::async::Executor>();

  executor->execute<ClientCoroutine>();

  executor->waitTasksFinished();
  OATPP_LOGd("TEST_ASYNC_CLIENT", "task finished")
  executor->stop();
  OATPP_LOGd("TEST_ASYNC_CLIENT", "executor stopped")
  executor->join();
  OATPP_LOGd("TEST_ASYNC_CLIENT", "done")

}

}

void ConnectionMonitorTest::onRun() {

  auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared(
    {"localhost", 8000});
  auto monitor = std::make_shared<oatpp::network::monitor::ConnectionMonitor>(connectionProvider);

  monitor->addMetricsChecker(
    std::make_shared<oatpp::network::monitor::ConnectionMaxAgeChecker>(
        std::chrono::seconds(10)
      )
  );

  {
    OATPP_LOGd(TAG, "run simple API test")
    auto server = runServer(monitor);
    runClient();
    server->stop();
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  {
    OATPP_LOGd(TAG, "run Async API test")
    auto server = runAsyncServer(monitor);
    runClient();
    server->stop();
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  {
    OATPP_LOGd(TAG, "run Async Client test")
    auto server = runServer(monitor);
    runAsyncClient();
    server->stop();
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  monitor->stop();

}

}}}}
