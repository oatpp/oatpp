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

#ifndef oatpp_test_web_ClientServerTestRunner_hpp
#define oatpp_test_web_ClientServerTestRunner_hpp

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/core/macro/component.hpp"

namespace oatpp { namespace test { namespace web {

class ClientServerTestRunner {
public:
  typedef oatpp::web::server::HttpRouter HttpRouter;
  typedef oatpp::web::server::api::ApiController ApiController;
private:
  OATPP_COMPONENT(std::shared_ptr<HttpRouter>, m_router);
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, m_connectionProvider);
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, m_connectionHandler);
public:

  std::shared_ptr<HttpRouter> getRouter() {
    return m_router;
  }

  void addController(const std::shared_ptr<ApiController>& controller) {
    controller->addEndpointsToRouter(m_router);
  }

  template<typename Lambda>
  void run(const Lambda& lambda) {

    oatpp::network::server::Server server(m_connectionProvider, m_connectionHandler);

    OATPP_LOGD("Server", "Running on port %s...", m_connectionProvider->getProperty("port").toString()->c_str());

    std::thread clientThread([this, &server, &lambda]{

      lambda();

      try {
        server.stop();
        m_connectionHandler->stop();
        m_connectionProvider->close();
      } catch(std::runtime_error e) {
        // DO NOTHING
      }

    });

    std::thread serverThread([&server]{
      server.run();
    });

    clientThread.join();
    serverThread.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  }

};

}}}

#endif //oatpp_test_web_ClientServerTestRunner_hpp
