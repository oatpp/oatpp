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

#include "UrlTest.hpp"

#include "oatpp/network/Url.hpp"
#include "oatpp/base/Log.hpp"

#include "oatpp-test/Checker.hpp"

namespace oatpp { namespace test { namespace network {

void UrlTest::onRun() {

  typedef oatpp::network::Url Url;

  {
    const char* urlText = "http://root@127.0.0.1:8000/path/to/resource/?q1=1&q2=2";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme && url.scheme == "http")
    OATPP_ASSERT(url.authority.userInfo && url.authority.userInfo == "root")
    OATPP_ASSERT(url.authority.host && url.authority.host == "127.0.0.1")
    OATPP_ASSERT(url.authority.port == 8000)
    OATPP_ASSERT(url.path && url.path == "/path/to/resource/")
    OATPP_ASSERT(url.queryParams.getSize() == 2)
    OATPP_ASSERT(url.queryParams.get("q1") == "1")
    OATPP_ASSERT(url.queryParams.get("q2") == "2")
  }

  {
    const char* urlText = "ftp://root@oatpp.io:8000/path/to/resource?q1=1&q2=2";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme && url.scheme == "ftp")
    OATPP_ASSERT(url.authority.userInfo && url.authority.userInfo == "root")
    OATPP_ASSERT(url.authority.host && url.authority.host == "oatpp.io")
    OATPP_ASSERT(url.authority.port == 8000)
    OATPP_ASSERT(url.path && url.path == "/path/to/resource")
    OATPP_ASSERT(url.queryParams.getSize() == 2)
    OATPP_ASSERT(url.queryParams.get("q1") == "1")
    OATPP_ASSERT(url.queryParams.get("q2") == "2")
  }

  {
    const char* urlText = "https://oatpp.io/?q1=1&q2=2";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme && url.scheme == "https")
    OATPP_ASSERT(url.authority.userInfo == nullptr)
    OATPP_ASSERT(url.authority.host && url.authority.host == "oatpp.io")
    OATPP_ASSERT(url.authority.port == -1)
    OATPP_ASSERT(url.path && url.path == "/")
    OATPP_ASSERT(url.queryParams.getSize() == 2)
    OATPP_ASSERT(url.queryParams.get("q1") == "1")
    OATPP_ASSERT(url.queryParams.get("q2") == "2")
  }

  {
    const char* urlText = "https://oatpp.io/";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme && url.scheme == "https")
    OATPP_ASSERT(url.authority.userInfo == nullptr)
    OATPP_ASSERT(url.authority.host && url.authority.host == "oatpp.io")
    OATPP_ASSERT(url.authority.port == -1)
    OATPP_ASSERT(url.path && url.path == "/")
    OATPP_ASSERT(url.queryParams.getSize() == 0)
  }

  {
    const char* urlText = "https://oatpp.io";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme && url.scheme == "https")
    OATPP_ASSERT(url.authority.userInfo == nullptr)
    OATPP_ASSERT(url.authority.host && url.authority.host == "oatpp.io")
    OATPP_ASSERT(url.authority.port == -1)
    OATPP_ASSERT(url.path == nullptr)
    OATPP_ASSERT(url.queryParams.getSize() == 0)
  }

  {
    const char* urlText = "oatpp.io";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme == nullptr)
    OATPP_ASSERT(url.authority.userInfo == nullptr)
    OATPP_ASSERT(url.authority.host && url.authority.host == "oatpp.io")
    OATPP_ASSERT(url.authority.port == -1)
    OATPP_ASSERT(url.path == nullptr)
    OATPP_ASSERT(url.queryParams.getSize() == 0)
  }

  {
    const char* urlText = "oatpp.io:8000/path";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto url = Url::Parser::parseUrl(urlText);

    OATPP_ASSERT(url.scheme == nullptr)
    OATPP_ASSERT(url.authority.userInfo == nullptr)
    OATPP_ASSERT(url.authority.host && url.authority.host == "oatpp.io")
    OATPP_ASSERT(url.authority.port == 8000)
    OATPP_ASSERT(url.path == "/path")
    OATPP_ASSERT(url.queryParams.getSize() == 0)
  }

  {
    const char* urlText = "?key1=value1&key2=value2&key3=value3";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto params = Url::Parser::parseQueryParams(urlText);
    OATPP_ASSERT(params.getSize() == 3)
    OATPP_ASSERT(params.get("key1") == "value1")
    OATPP_ASSERT(params.get("key2") == "value2")
    OATPP_ASSERT(params.get("key2") == "value2")
  }

  {
    const char *urlText = "?key1=value1&key2&key3=value3";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto params = Url::Parser::parseQueryParams(urlText);
    OATPP_ASSERT(params.getSize() == 3)
    OATPP_ASSERT(params.get("key1") == "value1")
    OATPP_ASSERT(params.get("key2") == "")
    OATPP_ASSERT(params.get("key3") == "value3")
  }

  {
    const char *urlText = "?key1=value1&key2&key3";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto params = Url::Parser::parseQueryParams(urlText);
    OATPP_ASSERT(params.getSize() == 3)
    OATPP_ASSERT(params.get("key1") == "value1")
    OATPP_ASSERT(params.get("key2") == "")
    OATPP_ASSERT(params.get("key3") == "")
  }

  {
    const char *urlText = "label?key1=value1&key2=value2&key3=value3";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto params = Url::Parser::parseQueryParams(urlText);
    OATPP_ASSERT(params.getSize() == 3)
    OATPP_ASSERT(params.get("key1") == "value1")
    OATPP_ASSERT(params.get("key2") == "value2")
    OATPP_ASSERT(params.get("key2") == "value2")
  }

  {
    const char* urlText = "label?key1=value1&key2&key3=value3";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto params = Url::Parser::parseQueryParams(urlText);
    OATPP_ASSERT(params.getSize() == 3)
    OATPP_ASSERT(params.get("key1") == "value1")
    OATPP_ASSERT(params.get("key2") == "")
    OATPP_ASSERT(params.get("key3") == "value3")
  }

  {
    const char* urlText = "label?key1=value1&key2&key3";
    OATPP_LOGv(TAG, "urlText='{}'", urlText)
    auto params = Url::Parser::parseQueryParams(urlText);
    OATPP_ASSERT(params.getSize() == 3)
    OATPP_ASSERT(params.get("key1") == "value1")
    OATPP_ASSERT(params.get("key2") == "")
    OATPP_ASSERT(params.get("key3") == "")
  }

}

}}}
