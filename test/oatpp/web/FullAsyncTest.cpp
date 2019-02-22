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

#include "FullAsyncTest.hpp"

#include "oatpp/web/app/Client.hpp"

#include "oatpp/web/app/ControllerAsync.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"
#include "oatpp/network/server/Server.hpp"

namespace oatpp { namespace test { namespace web {
  
void FullAsyncTest::onRun() {
  
  auto interface = oatpp::network::virtual_::Interface::createShared("virtualhost");
  
  auto serverConnectionProvider = oatpp::network::virtual_::server::ConnectionProvider::createShared(interface, true);
  auto clientConnectionProvider = oatpp::network::virtual_::client::ConnectionProvider::createShared(interface);
  
  serverConnectionProvider->setSocketMaxAvailableToReadWrtie(123, 11);
  clientConnectionProvider->setSocketMaxAvailableToReadWrtie(12421, 21312);

  //serverConnectionProvider->setSocketMaxAvailableToReadWrtie(1, 1);
  //clientConnectionProvider->setSocketMaxAvailableToReadWrtie(1, 1);
  
  auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  auto router = oatpp::web::server::HttpRouter::createShared();
  auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router);
  
  auto controller = app::ControllerAsync::createShared(objectMapper);
  controller->addEndpointsToRouter(router);
  
  auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);
  
  auto client = app::Client::createShared(requestExecutor, objectMapper);
  
  auto server = oatpp::network::server::Server::createShared(serverConnectionProvider, connectionHandler);
  
  std::thread clientThread([client, server, connectionHandler, objectMapper]{
    
    for(v_int32 i = 0; i < 1000; i ++) {
      
      { // test simple GET
        auto response = client->getRoot();
        auto value = response->readBodyToString();
        OATPP_ASSERT(value == "Hello World Async!!!");
      }
      
      { // test GET with path parameter
        auto response = client->getWithParams("my_test_param-Async");
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper);
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_param-Async");
      }
      
      { // test GET with header parameter
        auto response = client->getWithHeaders("my_test_header-Async");
        //auto str = response->readBodyToString();
        //OATPP_LOGE("AAA", "code=%d, str='%s'", response->statusCode, str->c_str());
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper);
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_header-Async");
      }
      
      { // test POST with body
        auto response = client->postBody("my_test_body-Async");
        auto dto = response->readBodyToDto<app::TestDto>(objectMapper);
        OATPP_ASSERT(dto);
        OATPP_ASSERT(dto->testValue == "my_test_body-Async");
      }

      { // test Big Echo with body
        oatpp::data::stream::ChunkedBuffer stream;
        for(v_int32 i = 0; i < oatpp::data::buffer::IOBuffer::BUFFER_SIZE; i++) {
          stream.write("0123456789", 10);
        }
        auto data = stream.toString();
        auto response = client->echoBody(data);

        auto returnedData = response->readBodyToString();

        OATPP_ASSERT(returnedData);
        OATPP_ASSERT(returnedData == data);
      }
      
    }
    
    try {
      connectionHandler->stop();
      server->stop();
      client->getConnection(); // wake blocking server accept
    } catch(std::runtime_error e) {
      // DO NOTHING
    }
    
  });
  
  std::thread serverThread([server]{
    server->run();
  });
  
  clientThread.join();
  serverThread.join();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

}
  
}}}
