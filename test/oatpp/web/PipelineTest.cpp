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

#include "PipelineTest.hpp"

#include "oatpp/web/app/Controller.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/network/client/SimpleTCPConnectionProvider.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

namespace oatpp { namespace test { namespace web {

namespace {

class TestComponent {
private:
  v_int32 m_port;
public:

  TestComponent(v_int32 port)
    : m_port(port)
  {}

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, virtualInterface)([] {
    return oatpp::network::virtual_::Interface::obtainShared("virtualhost");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([this] {

    if(m_port == 0) { // Use oatpp virtual interface
      OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, interface);
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

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([this] {

    if(m_port == 0) {
      OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, interface);
      return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
        oatpp::network::virtual_::client::ConnectionProvider::createShared(interface)
      );
    }

    return std::static_pointer_cast<oatpp::network::ClientConnectionProvider>(
      oatpp::network::client::SimpleTCPConnectionProvider::createShared("127.0.0.1", m_port)
    );

  }());

};

const char* const SAMPLE_IN =
  "GET / HTTP/1.1\r\n"
  "Connection: keep-alive\r\n"
  "Content-Length: 0\r\n"
  "\r\n";

const char* const SAMPLE_OUT =
  "HTTP/1.1 200 OK\r\n"
  "Content-Length: 14\r\n"
  "Connection: keep-alive\r\n"
  "Server: oatpp/" OATPP_VERSION "\r\n"
  "\r\n"
  "Hello World!!!";

}

void PipelineTest::onRun() {

  TestComponent component(m_port);

  oatpp::test::web::ClientServerTestRunner runner;

  runner.addController(app::Controller::createShared());

  runner.run([this, &runner] {

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

    auto connection = clientConnectionProvider->getConnection();

    std::thread pipeInThread([this, connection] {

      oatpp::data::stream::ChunkedBuffer pipelineStream;

      for (v_int32 i = 0; i < m_pipelineSize; i++) {
        pipelineStream << SAMPLE_IN;
      }

      auto dataToSend = pipelineStream.toString();
      OATPP_LOGD(TAG, "Sending %d bytes", dataToSend->getSize());

      oatpp::data::stream::BufferInputStream inputStream(dataToSend);

      oatpp::data::buffer::IOBuffer ioBuffer;

      OATPP_LOGD("AAA", "pipe tranfer begin");
      auto res = oatpp::data::stream::transfer(&inputStream, connection.get(), 0, ioBuffer.getData(), ioBuffer.getSize());
      OATPP_LOGD("AAA", "pipe tranfer end. %d", res);

    });

    std::thread pipeOutThread([this, connection] {

      connection->setInputStreamIOMode(oatpp::data::stream::IOMode::ASYNCHRONOUS);

      oatpp::data::stream::ChunkedBuffer pipelineStream;

      for (v_int32 i = 0; i < m_pipelineSize; i++) {
        pipelineStream << SAMPLE_OUT;
      }

      oatpp::data::stream::ChunkedBuffer receiveStream;
      oatpp::data::buffer::IOBuffer ioBuffer;

      v_int32 retries = 0;
      oatpp::data::v_io_size readResult;

      while(true) {

        async::Action action; // In this particular case, the action is just ignored.
        readResult = connection->read(ioBuffer.getData(), ioBuffer.getSize(), action);
        //OATPP_LOGD("AAA", "readResult=%d", readResult);
        if(readResult > 0) {
          retries = 0;
          receiveStream.writeSimple(ioBuffer.getData(), readResult);
        } else {
          retries ++;
          if(retries == 50) {
            break;
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

      }

      auto result = receiveStream.toString();
      auto wantedResult = pipelineStream.toString();

      if(result != wantedResult) {

//        if(result->getSize() == wantedResult->getSize()) {
//          for(v_int32 i = 0; i < result->getSize(); i++) {
//            if(result->getData()[i] != wantedResult->getData()[i]) {
//              OATPP_LOGD(TAG, "result0='%s'", result->getData());
//              OATPP_LOGD(TAG, "result='%s'", &result->getData()[i]);
//              OATPP_LOGD(TAG, "wanted='%s'", &wantedResult->getData()[i]);
//              OATPP_LOGD(TAG, "diff-pos=%d", i);
//              break;
//            }
//          }
//        }

        OATPP_LOGD(TAG, "result-size=%d, wanted-size=%d", result->getSize(), wantedResult->getSize());
        OATPP_LOGD(TAG, "last readResult=%d", readResult);

      }

      OATPP_ASSERT(result->getSize() == wantedResult->getSize());
      //OATPP_ASSERT(result == wantedResult); // headers may come in different order on different OSs

    });

    pipeOutThread.join();
    pipeInThread.join();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop server and unblock accepting thread

    runner.getServer()->stop();
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider);
    connectionProvider->getConnection();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

  }, std::chrono::minutes(10));

  std::this_thread::sleep_for(std::chrono::seconds(1));

}

}}}
