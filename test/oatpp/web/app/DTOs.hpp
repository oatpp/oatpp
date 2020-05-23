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

#ifndef oatpp_test_web_app_DTOs_hpp
#define oatpp_test_web_app_DTOs_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace test { namespace web { namespace app {

#include OATPP_CODEGEN_BEGIN(DTO)

class TestDto : public oatpp::DTO {
  
  DTO_INIT(TestDto, DTO)
  
  DTO_FIELD(String, testValue);
  DTO_FIELD(Int32, testValueInt);
  DTO_FIELD(Fields<String>, testMap);
  
};

ENUM(AllowedPathParams, v_int32,
  VALUE(HELLO, 100, "hello"),
  VALUE(WORLD, 200, "world")
)

#include OATPP_CODEGEN_END(DTO)
  
}}}}

#endif /* oatpp_test_web_app_DTOs_hpp */
