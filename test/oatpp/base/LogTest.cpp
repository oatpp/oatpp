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

//  oatpp::String val = "hello";
//  oatpp::Float64 fv;
//
//  LogMessage msg("{} double={}, float={}");
//
//  //msg << val << fv << "<end>";
//
//  LogMessage::log(0, TAG, msg << 1 << 2 << 3);
//
//  //std::cout << msg.toStdString() << std::endl;

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
