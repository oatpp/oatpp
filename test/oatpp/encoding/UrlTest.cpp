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

#include "oatpp/utils/Random.hpp"
#include "oatpp/encoding/Url.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace test { namespace encoding {

void UrlTest::onRun(){

  {
    oatpp::encoding::Url::Config config;
    config.spaceToPlus = false;

    for(v_int32 i = 0; i < 100; i++) {
      oatpp::String buff(100);
      utils::Random::randomBytes(reinterpret_cast<p_char8>(buff->data()), static_cast<v_buff_size>(buff->size()));
      auto encoded = oatpp::encoding::Url::encode(buff, config);
      auto decoded = oatpp::encoding::Url::decode(encoded);
      OATPP_ASSERT(decoded == buff)
    }
  }

  {
    oatpp::encoding::Url::Config config;
    config.spaceToPlus = true;

    for(v_int32 i = 0; i < 100; i++) {
      oatpp::String buff(100);
      utils::Random::randomBytes(reinterpret_cast<p_char8>(buff->data()), static_cast<v_buff_size>(buff->size()));
      auto encoded = oatpp::encoding::Url::encode(buff, config);
      auto decoded = oatpp::encoding::Url::decode(encoded);
      OATPP_ASSERT(decoded == buff)
    }
  }

  {
    oatpp::encoding::Url::Config config;
    config.spaceToPlus = false;
    auto encoded = oatpp::encoding::Url::encode(" ", config);
    OATPP_ASSERT_FMT("{}", encoded == "%20")
  }

  {
    oatpp::encoding::Url::Config config;
    config.spaceToPlus = true;
    auto encoded = oatpp::encoding::Url::encode(" ", config);
    OATPP_ASSERT(encoded == "+")
  }

  {
    oatpp::encoding::Url::Config config;
    config.spaceToPlus = false;
    auto encoded = oatpp::encoding::Url::encode("Смачна Овсяночка!", config);
    OATPP_ASSERT_FMT("{}", encoded == "%D0%A1%D0%BC%D0%B0%D1%87%D0%BD%D0%B0%20%D0%9E%D0%B2%D1%81%D1%8F%D0%BD%D0%BE%D1%87%D0%BA%D0%B0%21")
  }

  {
    oatpp::encoding::Url::Config config;
    config.spaceToPlus = true;
    auto encoded = oatpp::encoding::Url::encode("Смачна Овсяночка!", config);
    OATPP_ASSERT_FMT("{}", encoded == "%D0%A1%D0%BC%D0%B0%D1%87%D0%BD%D0%B0+%D0%9E%D0%B2%D1%81%D1%8F%D0%BD%D0%BE%D1%87%D0%BA%D0%B0%21")
  }

}

}}}
