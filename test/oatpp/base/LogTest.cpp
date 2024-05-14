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

#include "LogTest.hpp"

#include "oatpp/base/Log.hpp"

namespace oatpp::base {

void LogTest::onRun() {

  {
    LogMessage msg("{}");
    msg << "hello";
    OATPP_ASSERT(msg.toStdString() == "hello")
  }

  {
    LogMessage msg("{}");
    msg << oatpp::String("hello");
    OATPP_ASSERT(msg.toStdString() == "hello")
  }

  {
    LogMessage msg("{}");
    msg << std::string("hello");
    OATPP_ASSERT(msg.toStdString() == "hello")
  }

  {
    LogMessage msg("{} {}");
    msg << true << false;
    OATPP_ASSERT(msg.toStdString() == "true false")
  }

  {
    LogMessage msg("{} {}");
    msg << oatpp::Boolean(true) << oatpp::Boolean(false);
    OATPP_ASSERT(msg.toStdString() == "true false")
  }

  {
    LogMessage msg("{} {} {} {} {} {} {} {}");
    msg << v_int8(-8) << v_uint8(8);
    msg << v_int16(-16) << v_uint16(16);
    msg << v_int32(-32) << v_uint32(32);
    msg << v_int64(-64) << v_uint64(64);
    OATPP_ASSERT(msg.toStdString() == "-8 8 -16 16 -32 32 -64 64")
  }


  OATPP_LOGv(TAG, "1={}, 2={}, 3={}", 1, 2, 3)
  OATPP_LOGv(TAG, "empty params")

  OATPP_LOGd(TAG, "1={}, 2={}, 3={}", 1, 2, 3)
  OATPP_LOGd(TAG, "empty params")

  OATPP_LOGi(TAG, "1={}, 2={}, 3={}", 1, 2, 3)
  OATPP_LOGi(TAG, "empty params")

  OATPP_LOGw(TAG, "1={}, 2={}, 3={}", 1, 2, 3)
  OATPP_LOGw(TAG, "empty params")

  OATPP_LOGe(TAG, "1={}, 2={}, 3={}", 1, 2, 3)
  OATPP_LOGe(TAG, "empty params")

}

}
