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

void AuthorizationHandlerTest::onRun() {

  oatpp::String user = "foo";
  oatpp::String password = "bar";
  oatpp::String header = "Basic Zm9vOmJhcg==";

  {
    std::shared_ptr<oatpp::web::server::handler::DefaultAuthorizationHandler> default_authorization_handler = oatpp::web::server::handler::DefaultAuthorizationHandler::createShared();
    std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> auth = default_authorization_handler->handleAuthorization(header);
    OATPP_LOGV(TAG, "header=\"%s\" -> user=\"%s\" password=\"%s\"", header->c_str(), auth->user->c_str(), auth->password->c_str());
    OATPP_ASSERT(auth->user->equals("foo"));
    OATPP_ASSERT(auth->password->equals("bar"));
  }

}

}}}}}
