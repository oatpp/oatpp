/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "FullAsyncTest.hpp"

#include "oatpp/test/web/app/Client.hpp"

#include "oatpp/test/web/app/ControllerAsync.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

namespace oatpp { namespace test { namespace web {
  
bool FullAsyncTest::onRun() {
  
  auto interface = oatpp::network::virtual_::Interface::createShared("virtualhost");
  
  auto serverConnectionProvider = oatpp::network::virtual_::server::ConnectionProvider::createShared(interface);
  auto clientConnectionProvider = oatpp::network::virtual_::client::ConnectionProvider::createShared(interface);
  
  serverConnectionProvider->setSocketMaxAvailableToReadWrtie(1, 1);
  clientConnectionProvider->setSocketMaxAvailableToReadWrtie(1, 1);
  
  auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  auto router = oatpp::web::server::HttpRouter::createShared();
  auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router);
  
  auto controller = app::ControllerAsync::createShared(objectMapper);
  controller->addEndpointsToRouter(router);
  
  auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);
  
  auto client = app::Client::createShared(requestExecutor, objectMapper);
  
  auto server = oatpp::network::server::Server::createShared(serverConnectionProvider, connectionHandler);
  
  std::thread clientThread([client, server, connectionHandler]{
    
    for(v_int32 i = 0; i < 10; i ++) {
      
      try {
        auto response = client->getRoot();
        auto text = response->readBodyToString();
        OATPP_LOGD("client", "body='%s'", text->c_str());
      } catch(std::runtime_error e) {
        OATPP_LOGD("client", "error='%s'", e.what());
      }
      
    }
    
    try {
      connectionHandler->stop();
      server->stop();
      client->getRoot();
      OATPP_LOGD("client", "stoped");
    } catch(std::runtime_error e) {
      OATPP_LOGD("client", "stoped with e");
    }
    
  });
  
  std::thread serverThread([server]{
    server->run();
  });
  
  clientThread.join();
  serverThread.join();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  
  return true;
}
  
}}}
