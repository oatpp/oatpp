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

#include "HttpRouterTest.hpp"

#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace test { namespace web { namespace server {

namespace {

typedef oatpp::web::server::HttpRouterTemplate<v_int32> NumRouter;

}

void HttpRouterTest::onRun() {

  NumRouter router;
  router.route("GET", "ints/1", 1);
  router.route("GET", "ints/2", 2);
  router.route("GET", "ints/all/{value}", -1);

  router.route("POST", "ints/1", 1);
  router.route("POST", "ints/2", 2);
  router.route("POST", "ints/{value}", 3);
  router.route("POST", "ints/*", 4);
  router.route("POST", "*", -100);

  {
    OATPP_LOGI(TAG, "Case 1");
    auto r = router.getRoute("GET", "ints/1");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 1);
  }

  {
    OATPP_LOGI(TAG, "Case 2");
    auto r = router.getRoute("GET", "/ints/1");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 1);
  }

  {
    OATPP_LOGI(TAG, "Case 3");
    auto r = router.getRoute("GET", "ints/1//");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 1);
  }

  {
    OATPP_LOGI(TAG, "Case 4");
    auto r = router.getRoute("GET", "//ints///1//");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 1);
  }

  {
    OATPP_LOGI(TAG, "Case 5");
    auto r = router.getRoute("GET", "ints/1/*");
    OATPP_ASSERT(r.isValid() == false);
    OATPP_ASSERT(!r);
  }

  {
    OATPP_LOGI(TAG, "Case 6");
    auto r = router.getRoute("GET", "ints/2");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 2);
  }

  {
    OATPP_LOGI(TAG, "Case 7");
    auto r = router.getRoute("GET", "ints/all/10");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == -1);
    OATPP_ASSERT(r.getMatchMap().getVariables().size() == 1);
    OATPP_ASSERT(r.getMatchMap().getVariable("value") == "10");
  }

  {
    OATPP_LOGI(TAG, "Case 8");
    auto r = router.getRoute("GET", "//ints//all//10//");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == -1);
    OATPP_ASSERT(r.getMatchMap().getVariables().size() == 1);
    OATPP_ASSERT(r.getMatchMap().getVariable("value") == "10");
  }

  {
    OATPP_LOGI(TAG, "Case 9");
    auto r = router.getRoute("GET", "//ints//all//10//*");
    OATPP_ASSERT(r.isValid() == false);
    OATPP_ASSERT(!r);
  }

  {
    OATPP_LOGI(TAG, "Case 10");
    auto r = router.getRoute("POST", "ints/1");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 1);
  }

  {
    OATPP_LOGI(TAG, "Case 11");
    auto r = router.getRoute("POST", "ints/2");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 2);
  }

  {
    OATPP_LOGI(TAG, "Case 12");
    auto r = router.getRoute("POST", "ints/3");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 3);
    OATPP_ASSERT(r.getMatchMap().getVariables().size() == 1);
    OATPP_ASSERT(r.getMatchMap().getVariable("value") == "3");
  }

  {
    OATPP_LOGI(TAG, "Case 13");
    auto r = router.getRoute("POST", "ints/3/10");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == 4);
    OATPP_ASSERT(r.getMatchMap().getTail() == "3/10");
  }

  {
    OATPP_LOGI(TAG, "Case 14");
    auto r = router.getRoute("POST", "abc");
    OATPP_ASSERT(r.isValid());
    OATPP_ASSERT(r);
    OATPP_ASSERT(r.getEndpoint() == -100);
    OATPP_ASSERT(r.getMatchMap().getTail() == "abc");
  }

}

}}}}
