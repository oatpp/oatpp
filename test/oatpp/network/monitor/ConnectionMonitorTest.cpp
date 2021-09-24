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

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"

#include "oatpp/network/monitor/ConnectionMonitor.hpp"
#include "oatpp/network/monitor/ConnectionMaxAgeChecker.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include <thread>

namespace oatpp { namespace test { namespace network { namespace monitor {

namespace {

class StreamingHandler : public oatpp::web::server::HttpRequestHandler {
public:

  class ReadCallback : public oatpp::data::stream::ReadCallback {
  public:

    v_io_size read(void *buffer, v_buff_size count, async::Action &action) override {
      OATPP_LOGE("TEST", "read(...)")
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      char* data = (char*) buffer;
      data[0] = 'A';
      return 1;
    }

  };

  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>
      (std::make_shared<ReadCallback>());
    return OutgoingResponse::createShared(Status::CODE_200, body);
  }

};

std::shared_ptr<oatpp::network::Server> runServer(const std::shared_ptr<oatpp::network::monitor::ConnectionMonitor>& monitor) {

  auto router = oatpp::web::server::HttpRouter::createShared();
  router->route("GET", "/stream", std::make_shared<StreamingHandler>());

  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

  auto server = std::make_shared<oatpp::network::Server>(monitor, connectionHandler);

  std::thread t([server]{
    server->run();
  });
  t.detach();

  return server;

}

}

void ConnectionMonitorTest::onRun() {

  auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 8000, oatpp::network::Address::IP_4});
  auto monitor = std::make_shared<oatpp::network::monitor::ConnectionMonitor>(connectionProvider);

  monitor->addMetricsChecker(
    std::make_shared<oatpp::network::monitor::ConnectionMaxAgeChecker>(
        std::chrono::seconds(10)
      )
  );

  auto server = runServer(monitor);

  std::this_thread::sleep_for(std::chrono::minutes (10));

  monitor->stop();

}

}}}}
