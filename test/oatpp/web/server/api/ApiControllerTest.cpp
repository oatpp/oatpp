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

#include "ApiControllerTest.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"

namespace oatpp { namespace test { namespace web { namespace server { namespace api {

namespace {

class Controller : public oatpp::web::server::api::ApiController {
public:
  Controller(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

  static std::shared_ptr<Controller> createShared(const std::shared_ptr<ObjectMapper>& objectMapper){
    return std::make_shared<Controller>(objectMapper);
  }

#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT_INFO(root) {
    info->summary = "root_summary";
    info->addResponse<String>(Status::CODE_200, "text/plain", "test1-success");
    info->addResponse<String>(Status::CODE_404, "text/plain");
  }
  ENDPOINT("GET", "/", root) {
    return createResponse(Status::CODE_200, "test1");
  }

  ENDPOINT_INFO(pathParams) {
    info->pathParams["param1"].description = "this is param1";
    info->queryParams.add<String>("q1").description = "query param";
    info->headers.add<String>("X-TEST-HEADER").description = "TEST-HEADER-PARAM";
  }
  ENDPOINT("GET", "path/{param1}/{param2}", pathParams,
           PATH(String, param1),
           PATH(String, param2)) {
    return createResponse(Status::CODE_200, "test2");
  }

  ENDPOINT_INFO(queryParams) {
    info->queryParams["param1"].description = "this is param1";
  }
  ENDPOINT("GET", "query", queryParams,
           QUERY(String, param1),
           QUERY(String, param2)) {
    return createResponse(Status::CODE_200, "test3");
  }

  ENDPOINT_INFO(noContent) {
      info->addResponse(Status::CODE_204, "No Content");
    }
  ENDPOINT("GET", "noContent", noContent) {
    return createResponse(Status::CODE_204);
  }

#include OATPP_CODEGEN_END(ApiController)

};

}

// methods/fields with "Z__" prefix are for internal use only.
// Do not use these methods/fields for user-tests as naming can be changed

void ApiControllerTest::onRun() {

  typedef oatpp::web::protocol::http::Status Status;

  Controller controller(nullptr);
  oatpp::data::stream::BufferOutputStream headersOutBuffer;

  {
    auto endpoint = controller.Z__ENDPOINT_root;
    OATPP_ASSERT(endpoint)
    OATPP_ASSERT(endpoint->info()->summary == "root_summary")

    auto r200 = endpoint->info()->responses[Status::CODE_200];
    OATPP_ASSERT(r200.contentType == "text/plain")
    OATPP_ASSERT(r200.schema == oatpp::String::Class::getType())
    OATPP_ASSERT(r200.description == "test1-success")

    auto r404 = endpoint->info()->responses[Status::CODE_404];
    OATPP_ASSERT(r404.contentType == "text/plain")
    OATPP_ASSERT(r404.description == "Not Found")
    OATPP_ASSERT(r404.schema == oatpp::String::Class::getType())

    auto response = controller.root();
    OATPP_ASSERT(response->getStatus().code == 200)

    oatpp::data::stream::BufferOutputStream stream;
    response->send(&stream, &headersOutBuffer, nullptr);

    OATPP_LOGd(TAG, "response:\n---\n{}\n---\n", stream.toString())

  }

  {
    auto endpoint = controller.Z__ENDPOINT_pathParams;
    OATPP_ASSERT(endpoint)
    OATPP_ASSERT(!endpoint->info()->summary)

    OATPP_ASSERT(endpoint->info()->pathParams["param1"].name == "param1")
    OATPP_ASSERT(endpoint->info()->pathParams["param1"].description == "this is param1")

    OATPP_ASSERT(endpoint->info()->pathParams["param2"].name == "param2")
    OATPP_ASSERT(!endpoint->info()->pathParams["param2"].description)

    OATPP_ASSERT(endpoint->info()->queryParams["q1"].name == "q1")
    OATPP_ASSERT(endpoint->info()->queryParams["q1"].description == "query param")

    OATPP_ASSERT(endpoint->info()->headers["X-TEST-HEADER"].name == "X-TEST-HEADER")
    OATPP_ASSERT(endpoint->info()->headers["X-TEST-HEADER"].description == "TEST-HEADER-PARAM")

    auto response = controller.pathParams("p1", "p2");
    OATPP_ASSERT(response->getStatus().code == 200)

    oatpp::data::stream::BufferOutputStream stream;
    response->send(&stream, &headersOutBuffer, nullptr);

    OATPP_LOGd(TAG, "response:\n---\n{}\n---\n", stream.toString())

  }

  {
    auto endpoint = controller.Z__ENDPOINT_queryParams;
    OATPP_ASSERT(endpoint)
    OATPP_ASSERT(!endpoint->info()->summary)

    OATPP_ASSERT(endpoint->info()->queryParams["param1"].name == "param1")
    OATPP_ASSERT(endpoint->info()->queryParams["param1"].description == "this is param1")

    OATPP_ASSERT(endpoint->info()->queryParams["param2"].name == "param2")
    OATPP_ASSERT(!endpoint->info()->queryParams["param2"].description)

    auto response = controller.queryParams("p1", "p2");
    OATPP_ASSERT(response->getStatus().code == 200)

    oatpp::data::stream::BufferOutputStream stream;
    response->send(&stream, &headersOutBuffer, nullptr);

    OATPP_LOGd(TAG, "response:\n---\n{}\n---\n", stream.toString())

  }

  {
    auto endpoint = controller.Z__ENDPOINT_noContent;
    OATPP_ASSERT(endpoint)
    OATPP_ASSERT(!endpoint->info()->summary)

    auto r204 = endpoint->info()->responses[Status::CODE_204];
    OATPP_ASSERT(!r204.contentType)
    OATPP_ASSERT(!r204.schema)
    OATPP_ASSERT(r204.description == "No Content")

    auto response = controller.noContent();
    OATPP_ASSERT(response->getStatus().code == 204)
    OATPP_ASSERT(!response->getBody())

    oatpp::data::stream::BufferOutputStream stream;
    response->send(&stream, &headersOutBuffer, nullptr);

    OATPP_LOGd(TAG, "response:\n---\n{}\n---\n", stream.toString())

  }

}

}}}}}
