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

#include "ContentMappersTest.hpp"

#include "oatpp/web/mime/ContentMappers.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

namespace oatpp::web::mime {

void ContentMappersTest::onRun() {

  protocol::http::incoming::Request request(nullptr, {}, {}, nullptr, nullptr);

  request.putHeader("Content-Type", "application/json");
  request.putHeader("Content-Length", "1000");

  request.putHeader("Accept", "application/*");
  request.putHeader("Accept", "application/json");
  request.putHeader("Accept", "text/html, application/xhtml+xml, application/xml;q=0.9, image/webp, */*;q=0.8");

  auto values = request.getHeaderValues("Accept");

  protocol::http::HeaderValueData data;

  for(auto& v : values) {
    protocol::http::Parser::parseHeaderValueData(data, v, ',');
    OATPP_LOGD(TAG, "value='%s'", v->c_str())
  }

  OATPP_LOGD(TAG, "")

  for(auto& t : data.tokens) {
    OATPP_LOGD(TAG, "token='%s'", t.toString()->c_str())
  }

  for(auto& p : data.titleParams) {
    OATPP_LOGD(TAG, "'%s'='%s'", p.first.toString()->c_str(), p.second.toString()->c_str())
  }

  ContentMappers mappers;

}

}
