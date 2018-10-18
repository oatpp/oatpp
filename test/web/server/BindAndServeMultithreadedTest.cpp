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

#include "BindAndServeMultithreadedTest.hpp"

#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/network/client/SimpleTCPConnectionProvider.hpp"

#include "oatpp/core/Types.hpp"

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace test { namespace web { namespace server {
  
namespace {
  
  const v_word16 TEST_SERVER_PORT = 48000;
  bool m_serverRunning = false;
  std::mutex m_testMutex;
  std::condition_variable m_testCondition;
  
  const char* TEST_SERVER_RESPONSE = "<<TEST-RESPONSE>>";
  
  /* typedef for convenience */
  typedef oatpp::web::protocol::http::Status Status;
  typedef oatpp::web::protocol::http::Header Header;
  typedef oatpp::web::protocol::http::incoming::Request Request;
  typedef oatpp::web::protocol::http::outgoing::Response Response;
  
  class Handler : public oatpp::web::url::mapping::Subscriber<std::shared_ptr<Request>, std::shared_ptr<Response>> {
  public:
    
    std::shared_ptr<Response> processUrl(const std::shared_ptr<Request>& request) override {
      auto body = oatpp::web::protocol::http::outgoing::BufferBody::createShared(TEST_SERVER_RESPONSE);
      return Response::createShared(Status::CODE_200, body);
    }
    
    Action processUrlAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                           AsyncCallback callback,
                           const std::shared_ptr<Request>& request) override {
      throw std::runtime_error("this method is not used in the test");
    }
    
  };
  
  
}
  
bool BindAndServeMultithreadedTest::onRun() {
 
  auto connectionProvider = oatpp::network::server::SimpleTCPConnectionProvider::createShared(TEST_SERVER_PORT);
  auto router = oatpp::web::server::HttpRouter::createShared();
  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
  
  /* Handler subscribed to the root URL */
  router->addSubscriber("GET", "/", std::make_shared<Handler>());
  
  /* create server and run server */
  auto server = oatpp::network::server::Server::createShared(connectionProvider, connectionHandler);
  OATPP_LOGD("server", "multithreaded server running on port %d", connectionProvider->getPort());
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Start Server
  std::thread serverThread([server] {
    {
      std::lock_guard<std::mutex> lk(m_testMutex);
      m_serverRunning = true;
      m_testCondition.notify_one();
    }
    server->run();
  });
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Run Test
  std::thread testThread([server] {
    std::unique_lock<std::mutex> lock(m_testMutex);
    while(!m_serverRunning){
      m_testCondition.wait(lock);
    }
    
    auto connectionProvider = oatpp::network::client::SimpleTCPConnectionProvider::createShared("localhost", TEST_SERVER_PORT);
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);
    
    auto response = requestExecutor->execute("GET", "/", nullptr, nullptr);
    OATPP_ASSERT(response);
    String body = response->readBodyToString();
    OATPP_ASSERT(body);
    OATPP_LOGD("test", "response='%s'", body->c_str());
    OATPP_ASSERT(body->equals(TEST_SERVER_RESPONSE));
    
    server->stop();
    
    try{
      requestExecutor->execute("GET", "/", nullptr, nullptr);
    } catch(...) {
      // do nothing
    }
    
  });
  
  serverThread.join();
  testThread.join();
  
  return true;
}
  
}}}}
