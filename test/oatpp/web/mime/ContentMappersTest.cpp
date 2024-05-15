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

namespace {

class FakeMapper : public data::mapping::ObjectMapper {
public:

  FakeMapper(const oatpp::String& mimeType, const oatpp::String& mimeSubtype)
    : ObjectMapper(Info(mimeType, mimeSubtype))
  {}


  void write(data::stream::ConsistentOutputStream* stream, const oatpp::Void& variant, data::mapping::ErrorStack& errorStack) const override {
    // DO NOTHING
  }

  oatpp::Void read(oatpp::utils::parser::Caret& caret, const oatpp::Type* type, data::mapping::ErrorStack& errorStack) const override {
    return nullptr;
  }

};

}

void ContentMappersTest::onRun() {

  {
    OATPP_LOGd(TAG, "case 1 - default mapper")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    auto m = mappers.getMapper("APPLICATION/JSON");
    OATPP_ASSERT(m != nullptr)
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
    OATPP_ASSERT(mappers.getDefaultMapper().get() == m.get())
  }

  {
    OATPP_LOGd(TAG, "case 2 - default mapper")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    mappers.putMapper(std::make_shared<FakeMapper>("text", "html"));

    auto m = mappers.getMapper("text/HTML");
    auto d = mappers.getDefaultMapper();
    OATPP_ASSERT(m != nullptr)
    OATPP_ASSERT(m->getInfo().httpContentType == "text/html")
    OATPP_ASSERT(d->getInfo().httpContentType == "application/json")
  }

  {
    OATPP_LOGd(TAG, "case 3 - default mapper")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    mappers.putMapper(std::make_shared<FakeMapper>("text", "html"));

    mappers.setDefaultMapper("text/html");

    auto m = mappers.getMapper("application/JSON");
    auto d = mappers.getDefaultMapper();
    OATPP_ASSERT(m != nullptr)
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
    OATPP_ASSERT(d->getInfo().httpContentType == "text/html")
  }


  {
    OATPP_LOGd(TAG, "case 4 - select mapper")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    mappers.putMapper(std::make_shared<FakeMapper>("text", "html"));
    auto m= mappers.selectMapper(std::vector<oatpp::String>{
        "application/json",
        "text/html",
      }
    );
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json" || m->getInfo().httpContentType == "text/html")
  }

  {
    OATPP_LOGd(TAG, "case 5 - select mapper")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    mappers.putMapper(std::make_shared<FakeMapper>("text", "html"));
    auto m= mappers.selectMapper(std::vector<oatpp::String>{
        "application/json;q=0.9",
        "text/html;q=0.1",
      }
    );
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
  }

  {
    OATPP_LOGd(TAG, "case 6 - select mapper")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    mappers.putMapper(std::make_shared<FakeMapper>("text", "html"));
    auto m= mappers.selectMapper(std::vector<oatpp::String>{
                                   "application/json;q=0.1",
                                   "text/html;q=0.9",
                                 }
    );
    OATPP_ASSERT(m->getInfo().httpContentType == "text/html")
  }

  {
    OATPP_LOGd(TAG, "case 7 - select mapper - corrupted input")
    ContentMappers mappers;
    mappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
    mappers.putMapper(std::make_shared<FakeMapper>("text", "html"));
    auto m= mappers.selectMapper(std::vector<oatpp::String>{
                                   "application/json;<anything>=0.5",
                                   "text/html;q=",
                                 }
    );
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
  }

  ContentMappers richMappers;

  richMappers.putMapper(std::make_shared<FakeMapper>("application", "json"));
  richMappers.putMapper(std::make_shared<FakeMapper>("application", "xml"));
  richMappers.putMapper(std::make_shared<FakeMapper>("application", "octet-stream"));
  richMappers.putMapper(std::make_shared<FakeMapper>("text", "css"));
  richMappers.putMapper(std::make_shared<FakeMapper>("text", "csv"));
  richMappers.putMapper(std::make_shared<FakeMapper>("text", "html"));

  {
    OATPP_LOGd(TAG, "case 8 - select mapper - empty")
    auto m = richMappers.selectMapper(
      ""
      );
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
  }

  {
    OATPP_LOGd(TAG, "case 9 - select mapper - corrupted input")
    auto m = richMappers.selectMapper(
      "application/*;q=0.8, text/*;q=0.9, *.*"
    );
    OATPP_ASSERT(m->getInfo().mimeType == "text")
  }

  {
    OATPP_LOGd(TAG, "case 10 - select mapper ")
    auto m = richMappers.selectMapper(
      "application/*;q=0.8, text/*;q=0.9, */*"
    );
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
  }

  {
    OATPP_LOGd(TAG, "case 11 - select mapper ")
    auto m = richMappers.selectMapper(
      "application/*;q=0.9, text/*;q=0.8"
    );
    OATPP_ASSERT(m->getInfo().httpContentType == "application/json")
  }

}

}
