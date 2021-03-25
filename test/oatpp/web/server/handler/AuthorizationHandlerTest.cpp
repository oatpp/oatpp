/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
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

#include "AuthorizationHandlerTest.hpp"

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"

namespace oatpp { namespace test { namespace web { namespace server { namespace handler {

namespace {

class MyBasicAuthorizationObject : public oatpp::web::server::handler::AuthorizationObject {
public:
  oatpp::String userId;
  oatpp::String password;
};

class MyBasicAuthorizationHandler : public oatpp::web::server::handler::BasicAuthorizationHandler {
public:

  std::shared_ptr<AuthorizationObject> authorize(const oatpp::String& userId, const oatpp::String& password) {
    auto authObject = std::make_shared<MyBasicAuthorizationObject>();
    authObject->userId = userId;
    authObject->password = password;
    return authObject;
  }

};

}

void AuthorizationHandlerTest::onRun() {

  oatpp::String user = "foo";
  oatpp::String password = "bar";
  oatpp::String header = "Basic Zm9vOmJhcg==";

  {
    MyBasicAuthorizationHandler basicAuthHandler;
    auto auth = std::static_pointer_cast<MyBasicAuthorizationObject>(basicAuthHandler.handleAuthorization(header));
    OATPP_LOGV(TAG, "header=\"%s\" -> user=\"%s\" password=\"%s\"", header->c_str(), auth->userId->c_str(), auth->password->c_str());
    OATPP_ASSERT(auth->userId == "foo");
    OATPP_ASSERT(auth->password == "bar");
  }

  {
    oatpp::web::server::handler::BasicAuthorizationHandler defaultBasicAuthHandler;
    auto auth = std::static_pointer_cast<oatpp::web::server::handler::DefaultBasicAuthorizationObject>(defaultBasicAuthHandler.handleAuthorization(header));
    OATPP_LOGV(TAG, "header=\"%s\" -> user=\"%s\" password=\"%s\"", header->c_str(), auth->userId->c_str(), auth->password->c_str());
    OATPP_ASSERT(auth->userId == "foo");
    OATPP_ASSERT(auth->password == "bar");
  }

}

}}}}}
