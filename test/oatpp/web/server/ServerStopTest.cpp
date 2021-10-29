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

#include "ServerStopTest.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"

#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"

namespace oatpp { namespace test { namespace web { namespace server {

namespace {

class ReadCallback : public oatpp::data::stream::ReadCallback {
public:

  v_io_size read(void *buffer, v_buff_size count, async::Action &action) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    char *data = (char *) buffer;
    data[0] = 'A';
    return 1;
  }

};

class AsyncReadCallback : public oatpp::data::stream::ReadCallback {
private:
  bool wait = false;
public:

  v_io_size read(void *buffer, v_buff_size count, async::Action &action) override {
    wait = !wait;
    if(wait) {
      action = oatpp::async::Action::createWaitRepeatAction(
        oatpp::base::Environment::getMicroTickCount() + 100 * 1000);
      return oatpp::IOError::RETRY_READ;
    }
    char *data = (char *) buffer;
    data[0] = 'A';
    return 1;
  }

};

class StreamingHandler : public oatpp::web::server::HttpRequestHandler {
public:

  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest> &request) override {
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>
      (std::make_shared<ReadCallback>());
    return OutgoingResponse::createShared(Status::CODE_200, body);
  }

};

class AsyncStreamingHandler : public oatpp::web::server::HttpRequestHandler {
public:

  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse> &>
  handleAsync(const std::shared_ptr<IncomingRequest> &request) {

    class StreamCoroutine
      : public oatpp::async::CoroutineWithResult<StreamCoroutine, const std::shared_ptr<OutgoingResponse> &> {
    public:

      Action act() override {
        auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>
          (std::make_shared<AsyncReadCallback>());
        return _return(OutgoingResponse::createShared(Status::CODE_200, body));
      }

    };

    return StreamCoroutine::startForResult();

  }

};

std::shared_ptr<oatpp::network::Server>
runServer(const std::shared_ptr<oatpp::network::ServerConnectionProvider>& connectionProvider) {

  auto router = oatpp::web::server::HttpRouter::createShared();

  router->route("GET", "/stream", std::make_shared<StreamingHandler>());

  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

  auto server = std::make_shared<oatpp::network::Server>(connectionProvider, connectionHandler);

  std::thread t([server, connectionHandler] {
    server->run();
    OATPP_LOGD("TEST", "server stopped");
    connectionHandler->stop();
    OATPP_LOGD("TEST", "connectionHandler stopped");
  });
  t.detach();

  return server;

}

std::shared_ptr<oatpp::network::Server>
runAsyncServer(const std::shared_ptr<oatpp::network::ServerConnectionProvider>& connectionProvider) {

  auto router = oatpp::web::server::HttpRouter::createShared();
  router->route("GET", "/stream", std::make_shared<AsyncStreamingHandler>());

  auto executor = std::make_shared<oatpp::async::Executor>();

  auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);

  auto server = std::make_shared<oatpp::network::Server>(connectionProvider, connectionHandler);

  std::thread t([server, connectionHandler, executor] {
    server->run();
    OATPP_LOGD("TEST_ASYNC", "server stopped");
    connectionHandler->stop();
    OATPP_LOGD("TEST_ASYNC", "connectionHandler stopped");
    executor->waitTasksFinished();
    executor->stop();
    executor->join();
    OATPP_LOGD("TEST_ASYNC", "executor stopped");
  });
  t.detach();

  return server;

}

void runClient(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider) {

  oatpp::web::client::HttpRequestExecutor executor(connectionProvider);

  auto response = executor.execute("GET", "/stream", oatpp::web::protocol::http::Headers({}), nullptr, nullptr);

  OATPP_ASSERT(response->getStatusCode() == 200);
  auto data = response->readBodyToString();

  OATPP_ASSERT(data)
  OATPP_LOGD("TEST", "data->size() == %d", data->size())

}

}

void ServerStopTest::onRun() {

  std::shared_ptr<oatpp::network::ServerConnectionProvider> serverConnectionProvider;
  std::shared_ptr<oatpp::network::ClientConnectionProvider> clientConnectionProvider;

  if(m_port == 0) {
    auto _interface = oatpp::network::virtual_::Interface::obtainShared("virtualhost");
    serverConnectionProvider = oatpp::network::virtual_::server::ConnectionProvider::createShared(_interface);
    clientConnectionProvider = oatpp::network::virtual_::client::ConnectionProvider::createShared(_interface);
  } else {
    serverConnectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8000});
    clientConnectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({"localhost", 8000});
  }

  {
    OATPP_LOGD(TAG, "Run Simple API test on host=%s, port=%s",
               serverConnectionProvider->getProperty("host").toString()->c_str(),
               serverConnectionProvider->getProperty("port").toString()->c_str())

    auto server = runServer(serverConnectionProvider);
    std::list<std::thread> threads;

    for(v_int32 i = 0; i < 100; i ++) {
      threads.emplace_back(std::thread([clientConnectionProvider]{
        runClient(clientConnectionProvider);
      }));
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    server->stop();

    for(auto& t : threads) {
      t.join();
    }

    /* wait connection handler to stop */
    std::this_thread::sleep_for(std::chrono::seconds(5));
    OATPP_LOGD(TAG, "DONE");
  }

  {
    OATPP_LOGD(TAG, "Run Async API test on host=%s, port=%s",
               serverConnectionProvider->getProperty("host").toString()->c_str(),
               serverConnectionProvider->getProperty("port").toString()->c_str())

    auto server = runAsyncServer(serverConnectionProvider);
    std::list<std::thread> threads;

    for(v_int32 i = 0; i < 100; i ++) {
      threads.emplace_back(std::thread([clientConnectionProvider]{
        runClient(clientConnectionProvider);
      }));
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    server->stop();

    for(auto& t : threads) {
      t.join();
    }

    /* wait connection handler to stop */
    std::this_thread::sleep_for(std::chrono::seconds(5));
    OATPP_LOGD(TAG, "DONE");
  }

}

}}}}
