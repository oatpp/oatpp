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

#include "HexTest.hpp"

#include "oatpp/encoding/Hex.hpp"

namespace oatpp { namespace encoding {

void HexTest::onRun() {
  oatpp::String original = "Light and powerful C++ web framework for highly scalable and resource-efficient web application. It's zero-dependency and easy-portable.";
  auto encoded = Hex::encode(original);
  auto decoded = Hex::decode(encoded);
  OATPP_LOGd(TAG, "original='{}'", original)
  OATPP_LOGd(TAG, "encoded ='{}'", encoded)
  OATPP_LOGd(TAG, "decoded ='{}'", decoded)
  OATPP_ASSERT(original == decoded)
}

}}
