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

#include "BooleanTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/codegen.hpp"

namespace oatpp
{
namespace test
{
namespace parser
{
namespace json
{
namespace mapping
{
void BooleanTest::onRun()
{
   oatpp::parser::json::mapping::ObjectMapper mapper;

   {
      OATPP_LOGI(TAG, "Serialize true to string...");
      auto value = mapper.writeToString(Boolean(true));
      OATPP_ASSERT(value == "true");
      OATPP_LOGI(TAG, "OK");
   }

   {
      OATPP_LOGI(TAG, "Serialize false to string...");
      auto value = mapper.writeToString(Boolean(false));
      OATPP_ASSERT(value == "false");
      OATPP_LOGI(TAG, "OK");
   }

   {
      OATPP_LOGI(TAG, "Deserialize true string...");
      Boolean value = mapper.readFromString<Boolean>("true");
      OATPP_ASSERT(static_cast<bool>(value));
      OATPP_LOGI(TAG, "OK");
   }

   {
      OATPP_LOGI(TAG, "Deserialize false string...");
      Boolean value = mapper.readFromString<Boolean>("false");
      OATPP_ASSERT(!static_cast<bool>(value));
      OATPP_LOGI(TAG, "OK");
   }
}

}  // namespace mapping
}  // namespace json
}  // namespace parser
}  // namespace test
}  // namespace oatpp